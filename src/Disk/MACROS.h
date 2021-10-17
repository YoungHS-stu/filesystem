#pragma once
const int INIT_DISK_SIZE = 100 * 1024 * 1024;		// System总大小
const int BLOCK_SIZE = 1 * 1024;				// Block空间大小
const int NUM_BLOCKS = 16 * 1024;				// Block总个数
const int NUM_INODES = 47;						// i-node总个数
const int ADDR_PER_INODE = 10;					// i-node中direct的个数
const int DIRECTORY_TABLE_SIZE = NUM_INODES;	// Direcotry table的大小

const int OFFSET_INODE_BITMAP = 1;
const int OFFSET_DATA_BITMAP = OFFSET_INODE_BITMAP + 1;
const int OFFSET_INODES = OFFSET_DATA_BITMAP + 16;
// const int OFFSET_DITRCORY_TABLE = OFFSET_INODES + 3;
const int OFFSET_DATA = OFFSET_INODES + 5;

const int NUM_DATA_BLOCKS = NUM_BLOCKS - OFFSET_DATA;

const char DISK_FILE_NAME[] = "YDisk.yhs";
const char OVERRIDE[] = "yhs";


const unsigned int DIRECT_ADDRESS_NUMBER = 1024;

// measured in "BYTES" 
#define INITIAL_DISK_SIZE  100 * 1024 * 1024 //100mb
#define INITIAL_BLOCK_SIZE  1 * 1024 //1kb
#define INITIAL_SUPERBLOCK_SIZE  1 * 1024//1kb
#define INITIAL_DATA_BLOCK_SIZE  1 * 1021 //1021+3(下个Block地址)
#define INITIAL_DATA_BITMAP_BLOCK  90
#define INITIAL_BLOCK_NUM  100 * 1024

#define INITIAL_INODE_BlOCK_NUMBER  10000
#define INITIAL_INODE_NUMBER  182857 // 10000*1024/56
#define INITIAL_INODE_SIZE 56//我的inode就是56bytes
#define INITIAL_INODE_BITMAP_BLOCK  178 

#define INITIAL_DATA_BLOCK_NUMBER 92000
#define INITIAL_REDUNDANT_BLOCK_NUMBER 130
#define INITIAL_USER_BLOCK_NUMBER 1
// super(1)+user(1)+inodes(10240)+inodeMap(10)+DataBlockMap(91)+DataBlock(92055)+RD(2)
#define DISK_PATH "disk.dat"

#define MAXIMUM_FILENAME_LENGTH 63
 