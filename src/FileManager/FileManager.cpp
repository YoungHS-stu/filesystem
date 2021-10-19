#include"FileManger.h"

FileManager::FileManager()
{
	fileNamePattern = std::regex("^([a-z]|[A-Z]|[_/.]|[0-9])*$");
    disk = Disk();
}

FileManager::~FileManager()
{
}

void FileManager::run()
{
    disk.run();
    PrintWelcomeInfo();
    memset(userInput, 0, sizeof(userInput));
    while (true)
    {

        printf("[%s@YFS%s ]$: ", "root", GetWorkingDirectory().c_str());
		scanf("%[^\n]", &userInput);
        getchar();
        
	    if (strtok(userInput, " ") == NULL) continue;
        if (!strcmp(userInput, "exit"))break;
        if (!strcmp(userInput, "quit"))break;
        CmdParser();
        memset(userInput, 0, sizeof(userInput));
    }
    
}
int FileManager::WriteCmdTokens()
{
    return 0;
}
int FileManager::ReadCmdTokens()
{
    return 0;
}

int FileManager::myCreateFile()
{
    return 0;
}
int FileManager::myOpenFile()
{
    return 0;
}
int FileManager::myDeleteFile()
{
    return 0;
}
int FileManager::myCopyFile()
{
    return 0;
}

int FileManager::myCreateDirectory(char* path)
{
    printf_src("\n");
    std::vector<std::string> vPathList;
    SplitStringIntoVector(std::string(path), "/", vPathList);
    for (size_t i = 0; i < vPathList.size(); i++)
    {
        if (vPathList[i].length() > MAXIMUM_FILENAME_LENGTH - 1) {
            printf_err("The directory/file name: %s is too long! Maximum length: %d", vPathList[i].c_str(), MAXIMUM_FILENAME_LENGTH - 1);
            return -1;
        }
    }

    int inode_id = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < vPathList.size(); i++) {
        Inode inode_ptr = disk.oBlockManager.ReadInode(inode_id);
        if (!inode_ptr.bIsDir) {
				printf("%s is a file! You can not create directory under here!\n", GetFileNameFromInode(inode_ptr).c_str());
				return -1;
		}

    }

    return 0;
}
int FileManager::myDeleteDirectory()
{
    return 0;
}
int FileManager::myChangeDirectory()
{
    return 0;
}
int FileManager::myPrintWorkingDirectory()
{
    printf("%s\n",GetWorkingDirectory().c_str());
    return 0;
}

std::string FileManager::GetWorkingDirectory() {
    return GetFullFilePath(disk.oCurrentInode);
}
int FileManager::myListDirectory()
{
    return 0;
}

void FileManager::PrintDiskInfo()
{
    
}

void FileManager::PrintHelp()
{
    
}

int FileManager::SystemCheck()
{
    return 0;
}

