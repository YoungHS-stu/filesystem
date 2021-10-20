#pragma once
#include"Disk.h"
class FileManager
{
public:
    Disk disk;
	char userInput[1024];
    std::vector<std::string> vCmdTokens;
    std::string sResponse;
    std::map<std::string,int> mapUserToId;
    std::map<int,HANDLE*> mapIdToIPC;
    std::regex fileNamePattern;
    FileManager(/* args */);
    ~FileManager();
    std::string myCreateFile(); //newfile
    std::string myOpenFile(); // cat
    std::string myDeleteFile(); //rm
    std::string myCopyFile(); //cp

    std::string myCreateDirectory(char* path);//mkdir
    std::string myDeleteDirectory();//rmdir
    std::string myChangeDirectory();//cd
    std::string myPrintWorkingDirectory();//pwd
    std::string myListDirectory(); // ls or dir

    std::string GetWorkingDirectory();
    std::string PrintDiskInfo(); // info
    std::string PrintHelp(); //help
    std::string PrintWelcomeInfo();
    std::string SystemCheck(); // check
    int WriteCmdTokens();
    int ReadCmdTokens();

    int AllocateIPC(HANDLE*);
    int ListenToIPC(HANDLE*);
    int ReadIPC(HANDLE*);
    int WriteIPC(HANDLE*);
    void CmdParser();
    void run();
    int SplitStringIntoVector(const std::string& , const std::string&, std::vector<std::string>&);

    unsigned GetInodeIdFromPath(std::string );
    unsigned GetDirectorySizeFromInode(Inode );
    std::string GetFullFilePath(Inode);
    std::string GetFileFromInode(Inode);
    std::string GetFileNameFromInode(Inode );
    void RecursiveDeleteDirectory(Inode );
    int WriteFilesToDirectoryFile(Directory,Inode);
    Directory ReadFilesFromDirectoryFile(Inode);
    int ParentBlockRequired(Inode&);

};


