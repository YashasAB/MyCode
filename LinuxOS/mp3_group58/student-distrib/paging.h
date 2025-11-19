#include "types.h"

#define fourKB          4096
#define oneKB           1024
#define video_mem_entry 0xB8
#define video_term_0    0xB9
#define video_term_1    0xBA
#define video_term_2    0xBB
#define video_mem       0xb8000
#define fourMB          0x400000
#define eightMB         0x800000
#define RW_P_S          0x3
#define RW_P_USR        0x7
#define RW_P_S_MB       0x83
#define RW_NP           0x02
#define RW_P_U_MB       0x87
#define USER_128_MB     32
#define USER_V_140MB    35
#define KERNEL_PAGE     1

extern uint32_t page_directory [oneKB] __attribute__ ((aligned (fourKB)));
extern uint32_t page_table[oneKB] __attribute__ ((aligned (fourKB)));
extern uint32_t vid_page_table[oneKB] __attribute__ ((aligned (fourKB)));

// functions
void paging_init();
void user_paging(int32_t pid);
void user_vid_paging();
void user_vid_unpaging();
void terminal_vid(uint32_t physical_addr); 
