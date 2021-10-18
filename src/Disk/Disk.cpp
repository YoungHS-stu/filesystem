#include"Disk.h"
int Disk::InitRootDirectory(FILE** pFile)
{
    printf_src("Initializing Root Directory");
    Directory rootDir;
    rootDir.vFiles.push_back(File(".",0));
    rootDir.vFiles.push_back(File("..",0));
    size_t DirSize = rootDir.vFiles.size() * sizeof(File);
    printf_src("Root dir size");
    int inodeId = oBlockManager.GetNextFreeInode();
    int addrInt = oBlockManager.GetNextFreeBlock();
    Address addr = Address(addrInt);
    Inode inode = Inode(DirSize, inodeId, addr, true);
    Block block = Block(0);
    memcpy(block.content, &rootDir, DirSize);
    oBlockManager.WriteNewBlock(addr, block);
    oBlockManager.WriteNewInode(inode);
    return 0;
}

int Disk::SetCurrentInode(int inode_id)
{
    Inode inode = oBlockManager.ReadInode(inode_id);
    return 0;
}

int Disk::LoadDiskFile(FILE** pFile) {
    printf_src("Begin");
    printf_src("Load SuperBlock");
    Fseek(*pFile, 0L, SEEK_SET);
    errno_t r = Fread(&oSuperBlock, sizeof(oSuperBlock), 1, *pFile);
    if(r!=1) {printf("Load SuperBlock Failed\n"); return -1;} //1为正常值，模仿的原生逻辑

    //-2 set init inode
    r = SetCurrentInode(0);
    if(r!=0) {printf("Set Initiail Inode Failed\n"); return -2;} 

    
    return 0;
}

int Disk::CreateDiskFile(FILE** pFile)
{
    printf("creating disk file from %s\n", __FUNCTION__);
    Fopen(pFile, DISK_FILE_NAME, "w");
    // !为了创建指定大小的文件
    Fseek(*pFile, INIT_DISK_SIZE-1, SEEK_SET);
    
    errno_t errCode = fputc(0, *pFile);
    if(errCode!=0){printf("Disk fputc Fails!\n"); return -1;}
    fclose(*pFile);
    
    // !以读写方式重新打开文件
    Fopen(pFile, DISK_FILE_NAME, "rb+");
    printf("Disk File Created!\n");
    return 0;
}
int Disk::OpenDiskFile(FILE** pFile)
{
    printf("opening disk file\n");
    bool bLoadDisk = false;
    // errno_t = int
    
    errno_t errCode = Fopen(pFile,DISK_FILE_NAME,"rb+");
    if (errCode == 0)
    {
        printf("Disk File Exists!\n");
        bLoadDisk = true;
    }
    if (!bLoadDisk)
    {   
        printf("Disk File Not Found! Begin Disk Initialization: Disk File Name -> %s\n",DISK_FILE_NAME);
        // -1 创建Disk文件
        errCode = CreateDiskFile(pFile);
        if(errCode!=0){printf("Disk Create Fails!\n"); return -1;}
        // -2 构建磁盘结构
        BlockManager tempManager;
        memcpy(tempManager.pFile, *pFile, sizeof(tempManager.pFile));

        oBlockManager.InitializeDiskBlocks(&oSuperBlock, pFile);
        if(errCode!=0){printf("Disk Construction Fails!\n"); return -1;}
        InitRootDirectory(pFile);
        printf("Disk Structures Built Success!*************************");
    }
    return 0;
}

Disk::Disk()
{
    printf("constructing disk\n");
    //pSuperBlock = new SuperBlock(); // 如果已有磁盘，这部分会被从文件读取的数据Override
    FILE *tempFilePtr; 
    OpenDiskFile(&tempFilePtr);

    oBlockManager = BlockManager();
    memcpy(pFile, tempFilePtr, sizeof(tempFilePtr));
    memcpy(oBlockManager.pFile, tempFilePtr, sizeof(tempFilePtr));
    LoadDiskFile(&pFile);
    
    printf("closing file\n");
    fclose(pFile);

}

int Disk::run(){
    cout<< "Disk running begin" <<endl;
    return 0;
}

Disk::~Disk()
{
    printf("Disk closes successfully");
}
