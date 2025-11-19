#include "lib.h"
#include "types.h"
#include "x86_desc.h"
#include "filesystem.h"
#include "pcb.h"

uint8_t * fs;
boot_block_t * boot;
inode_t * inode_ptr;
dentry_t * dentry_ptr;
uint32_t dentry_index;

/*
 * file_boot()
 * DESCRIPTION: initializes the file system
 *       INPUT: uint8_t *fs_ptr, a file system pointer to fetch the boot block
 *      RETURN: NONE
 */
void file_boot(uint8_t * fs_ptr )
{
    fs = fs_ptr;
    boot = (boot_block_t*) fs_ptr;
    dentry_ptr = &((dentry_t*) boot)[1];
    inode_ptr = &((inode_t*) fs)[1];
}

/*------------------------------------------------------------------------------*/
/*                                     Files                                    */
/*------------------------------------------------------------------------------*/

/*
 * file_open()
 * DESCRIPTION: reads the inode number of the file user is trying to open to 
 *              inode pointer to open file
 *       INPUT: const uint8_t *filename, a neame of file user is trying to open
 *      RETURN: file descriptor upon success, -1 if failure
 */
int32_t file_open(const uint8_t * filename)
{
    dentry_t f0;
    // if finding the file using filename fails, return -1
    if(read_dentry_by_name(filename, &f0) == FAILURE) {
        return FAILURE;
    }

    int32_t fd;
    for(fd = FILE_ARRAY_START; fd < FILE_ARRAY_LENGTH; ++fd) {
      if(pcb->file_desc_table[fd].flags == UNUSED) {
        break;
      }
    } 
    if(fd == FILE_ARRAY_LENGTH) {
      return FAILURE;
    }

    // file we are looking for is found
    // inode number of the file is saved into inode pointer
    pcb->file_desc_table[fd].inode = f0.inode_num;
    // default file position
    pcb->file_desc_table[fd].file_position = 0;
    // file is found successfully, return 0
    return fd;

}

/*
 * file_close()
 * DESCRIPTION: closes the file which is opened, reset the values of
 *              inode and file_position to 0, which is default
 *       INPUT: uint32_t fd
 *      RETURN: 0
 */
int32_t file_close(int32_t fd)
{
    return SUCCESS;
}

/* file_write()
 * DESCRIPTION: return -1, as file system is read-only
 *       INPUT: int32_t fd, const void* buf, int32_t nbytes
 *      RETURN: -1
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes)
{
    // file system is READ ONLY, and we better keep it that way
    return FAILURE;
}

/* file_read()
 * DESCRIPTION: reads the file in the file system using read_data(),
 *              increments the file_position based on the return value from it
 *       INPUT: int32_t fd,
 *              void *buf, 
 *              int32_t nbytes 
 *      RETURN: num_read, a size of buffer we will need to print out the data
 */
int32_t file_read(int32_t fd, void *buf, int32_t nbytes)
{
    int32_t num_read = read_data(pcb->file_desc_table[fd].inode, pcb->file_desc_table[fd].file_position, buf, nbytes);
    pcb->file_desc_table[fd].file_position += num_read;
    return num_read;
}

/* read_dentry_by_name()
 * DESCRIPTION: reads dentry by filename
 *       INPUT: const uint8_t *fname, the name of the file user wants to read
 *              dentry_t *dentry, a dentry structure
 *      RETURN: num_read, a size of buffer we will need to print out the data
 */
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry)
{
    // check for validity
    if(dentry == NULL || fname == NULL)
    {
        return FAILURE;
    }

    int i;                            // loop index
    uint32_t len;                     // variable to hold the length of the requested file name
    len = strlen((int8_t *)fname);

    // If the length of the file name exceeds the maximum length of the filename, chop it down to fit the maximum size 
    // ! file name must never exceed the maximum length in first place
    if (len > MAX_FNAME)
    {
        // len = MAX_FNAME;
        return FAILURE;
    }

    // variable to hold the # of dir. entries
    uint32_t dentry_num;
    // the # of dir. entries is fetched from boot block
    dentry_num = boot->num_dentries;
    
    for(i = 0; i <dentry_num ; ++i)
    {
        // check if strings are equal in length
        uint32_t temp_len = strlen((int8_t*) dentry_ptr[i].filename);
        if(temp_len > MAX_FNAME) temp_len = MAX_FNAME;
        if(temp_len == len) {
            // check if each character is the same
            if(strncmp((int8_t*) dentry_ptr[i].filename, (int8_t *)fname, len)== 0)
            {
                // file with exact name found, dentry now holds the data
                *dentry = dentry_ptr[i];
                // successful read performed
                return SUCCESS;
            }
        }
        
    }

    // no matching fname in filename in dentry, thus read failed
    return FAILURE;
}

