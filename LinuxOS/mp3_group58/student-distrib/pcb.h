/* pcb.h - pcb file
 * vim:ts=4 noexpandtab
 */

#ifndef _PCB_H
#define _PCB_H

#include "types.h"

#define NULL              0
#define FILE_ARRAY_LENGTH 8
#define TYPE_RTC          0
#define TYPE_DIR          1
#define TYPE_FILE         2
#define MAX_PROGRAM       6
#define IN_USE            1
#define UNUSED            0
#define FILE_ARRAY_START  2
#define ARGS_SIZE         128
#define BASE_SHELLS       3
#define BUF_SIZE          128
#define TERMINAL_0        0
#define TERMINAL_1        1
#define TERMINAL_2        2
#define NOT_PRESENT       0
#define PRESENT           1

#ifndef ASM

// file operations table
typedef struct file_ops
{
    int32_t (*op_read)(int32_t, void*, int32_t);
    int32_t (*op_write)(int32_t, const void*, int32_t);
    int32_t (*op_open)(const uint8_t*);
    int32_t (*op_close)(int32_t);
} file_ops_t;

// file descriptor table
typedef struct file_desc
{
    file_ops_t *ops_table;
    int32_t    inode;
    int32_t    file_position;
    int32_t    flags;
} file_desc_t;

// program control block
typedef struct pcb
{
    uint8_t args[ARGS_SIZE];
    uint32_t args_length;
    file_desc_t file_desc_table[FILE_ARRAY_LENGTH];;
    uint32_t pid;
    uint32_t parent_esp0;
    uint32_t esp0;
    void * parent_pcb;
} pcb_t;

// terminal struct
typedef struct terminal {
    uint8_t term_num;
    int8_t line_buffer[BUF_SIZE];
    uint8_t return_flag;
    uint8_t input_flag;
    uint8_t vidmap_flag;
    uint32_t index;
    uint8_t * terminal_video;
    uint32_t x;
    uint32_t y;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esp0;
    pcb_t * curr_pcb;
} terminal_t;

terminal_t terminals[BASE_SHELLS];

// current terminal
terminal_t * curr_term;

// current pcb
pcb_t * pcb;

// current visible terminal
terminal_t * visible_term;

#endif /* ASM */

#endif /* _PCB_H */
