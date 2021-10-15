#include"Disk.h"

int ConstructingDisk(FILE* pFile)
{

    return 0;
}
int CreateDiskFile(FILE* pFile)
{
    printf("creating disk file\n");
    errCode = fopen_s(&pFile, DISK_FILE_NAME, "w");
    //fprintf(pFile, "%d,%d,%d\n", 1, 2, 3);// !写入ASCII字符
    if(errCode!=0){printf("Disk File Initialization Fails!\n"); return -1;}

    // !为了创建指定大小的文件
    errCode = fseek(pFile, INIT_DISK_SIZE-1, SEEK_SET);
    if(errCode!=0){printf("Disk Seek Fails!\n"); return -1;}

    errCode = fputc(0, pFile);
    if(errCode!=0){printf("Disk fputc Fails!\n"); return -1;}
    fclose(pFile);
    

    // !以读写方式重新打开文件, 并二进制加入Override
    errCode = fopen_s(&pFile, DISK_FILE_NAME, "rb+");
    size_t r = fwrite(OVERRIDE, sizeof(char), sizeof(OVERRIDE), pFile);
    if(r!=sizeof(OVERRIDE)){printf("Disk Put Override Fails!\n"); return -1;}

    printf("Disk File Created!\n");
    return 0;
}
int OpenDiskFile(FILE* pFile )
{
    printf("opening disk file\n");
    // errno_t = int
    errno_t errCode = fopen_s(&pFile,DISK_FILE_NAME,"rb+");
    if (errCode == 0)
    {
        printf("Disk File Exists!\n")
    }
    else
    {   
        printf("Disk File Not Found! Begin Disk Initialization: Disk File Name -> %s\n",DISK_FILE_NAME);
        errCode = CreateDiskFile(pFile);
        if(errCode!=0){printf("Disk Create Fails!\n"); return -1;}
        errCode = ConstructingDisk(pFile);
        if(errCode!=0){printf("Disk Construction Fails!\n"); return -1;}


    }
    return 0;
}

int LoadDiskFile(FILE* pFile, )
Disk::Disk()
{
    printf("constructing disk\n");
    this->fileNamePattern = regex("^([A-Z]|[a-z]|[-+_/.]|[0-9])*$");
    FILE* pFile;
    OpenDiskFile(pFile);
    if(errCode!=0){printf("Disk Open Fails!\n"); return -1;}


}

int Disk::run(){
    cout<< "Disk running begin" <<endl;
    return 0;
}

Disk::~Disk()
{
    printf("bye disk~");
}
