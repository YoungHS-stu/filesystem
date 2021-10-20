#pragma once
#include"Address.h"
#include"SuperBlock.h"
#include"Inode.h"
#include"Block.h"
#include"utils.h"

class BlockManager
{
public:
    Address freeptr;
    SuperBlock oSuperBlock;
    FILE* pFile;
    BlockManager() {}; 
    ~BlockManager() {};
    void InitializeDiskBlocks(SuperBlock* pSuperBlock, FILE** pFile) {
        this->oSuperBlock = *pSuperBlock;
        printf("---------------InitializeDiskBlocks Start------------------\n");
        errno_t r;
        //-1 superblock
        printf_src("init superblock\n");
        Fseek(*pFile, 0L, SEEK_SET);
        printf_src("putting superblock sizeof(pSuperBlock):[%d]\n", sizeof(*pSuperBlock));
        Fwrite(pSuperBlock, sizeof(*pSuperBlock), 1, *pFile, "Disk Put SuperBlock");
        
        //-2 UserBlock
        Fseek(*pFile, pSuperBlock->SUPERBLOCK_SIZE, SEEK_SET);
        char UserBlock[BLOCK_SIZE];
        memset(UserBlock, 'U', sizeof(UserBlock));
        Fwrite(UserBlock, sizeof(UserBlock), 1, *pFile, "Disk Put UserBlock");

        //-3 Inodes
        Inode inode;
        printf_src("init Inode: sizeof Inode:[%d]\n", sizeof(inode));
        for (unsigned int i = 0; i < pSuperBlock->iInodeN; i++)
        {
            inode.iInodeId = i;
            r = Fwrite(&inode, sizeof(inode), 1, *pFile, "", false);
            if(r!=1){printf("Disk Put inode [%d] Fails! r=[%d]\n",i,r);break;}
        }
        
        //-4.1 Bitmap: Inode-bitmap
        Fseek(*pFile, pSuperBlock->iInodeBitMapStart, SEEK_SET);
        char InodeBitmap[pSuperBlock->iInodeBitMapBlockN * pSuperBlock->BLOCK_SIZE];
        printf_src("init Inode-bitmap, sizeof InodeBitmap:[%d]\n",sizeof(InodeBitmap));
        memset(InodeBitmap, 'F', sizeof(InodeBitmap)); //F(ree)代表未占用
        Fwrite(InodeBitmap, sizeof(InodeBitmap), 1, *pFile, "Disk Put InodeBitmap");
        
        //-4.2 Bitmap: Data-bitmap
        //Fseek(*pFile, pSuperBlock->iDataBlockBitMapStart, SEEK_SET);
        char DataBitmap[pSuperBlock->iDataBlockBitMapBlockN * pSuperBlock->BLOCK_SIZE];
        printf_src("init DataBitmap, sizeof DataBitmap:[%d]\n",sizeof(DataBitmap));
        memset(DataBitmap, 'f', sizeof(DataBitmap)); //0代表未占用
        Fwrite(DataBitmap, sizeof(DataBitmap), 1, *pFile, "Disk Put DataBitmap");
        

        //-5 DataBlock
        Block block;
        memset(block.next.addr, 'D', sizeof(block.next.addr));
        memset(block.content, 'd', sizeof(block.content));
        printf_src("init DataBlock sizeof block:[%d]\n", sizeof(block));
        //Fseek(*pFile, pSuperBlock->iDataBlockStart, SEEK_SET);
        for (size_t i = 0; i < pSuperBlock->iDataBlockN; i++)
        {
            r = Fwrite(&block, sizeof(block), 1, *pFile, "", false);
            if(r!=1){printf("Disk Put block [%d] Fails! r=[%d]\n",i,r);break;}
        }

        //-6 RedundancyBlock
        memset(block.next.addr, 'R', sizeof(block.next.addr));
        memset(block.content, 'r', sizeof(block.content));
        //Fseek(*pFile, pSuperBlock->iRedundancyStart, SEEK_SET);
        for (size_t i = 0; i < pSuperBlock->iRedundancyBlockN; i++)
        {
            r = Fwrite(&block, sizeof(block), 1, *pFile, "", false);
            if(r!=1){printf("Disk Put block [%d] Fails! r=[%d]\n",i,r);break;}
        }
        Fseek(*pFile,-1024,SEEK_CUR);
        char myWords[] = "write something to the last block";
        Fwrite(myWords, sizeof(myWords), 1, *pFile, "Disk Put myWords");
        printf("---------------InitializeDiskBlocks Done------------------\n");
    }

