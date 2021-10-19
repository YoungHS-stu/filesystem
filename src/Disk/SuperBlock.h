#pragma once
#include"Address.h"
#include"MACROS.h"
class SuperBlock
{
public:
	char override[4] = "yhs";
	unsigned int iBlockN;
	unsigned int iDataBlockN;
	unsigned int iDataBlockFreeN;
	unsigned int iInodeN;
	unsigned int iInodeFreeN;

	unsigned int iInodeBitMapBlockN;
	unsigned int iDataBlockBitMapBlockN;
	unsigned int iRedundancyBlockN;

	unsigned int SUPERBLOCK_SIZE; //1kb
	unsigned int BLOCK_SIZE; //1kb
	unsigned int DATA_BLOCK_SIZE; //1021b
	unsigned int INODE_SIZE; //44b
	unsigned int TOTAL_SIZE; //100mb

	unsigned int iSuperBlockStart;
	unsigned int iUserStart;
	unsigned int iInodeStart;
	unsigned int iInodeBitMapStart;
	unsigned int iDataBlockBitMapStart;
	unsigned int iDataBlockStart;
	unsigned int iRedundancyStart;

	Address freeptr;

	SuperBlock(){
		iBlockN = INITIAL_BLOCK_NUM;
		iDataBlockN = INITIAL_DATA_BLOCK_NUMBER;
		iDataBlockFreeN = INITIAL_DATA_BLOCK_NUMBER;
		iInodeN = INITIAL_INODE_NUMBER;
		iInodeFreeN = INITIAL_INODE_NUMBER;
		iInodeBitMapBlockN = INITIAL_INODE_BITMAP_BLOCK;
		iDataBlockBitMapBlockN = INITIAL_DATA_BITMAP_BLOCK;
		iRedundancyBlockN = INITIAL_REDUNDANT_BLOCK_NUMBER;
		SUPERBLOCK_SIZE = INITIAL_BLOCK_SIZE;
		BLOCK_SIZE = INITIAL_BLOCK_SIZE;
		INODE_SIZE = INITIAL_INODE_SIZE;
		TOTAL_SIZE = INITIAL_DISK_SIZE;
		DATA_BLOCK_SIZE = INITIAL_DATA_BLOCK_SIZE;

		iSuperBlockStart = 0;
		iUserStart  = iSuperBlockStart + INITIAL_BLOCK_SIZE;
		iInodeStart = iUserStart + INITIAL_BLOCK_SIZE;
		iInodeBitMapStart = iInodeStart + INITIAL_INODE_BlOCK_NUMBER * INITIAL_BLOCK_SIZE;
		iDataBlockBitMapStart = iInodeBitMapStart + INITIAL_INODE_BITMAP_BLOCK * INITIAL_BLOCK_SIZE;
		iDataBlockStart = iDataBlockBitMapStart + INITIAL_DATA_BITMAP_BLOCK * INITIAL_BLOCK_SIZE;
		iRedundancyStart = iDataBlockStart + INITIAL_DATA_BLOCK_NUMBER * INITIAL_BLOCK_SIZE;
	}

	void PrintSuperBlockInfo()
	{
		printf("Block size: %d\n", BLOCK_SIZE);
		printf("Data Block Number: %d\n", iDataBlockN);
		printf("Free Data Block Number: %d\n", iDataBlockFreeN);
		printf("Inode Number: %d\n", iInodeN);
		printf("Free Inode Block Number: %d\n", iInodeFreeN);
	}
};


