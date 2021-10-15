#pragma once
const int INIT_DISK_SIZE = 16 * 1024 * 1024;		// System总大小
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