int FileManager::AllocateIPC(HANDLE*)
{
    return 0;
}
int FileManager::ListenToIPC(HANDLE*)
{
    return 0;
}
int FileManager::ReadIPC(HANDLE*)
{
    return 0;
}
int FileManager::WriteIPC(HANDLE*)
{
    return 0;
}
void FileManager::CmdParser()
{
    char* command = strtok(userInput, " ");
	if (command == NULL) {
		//printf_err("no command");
		return;
	}
    if (strcmp(command, "mkdir")==0) {
        char* path = strtok(NULL, " ");
		if (path == NULL) {
			printf_err("lack of path");
			return;
		}
        char* redundant = strtok(NULL, " ");
		if (redundant != NULL) {
			printf_err("too many arguments");
			return;
		}
        if (!std::regex_match(std::string(path), fileNamePattern))
		{
			printf_err("Your directory name does not meet the regex specification '^([a-z]|[A-Z]|[_/.]|[0-9])*$' "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores");
			return;
		}
        myCreateDirectory(path);
    }
    else if (strcmp(command, "rmdir")==0) {
        myDeleteDirectory();
    }
    else if (strcmp(command, "cd")==0) {
        myChangeDirectory();
    }
    else if (strcmp(command, "pwd")==0) {
        myPrintWorkingDirectory();
    }
    else if (strcmp(command, "dir")==0 || strcmp(command, "ls")==0) {
        myListDirectory();
    }
    else if (strcmp(command, "cp")==0) {
        myCopyFile();
    }
    else if (strcmp(command, "rmfile")==0) {
        myDeleteFile();
    }
    else if (strcmp(command, "mkfile")==0) {
        myCreateFile();
    }
    else if (strcmp(command, "cat")==0) {
        myOpenFile();
    }
    else if (strcmp(command, "help")==0) {
        PrintHelp();
    }
    else if (strcmp(command, "info")==0) {
        PrintDiskInfo();
    }
    else if (strcmp(command, "check")==0) {
        SystemCheck();
    }
    else 
    {
	    printf_err("Unknown command! Please check again!");
    }
    return ;
}

int FileManager::SplitStringIntoVector(const std::string& str, const std::string& regexStr, std::vector<std::string>& dst)
{
    std::regex re(regexStr);
    for (std::sregex_token_iterator it = std::sregex_token_iterator(str.begin(), str.end(), re, -1); 
         it != std::sregex_token_iterator(); ++it)
    {
        if((std::string) *it != "") {dst.push_back((std::string) *it);}
    }
    return 0;
}


unsigned FileManager::GetInodeIdFromPath(std::string path)
{
    return 0;
}
unsigned FileManager::GetDirectorySizeFromInode(Inode inode)
{
    size_t DirSize = 0;
    return DirSize;
}
std::string FileManager::GetFileFromInode(Inode)
{
    return "";
}

std::string FileManager::GetFileNameFromInode(Inode inode) {
    if (inode.iInodeId == 0) {
		return string("");
	}
	Inode parentInode = disk.oBlockManager.ReadInode(inode.iParent);
	Directory parentDir = ReadFilesFromDirectoryFile(parentInode);

	for (size_t i = 0; i < parentDir.vFiles.size(); i++)
	{
		if (parentDir.vFiles[i].InodeId == inode.iInodeId)
			return string(parentDir.vFiles[i].fileName);
	}
	printf_err("No such file or directory!");
	return "";
}


void FileManager::RecursiveDeleteDirectory(Inode inode)
{

}
int FileManager::WriteFilesToDirectoryFile(Directory dir, Inode inode)
{
    return 0;
}
Directory FileManager::ReadFilesFromDirectoryFile(Inode inode)
{
    Directory dir; //dir 存着文件列表
	File file;
    size_t fileSize = inode.fileSize; //文件大小
    size_t remainingSize = fileSize; // 还未读取的大小
    size_t readSize = 0;
    std::string allContent = "";

    Address currentAddr = inode.addrStart;
    Block block;
    do
    {
        disk.oBlockManager.ReadBlock(currentAddr, block);
        //do something
        if(remainingSize > disk.oSuperBlock.DATA_BLOCK_SIZE){readSize = disk.oSuperBlock.DATA_BLOCK_SIZE;}
        else {readSize = remainingSize;}

        allContent += std::string(block.content, readSize);
        remainingSize -= readSize;
        currentAddr = block.next;
    } while (currentAddr.to_int() != 0);
        
    for (size_t i = 0; i < (fileSize / sizeof(file)); i++)
        {
            memcpy(&file, &allContent + i*sizeof(file), sizeof(File));
            dir.vFiles.push_back(file);    
        }
    
    return dir;
}


void FileManager::PrintWelcomeInfo()
{
    printf("#################################################\n");
    printf("#    Index-node-based File Management System    #\n");
    printf("#                  Version 1.0                  #\n");
    printf("#                                               #\n");
    printf("#                 Developed by:                 #\n");
    printf("#                 Yang Hongshen                 #\n");
    printf("#                 201830671129                  #\n");
    printf("#################################################\n");
    printf("#   School of Computer Science and Engineering  #\n");
    printf("#      South China University of Technology     #\n");
    printf("#################################################\n");
    printf("\nYou can type 'help' to get command instructions!\n");
    printf("\n");
}

std::string FileManager::GetFullFilePath(Inode inode)
{
    string lastChar = (inode.bIsDir ? "/" : "");
    string result = "/";
    std::stack<string> directories;
    //入栈
    while (inode.iInodeId != 0)
	{
		directories.push(GetFileNameFromInode(inode));
		inode = disk.oBlockManager.ReadInode(inode.iParent);
	}
    //出栈
    while (!directories.empty()) {
		result += (directories.top() + (directories.size() == 1 ? lastChar : "/"));
		directories.pop();
	}
	return result;
}