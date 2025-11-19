
#ifndef _SYSTEM_CALLS_H
#define _SYSTEM_CALLS_H

#include "types.h"

// in types.h
// #define FILE_ARRAY_LENGTH 8
#define STD_IN_FD     0
#define STD_OUT_FD    1
#define BIG_FILE      102400
#define SMALL_BUF     5
#define NO_OFFSET     0
#define PROGRAM_ADDR  0x08048000
#define EIGHT_MB      0x800000
#define EIGHT_KB      8192
#define L_OFFSET      1
#define F_OFFSET      2
#define FAULT_OFFSET  4
#define U_128_MB      0x08000000
#define U_132MB       0x08400000
#define U_140MB       0x08C00000
#define BASE_SHELL    0

/*
 * system_read
 * DESCRIPTION: Calls the read function for the given file descriptor
 *       INPUT: fd - file descriptor
 *              buf - buffer the system will put data into
 *              nbytes - number of bytes to be read
 *      RETURN: number of bytes read
 */
int32_t system_read(int32_t fd, void* buf, int32_t nbytes);

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
int32_t system_write(int32_t fd, const void* buf, int32_t nbytes);

/*
 * system_open
 * DESCRIPTION: Opens a directory, file, or device if the inputted
 *              filename is valid
 *       INPUT: filename - pointer to filename
 *      RETURN: file descriptor upon success
 */
int32_t system_open(const uint8_t* filename);

/*
 * system_close
 * DESCRIPTION: Closes the file descriptor fd by marking the
 *              entry as unused and calls the corresponding
 *              close function.
 *       INPUT: fd - file descriptor
 *      RETURN: 0 upon success and -1 upon failure
 */
int32_t system_close(int32_t fd);

/*
 * _system_execute
 * DESCRIPTION: Helper function which checks the validity of
 *              executables, creates a new PCB for a new
 *              executable, and sets up the paging for a new
 *              program
 *       INPUT: command - executable command
 *      RETURN: 0 upon success; -1 upon failure
 */
int32_t _system_execute(const uint8_t* command);

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
int32_t _system_halt(uint8_t status);

/*
 * system_getargs
 * DESCRIPTION: Writes program arguments into buf according
 *              to the number of bytes specified
 *       INPUT: buf - user level buffer to be written to
 *      RETURN: -1 upon failure and 0 upon success
 */
int32_t system_getargs(uint8_t* buf, int32_t nbytes);

/*
 * system_vidmap
 * DESCRIPTION: allocate user video memory and give it
 *              to a user ptr
 *       INPUT: ptr to a user ptr
 *      RETURN: 0 upon success; -1 upon failure
 */
int32_t system_vidmap(uint8_t ** screen_start);

/*
 * set_exception_one
 * DESCRIPTION: Helper function which set the exception to 1
 *       INPUT: none
 *      RETURN: none
 */
void set_exception_one();

#endif /* _SYSTEM_CALLS_H */
