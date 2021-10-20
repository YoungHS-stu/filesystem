#pragma once
#include"BlockManager.h"
#include"utils.h"
#include"MACROS.h"


class Disk {
public:
    FILE* pFile;

    Disk();
    ~Disk();
    int run();
    std::string getWelcomeMessage();
    //int ConstructingDisk(FILE** , SuperBlock*); //指针引用传值
    int CreateDiskFile(FILE** );
    int OpenDiskFile(FILE** );
    int LoadDiskFile(FILE** );
    int SetCurrentInode(int);
    int InitRootDirectory(FILE** );
    // Inode* 
    Inode oCurrentInode;
    SuperBlock oSuperBlock;
    Block oBlock;
    BlockManager oBlockManager;
};