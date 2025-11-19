/* systemcalls.c - Implements system call functions
 * vim:ts=4 noexpandtab
 */
#include "lib.h"
#include "types.h"
#include "pcb.h"
#include "rtc.h"
#include "filesystem.h"
#include "terminal.h"
#include "paging.h"
#include "x86_desc.h"
#include "systemcalls.h"

static int32_t bad_call();
extern int32_t system_execute(const uint8_t * command);

uint32_t set_exception = 0;
static uint8_t total_pid = 0;
static uint8_t pid_flags[MAX_PROGRAM] = {FREE,FREE,FREE,
                                         FREE,FREE,FREE};

/* file operation tables */
static file_ops_t rtc_table = {rtc_read,rtc_write,rtc_open,rtc_close};
static file_ops_t file_table = {file_read,file_write,file_open,file_close};
static file_ops_t dir_table = {dir_read,dir_write,dir_open,dir_close};
static file_ops_t stdin_table = {terminal_read,bad_call,bad_call,bad_call};
static file_ops_t stdout_table = {bad_call,terminal_write,bad_call,bad_call};
static file_ops_t default_table = {bad_call,bad_call,bad_call,bad_call};

/*
 * system_read
 * DESCRIPTION: Calls the read function for the given file descriptor
 *       INPUT: fd - file descriptor
 *              buf - buffer the system will put data into
 *              nbytes - number of bytes to be read
 *      RETURN: number of bytes read
 */
int32_t system_read(int32_t fd, void* buf, int32_t nbytes) {
  if((fd < STD_IN_FD) || (fd >= FILE_ARRAY_LENGTH)) {
      return FAILURE;
  }
  return pcb->file_desc_table[fd].ops_table->op_read(fd,buf,nbytes);
}

/*
 * system_write
 * DESCRIPTION: Calls the write function asscoiated with file descriptor
 *              fd. IF the write function exists for the file descriptor
 *              index, the contents of the write populate the buffer.
 *       INPUT: fd - file descriptor
 *              buf - buffer the system will write data from
 *              nbytes - number of bytes to be written
 *      RETURN: nuber of byts written upon success
 */
int32_t system_write(int32_t fd, const void* buf, int32_t nbytes) {
  if((fd < STD_IN_FD) || (fd >= FILE_ARRAY_LENGTH)) {
      return FAILURE;
  }
  return pcb->file_desc_table[fd].ops_table->op_write(fd,buf,nbytes);
}

/*
 * system_open
 * DESCRIPTION: Opens a directory, file, or device if the inputted
 *              filename is valid
 *       INPUT: filename - pointer to filename
 *      RETURN: file descriptor upon success
 */
int32_t system_open(const uint8_t* filename) {

  dentry_t dentry;
  int32_t fd;

  if(read_dentry_by_name(filename,&dentry) == FAILURE)
    return FAILURE;

  switch(dentry.filetype) {
    case(TYPE_RTC) :
      fd = rtc_table.op_open(filename);
      if(fd == FAILURE) {return FAILURE;}
      pcb->file_desc_table[fd].ops_table = &rtc_table;
      pcb->file_desc_table[fd].flags = IN_USE;
      break;
    case(TYPE_DIR) :
      fd = dir_table.op_open(filename);
      if(fd == FAILURE) {return FAILURE;}
      pcb->file_desc_table[fd].ops_table = &dir_table;
      pcb->file_desc_table[fd].flags = IN_USE;
      break;
    case(TYPE_FILE) :
      fd = file_table.op_open(filename);
      if(fd == FAILURE) {return FAILURE;}
      pcb->file_desc_table[fd].ops_table = &file_table;
      pcb->file_desc_table[fd].flags     = IN_USE;
      break;
  }
  return fd;
}

/*
 * system_close
 * DESCRIPTION: Closes the file descriptor fd by marking the
 *              entry as unused and calls the corresponding
 *              close function.
 *       INPUT: fd - file descriptor
 *      RETURN: 0 upon success and -1 upon failure
 */
