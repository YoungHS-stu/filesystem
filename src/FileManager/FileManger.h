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
    int myCreateFile(); //newfile
    int myOpenFile(); // cat
    int myDeleteFile(); //rm
    int myCopyFile(); //cp

    int myCreateDirectory(char* path);//mkdir
    int myDeleteDirectory();//rmdir
    int myChangeDirectory();//cd
    int myPrintWorkingDirectory();//pwd
    int myListDirectory(); // ls or dir

    std::string GetWorkingDirectory();
    void PrintDiskInfo(); // info
    void PrintHelp(); //help
    int SystemCheck(); // check
    int WriteCmdTokens();
    int ReadCmdTokens();

    int AllocateIPC(HANDLE*);
    int ListenToIPC(HANDLE*);
    int ReadIPC(HANDLE*);
    int WriteIPC(HANDLE*);
    void CmdParser();
    void run();
    int SplitStringIntoVector(const std::string& , const std::string&, std::vector<std::string>&);
};


