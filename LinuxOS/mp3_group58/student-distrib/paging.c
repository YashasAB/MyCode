#include "paging.h"
#include "types.h"

// create PDE and PTE arrays make sure they are aligned
uint32_t page_directory[oneKB] __attribute__ ((aligned (fourKB)));
uint32_t page_table[oneKB] __attribute__ ((aligned (fourKB)));
uint32_t vid_page_table[oneKB] __attribute__ ((aligned (fourKB)));

/*
  function : paging_init()
  effects: set up paging for kernel and video_mem
*/
void paging_init(){
  page_directory[0] = (uint32_t) page_table | RW_P_S;
  // r/w enabled, present, supervisor
  page_directory[KERNEL_PAGE] = fourMB | RW_P_S_MB;
  // r/w enabled, present, supervisor, 4MB page
  int i;
  for (i = 2; i <oneKB; i++){
    page_directory[i] = RW_NP; // r/w enabled, not present
  }
  for( i =0; i<oneKB; i++){
    switch(i) {

      // main video memory
      case video_mem_entry :
        page_table[i] = i*fourKB | RW_P_S;
        break;

      // terminal 0 video memory
      case video_term_0 :
        page_table[i] = i*fourKB | RW_P_S;
        break;

      // terminal 1 video memory
      case video_term_1 :
        page_table[i] = i*fourKB | RW_P_S;
        break;

      // terminal 2 video memory
      case video_term_2 :
        page_table[i] = i*fourKB | RW_P_S;
        break;

      // no page
      default :
        page_table[i] = i*fourKB | RW_NP;
        break;
    }
  }
  /*
  set the PSE flag in CR4 to enable 4MB page
  set the address of page_directory in CR3
  set the 31-bit in CR0 to enable paging
  */
  asm volatile (
      "movl %%cr4, %%eax ;"
      "orl $0x00000010, %%eax ;"
      "movl %%eax, %%cr4 ;"
      "movl %0, %%eax ;"
      "movl %%eax, %%cr3 ;"
      "movl %%cr0, %%eax ;"
      "orl $0x80000000, %%eax ;"
      "movl %%eax, %%cr0 ;"
       :
       : "r" (page_directory)
       :"memory", "cc", "eax"

    );

}
/*
  function : user_paging()
  effects: set up paging for user program
*/
void user_paging(int32_t pid) {
  // map the virtual memory to the corresponding physical mem
  page_directory[USER_128_MB] = (eightMB + pid*fourMB) | RW_P_U_MB;
  asm volatile (
      "movl %%cr3, %%eax ;"
      "movl %%eax, %%cr3 ;"
      :
      :
      :"eax"
    ); // flush TLB
}

/*
 * function : user_vid_paging()
 * effects: set up paging for user video memory
 */
void user_vid_paging(){
  uint32_t i;
  // read_write, present, and user privilege
  page_directory[USER_V_140MB] = (uint32_t) vid_page_table | RW_P_USR;
  vid_page_table[0] = video_mem | RW_P_USR;
  for (i = 1; i <oneKB; i++){
    vid_page_table[i] = RW_NP;
  }

  asm volatile (
      "movl %%cr3, %%eax ;"
      "movl %%eax, %%cr3 ;"
      :
      :
      :"eax"
    ); // flush TLB
}

/*
 * function : user_vid_unpaging()
 * effects: destroy user video memory page
 */
void user_vid_unpaging(){
  // set user memory page to be not present !
  page_directory[USER_V_140MB] = RW_NP;
  vid_page_table[0] = RW_NP;

  asm volatile (
      "movl %%cr3, %%eax ;"
      "movl %%eax, %%cr3 ;"
      :
      :
      :"eax"
    ); // flush TLB
}


void terminal_vid(uint32_t physical_addr){
  uint32_t i;
  // read_write, present, and user privilege
  page_directory[USER_V_140MB] = (uint32_t) vid_page_table | RW_P_USR;
  vid_page_table[0] = physical_addr | RW_P_USR;
  for (i = 1; i <oneKB; i++){
    vid_page_table[i] = RW_NP;
  }

  asm volatile (
      "movl %%cr3, %%eax ;"
      "movl %%eax, %%cr3 ;"
      :
      :
      :"eax"
    ); // flush TLB
}