    int ReadBlock(Address addr, Block& block){
        int addr_int = addr.to_int();
        Fseek(pFile, oSuperBlock.iDataBlockStart + addr_int * BLOCK_SIZE, SEEK_SET);
        Fread(&block, sizeof(block), 1, pFile);
        return 0;
    }

    int WriteBlock(Address addr, Block& block){
        int addr_int = addr.to_int();
        Fseek(pFile, oSuperBlock.iDataBlockStart + addr_int * BLOCK_SIZE, SEEK_SET);
        Fwrite(&block, sizeof(block), 1, pFile);
        return 0;
    }

    int WriteNewBlock(Address addr, Block& block) {
        errno_t r = WriteBlock(addr, block);
        if(r==0) {
            oSuperBlock.iDataBlockFreeN--;
            UpdateDataBlockBitmap(addr, false);
        }
        return 0;
    }

    int ClearBlock(Address addr) {
        int addr_int = addr.to_int();
        Block block;
        memset(block.next.addr, 'D', sizeof(block.next.addr));
        memset(block.content, 'd', sizeof(block.content));
        errno_t r = WriteBlock(addr, block);
        if(r==0) {
            oSuperBlock.iDataBlockFreeN++;
            UpdateDataBlockBitmap(addr, true);
        }
        return 0;
    }

    // 查看inode-bitmap
    int GetNextFreeInode() {
        Fseek(pFile, oSuperBlock.iInodeBitMapStart, SEEK_SET);
        char map[oSuperBlock.iInodeN]; 
        Fread(map, sizeof(map), 1, pFile);
        for (size_t i = 0; i < sizeof(map); i++)
        {
            if(map[i]=='F') {return i;}
        }
        printf("No more free Inode");
        return -1;
    }

    int UpdateInodeBitmap(int InodeId, bool isFree) {
        Fseek(pFile, oSuperBlock.iInodeBitMapStart, SEEK_SET);
        char map[oSuperBlock.iInodeN]; 
        Fread(map, sizeof(map), 1, pFile);
        if (InodeId > sizeof(map)) { printf_err("InodeId:%d too large, size of map:%d", InodeId, sizeof(map)); return -2;}
        if (isFree == true) {map[InodeId] = 'F';}
        else {map[InodeId] = 'N';}
        Fseek(pFile, oSuperBlock.iInodeBitMapStart, SEEK_SET);
        Fwrite(map, sizeof(map), 1, pFile);
        return -1;
    }

    int UpdateDataBlockBitmap(Address addr, bool isFree) {
        int addr_int = addr.to_int();
        Fseek(pFile, oSuperBlock.iDataBlockBitMapStart, SEEK_SET);
        char map[oSuperBlock.iInodeN]; 
        Fread(map, sizeof(map), 1, pFile);
        if (addr_int < sizeof(map)) { printf("addr too large"); return -2;}
        if (isFree == true) {map[addr_int] = 'f';}
        else {map[addr_int] = 'n';}
        Fseek(pFile, oSuperBlock.iDataBlockBitMapStart, SEEK_SET);
        Fwrite(map, sizeof(map), 1, pFile);
        return -1;
        
    }
    // 查看data-bitmap
    int GetNextFreeBlock() {
        Fseek(pFile, oSuperBlock.iDataBlockBitMapStart, SEEK_SET);
        char map[oSuperBlock.iDataBlockN]; 
        Fread(map, sizeof(map), 1, pFile);
        for (size_t i = 0; i < sizeof(map); i++)
        {
            if(map[i]=='f') {return i;}
        }
        printf("No more free Inode");
        return -1;
    }

    Inode ReadInode(int InodeId) {
        Inode inode;
        Fseek(pFile, oSuperBlock.iInodeStart + InodeId * sizeof(inode), SEEK_SET);
        Fread(&inode, sizeof(inode), 1, pFile);
        return inode;
    }

    int WriteInode(Inode inode) {
        Fseek(pFile, oSuperBlock.iInodeStart + inode.iInodeId * sizeof(inode), SEEK_SET);
        Fwrite(&inode, sizeof(inode), 1, pFile);
        return 0;
    }

    int WriteNewInode(Inode inode) {
        errno_t r = WriteInode(inode);
        if(r==0) {
            oSuperBlock.iInodeFreeN--;
            UpdateInodeBitmap(inode.iInodeId, false);
        }
        return 0;
    }

    int ClearNewInode(Inode inode) {
        int inode_id = inode.iInodeId;
        Inode new_inode;
        new_inode.iInodeId = inode_id;
        errno_t r = WriteInode(inode);
        if(r==0) {
            oSuperBlock.iInodeFreeN++;
            UpdateInodeBitmap(inode.iInodeId, false);
        }
        return 0;
    }
};