int32_t system_close(int32_t fd) {

  // Can't close stdin, stdout, or outside 2~7
  if((fd == STD_OUT_FD) || (fd <= STD_IN_FD) || (fd >= FILE_ARRAY_LENGTH))
  {
      return FAILURE;
  }
  if(pcb->file_desc_table[fd].flags == UNUSED) {
    return FAILURE;
  }
  pcb->file_desc_table[fd].ops_table->op_close(fd);
  pcb->file_desc_table[fd].ops_table = &default_table;
  pcb->file_desc_table[fd].flags = UNUSED;
  return SUCCESS;
}

/*
 * system_getargs
 * DESCRIPTION: Writes program arguments into buf according
 *              to the number of bytes specified
 *       INPUT: buf - user level buffer to be written to
 *      RETURN: -1 upon failure and 0 upon success
 */
int32_t system_getargs(uint8_t* buf, int32_t nbytes) {
  uint32_t i;

  if((*(pcb->args) == '\0') || (nbytes < pcb->args_length))
    return FAILURE;

  for(i = 0; (i < nbytes) && (pcb->args[i] != '\n'); ++i) {
    buf[i] = pcb->args[i];
  }
  return SUCCESS;
}

/*
 * system_vidmap
 * DESCRIPTION: allocate user video memory and give it
 *              to a user ptr
 *       INPUT: ptr to a user ptr
 *      RETURN: 0 upon success; -1 upon failure
 */
int32_t system_vidmap( uint8_t ** screen_start){
  if (screen_start == NULL){
    return FAILURE;
  }
  /* check to see if the ptr is within bounds*/
  if ((uint32_t) screen_start < U_128_MB || (uint32_t) screen_start > U_132MB){
    return FAILURE;
  }
  // set vidmap flag for current terminal
  curr_term->vidmap_flag = PRESENT;
  // call user video page allocation
  user_vid_paging();
  // assign user video page to a user ptr 
  *screen_start = (uint8_t *) U_140MB;
  return SUCCESS;

}

/*
 * bad_call
 * DESCRIPTION: Stand in function for file ops table when no valid
 *              function is available (such as write for the read-only
 *              file system)
 *       INPUT: NONE
 *      RETURN: -1, FAILURE
 */
int32_t bad_call() {
  return FAILURE;
}

/*
 * _system_execute
 * DESCRIPTION: Helper function which checks the validity of
 *              executables, creates a new PCB for a new
 *              executable, and sets up the paging for a new
 *              program
 *       INPUT: command - executable command
 *      RETURN: 0 upon success; -1 upon failure
 */
