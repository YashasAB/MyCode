#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "lib.h"
#include "types.h"

#define MAX_DATA_BLOCK_DATA     1023
#define MAX_DENTRY                63
#define MAX_FNAME                 32
#define FOURKB                  4096

// Directory Entry
// Specifications in MP3 documentation page 18
typedef struct dentry
{
    uint8_t filename[MAX_FNAME];        // 32B for file name (up to 32 characters, zero-padded)
    uint32_t filetype;                  // 4B for file type
    uint32_t inode_num;                 // 4B for inode #
    uint8_t reserved[24];               // 24B reserved
} dentry_t;

typedef struct boot_block
{
    uint32_t num_dentries;              // # dir. entries
    uint32_t num_inodes;                // # inodes (N)
    uint32_t num_dataBlocks;            // # data blocks (D)
    uint8_t reserved[52];               // 52B reserved
    dentry_t entries[MAX_DENTRY];       // 64B dir. entries, but the statistics and each directory entry occupy 64B, resulting in 63.
} boot_block_t;
// Inode (Index Nodes)

typedef struct inode
{
    uint32_t length;                                    // length in B
    uint32_t data_block_data[MAX_DATA_BLOCK_DATA];      // 4KB
} inode_t;

void file_boot(uint8_t* fs_ptr);

int32_t file_open(const uint8_t * filename);
int32_t file_close(int32_t fd);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t file_read(int32_t fd,  void *buf, int32_t nbytes);

int32_t dir_open(const uint8_t * filename);
int32_t dir_close(int32_t fd);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_read(int32_t fd,  void *buf, int32_t nbytes);
// Three routines written in MP3 documentation page 18
int32_t read_dentry_by_name(const uint8_t *fname, dentry_t *dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t *dentry);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);



#endif /* FILESYSTEM_H */