/* read_dentry_by_index()
 * DESCRIPTION: reads dentry by index
 *       INPUT: uint32_t *index, the index of dentry
 *              dentry_t *dentry, a dentry structure
 *      RETURN: num_read, a size of buffer we will need to print out the data
 */
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry)
{
    // check for validity
    if(index > boot->num_dentries)
    {
        return FAILURE;
    }

    *dentry = dentry_ptr[index];
    return SUCCESS;
}

/* read_data()
 * DESCRIPTION: reads data in the inode block in the dentry
 *       INPUT: uint32_t inode, the value of inode block
 *              uint32_t offset, the offset between the start location and start of
 *                               inode block
 *              uint8_t *buf, a buffer from file_read()
 *              uint32_t length, a length of the data
 *      RETURN: num_read, a size of buffer we will need to print out the data
 */
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length)
{
    // check for validity
    if (inode > boot->num_inodes)
    {
        return FAILURE;
    }

    // initialization of variables
    uint32_t num_read = 0;
    // fetch file length from the inode
    uint32_t file_length = inode_ptr[inode].length;
    

    // check for validity - offset can't be larger than file length
    if (offset > file_length)
    {
        return num_read;
    }
    // check for validity - sum of offset and length can't exceed file length
    if (offset + length > file_length)
    {
        length = file_length - offset;
    }
    
    uint32_t start_block = offset / FOURKB ;                //
    uint32_t end_block =  (offset + length) / FOURKB;       //
    uint32_t i;                                             // loop index
    uint32_t data_block_location;                           //
    uint8_t * data_location;                                //

    // iterating through blocks
    for (i = start_block; i <= end_block; i++)
    {
        data_block_location = inode_ptr[inode].data_block_data[i];
        data_location = (uint8_t *) fs + (data_block_location + boot->num_inodes +1)*FOURKB;
        // default values for start and stop
        uint32_t start = 0;
        uint32_t stop = FOURKB;

        // ? check since i = start_block shouldn't be functional as intended 
        if (i == start_block)
        {
            // start is calculated with remainder of offset
            // divided by 4KB, as if it exceeds 4KB, it should be on next block 
            start = offset % FOURKB;
        }
        if (offset + length < FOURKB * (i + 1))
        {
            // stop is caculated with remainder of (offset + length)
            // divided by 4KB, as the stop isn't always 4KB if length is short
            stop = (offset + length) % FOURKB;
        }
        memcpy(((uint8_t*)buf) + num_read, data_location + start, stop - start);
        // size needed = size between start and stop
        num_read += (stop - start);
    }

    return num_read;
}

/*------------------------------------------------------------------------------*/
/*                               Directories                                    */
/*------------------------------------------------------------------------------*/

/*
 * dir_open()
 * DESCRIPTION: opens the directory
 *       INPUT: uint8_t *filename, a neame of file user is trying to open
 *      RETURN: 0 if success
 */
int32_t dir_open(const uint8_t * filename)
{
    int32_t fd;
    for(fd = FILE_ARRAY_START; fd < FILE_ARRAY_LENGTH; ++fd) {
      if(pcb->file_desc_table[fd].flags == UNUSED) {
        break;
      }
    } 
    if(fd == FILE_ARRAY_LENGTH) {
      return FAILURE;
    }
    dentry_index = 0;
    return fd;
}

/*
 * dir_close()
 * DESCRIPTION: closes the directory
 *       INPUT: uint32_t fd
 *      RETURN: 0 if success
 */
int32_t dir_close(int32_t fd)
{
    dentry_index = 0;
    return SUCCESS;
}

/* 
 * dir_write()
 * DESCRIPTION: return -1, as file system is read-only
 *       INPUT: int32_t fd, const void* buf, int32_t nbytes
 *      RETURN: -1
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes)
{
    return FAILURE;
}

/* dir_read()
 * DESCRIPTION: reads the directory in the file system
 *              increments the file_position based on the return value from it
 *       INPUT: fd - file descriptor 
 *              buf - buffer to be read into 
 *              nbytes - number of bytes to be read
 *      RETURN: number of bytes read
 */
int32_t dir_read(int32_t fd,  void *buf, int32_t nbytes)
{
    dentry_t f;

    // check for validity - dentry index can't exceed the number of dentries
    // nor can the length be 0
    if ((dentry_index >= boot->num_dentries) || (nbytes == 0))    
    {
        return 0;
    }

    // dentry index is valid, we may now proceed to use the index to read
    // dentry by index
    read_dentry_by_index(dentry_index, &f);
    
    // if length is beyond the maximum length of filename, chop it out to match
    if(nbytes > MAX_FNAME)
    {
        nbytes = MAX_FNAME;
    }

    // memcpy to fetch
    memcpy(buf, f.filename, nbytes);

    dentry_index++;
    return nbytes;
}