int32_t _system_execute(const uint8_t* command) {

  uint8_t executable[MAX_FNAME];
  uint32_t command_length = 0;
  uint32_t args_length = 0;
  int32_t exec_check = 0;
  dentry_t dentry;
  int32_t i;
  uint8_t temp_pid;
  uint8_t buf[SMALL_BUF];
  pcb_t pcb_temp;

  if(command == NULL)
    return FAILURE;

  /* read in command */
  while(*command == ' ') {
    ++command;
  }
  while((*command != ' ') && (command_length < MAX_FNAME)) {
    executable[command_length++] = *command;
    if(*command == '\0')
      break;
    ++command;
  }
  if(*command == ' ') {
    executable[command_length++] = '\0';
  }
  /*******************/

  /* read in argument and store in PCB */
  while(*command == ' ') {
    ++command;
  }
  while(args_length < ARGS_SIZE) {
    pcb_temp.args[args_length++] = *command;
    if(*command++ == '\0')
      break;
  }
  pcb_temp.args_length = args_length;
  /************************************/

  // look for executable
  if(read_dentry_by_name(executable,&dentry) == FAILURE) {
    return FAILURE;
  }

  // see if the inputted file is an executable
  read_data(dentry.inode_num,NO_OFFSET,buf,SMALL_BUF);
  while(buf[exec_check] != 'E') {
    exec_check++;
  }
  if(!((buf[exec_check] =='E') && (buf[exec_check+L_OFFSET] == 'L') && (buf[exec_check+F_OFFSET] == 'F'))) {
    return FAILURE;
  }

  if (pcb != NULL) {
    // if there already is 6 process running
    if (total_pid >= MAX_PROGRAM) {
      printf("Max program number is reached. Try exiting now! \n");
      // returning success to prevent the 'no such command' error message
      return 1;
    }
  }

  // setup paging for new program
  if(curr_term->curr_pcb == NULL) {
    temp_pid = curr_term->term_num;
    user_paging(temp_pid);
    pid_flags[temp_pid] = WAITING;
  }
  else {
    for(temp_pid = BASE_SHELLS; temp_pid < MAX_PROGRAM; ++temp_pid) {
      if(pid_flags[temp_pid] == FREE) {
        pid_flags[temp_pid] = WAITING;
        user_paging(temp_pid);
        break;
      }
    }
  }

  // copy progam data into new page
  read_data(dentry.inode_num, 0, (void *) PROGRAM_ADDR, BIG_FILE);

  // setup STDIN and STDOUT in file descriptor array
  pcb_temp.file_desc_table[STD_IN_FD].ops_table = &stdin_table;
  pcb_temp.file_desc_table[STD_IN_FD].flags = IN_USE;
  pcb_temp.file_desc_table[STD_OUT_FD].ops_table = &stdout_table;
  pcb_temp.file_desc_table[STD_OUT_FD].flags = IN_USE;

  // initiliaze rest of file descriptor array
  for(i = FILE_ARRAY_START; i < FILE_ARRAY_LENGTH; ++i) {
    pcb_temp.file_desc_table[i].ops_table = &default_table;
    pcb_temp.file_desc_table[i].flags = UNUSED;
  }

  // assign PID, terminal number, and stack pointer
  if(pcb == NULL) {
    pcb_temp.pid = temp_pid;
    pcb_temp.parent_pcb = BASE_SHELL;
  }
  else {
    pcb_temp.pid = temp_pid;
    if(pcb_temp.pid == TERMINAL_1) { 
      pcb_temp.parent_pcb = BASE_SHELL;
    }
    else if(pcb_temp.pid == TERMINAL_2) {
      pcb_temp.parent_pcb = BASE_SHELL;
    }
    else {
      pcb_temp.parent_pcb = pcb;
    }
    // store TSS stack pointer in new PCB
    pcb_temp.parent_esp0 = tss.esp0;
  }

  total_pid++;

  pcb = (pcb_t*) (EIGHT_MB-(temp_pid+1)*EIGHT_KB);
  memcpy(pcb,&pcb_temp,sizeof(pcb_t));

  // update TSS stack pointer for new program
  pcb->esp0 = tss.esp0 = EIGHT_MB-pcb_temp.pid*EIGHT_KB - FAULT_OFFSET;

 // set current pcb of current terminal
  curr_term->curr_pcb = pcb;
  return SUCCESS;
}

/*
 * _system_halt
 * DESCRIPTION: Helper function which clears the file descriptor array,
 *              returns parent data to its former state from the PCB,
 *              and returns parent paging to its former status.
 *       INPUT: fd - file descriptor
 *      RETURN: 0 upon success; otherwise an exit on the
 *              first shell was called, and the shell
 *              is executed again
 */
int32_t _system_halt(uint8_t status) {
  int32_t fd;
  int32_t ret_status;
  for(fd = FILE_ARRAY_START; fd < FILE_ARRAY_LENGTH; ++fd) {
    pcb->file_desc_table[fd].ops_table->op_close(fd);
  }
  pid_flags[pcb->pid] = FREE;
  tss.esp0 = pcb->parent_esp0;
  pcb = (pcb_t *) pcb->parent_pcb;
  curr_term->curr_pcb = pcb;
  total_pid--;
  if(pcb == NULL) {
    // reboot the shell
    system_execute((uint8_t *) "shell");
  }
  user_paging(pcb->pid);
  if(curr_term->vidmap_flag){
    curr_term->vidmap_flag = NOT_PRESENT;
    user_vid_unpaging();
  }

  // if exception is 1 then return 256
  if (set_exception){
    set_exception = 0;
    ret_status = 256;
    return ret_status;
  }
  // else extend the 8 bit value
  else {
    ret_status = (int32_t) status;
    return ret_status;
  }
}

/*
 * set_exception_one
 * DESCRIPTION: Helper function which set the exception to 1
 *       INPUT: none
 *      RETURN: none
 */
void set_exception_one(){
  set_exception = 1 ;
  return;
}
