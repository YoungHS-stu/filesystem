#include"FileManger.h"

#define CHECK_REDUNDANT_ARGS  char* redundant = strtok(NULL, " ");if (redundant != NULL) {printf_err("too many arguments");return;}
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

        printf("[%s@YFS %s]$: ", "root", GetWorkingDirectory().c_str());
		scanf("%[^\n]", &userInput);
        getchar();
        
	    //if (strtok(userInput, " ") == NULL) continue;
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

std::string FileManager::myCreateFile()
{
    return "";
}
std::string FileManager::myOpenFile()
{
    return "";
}
std::string FileManager::myDeleteFile()
{
    return "";
}
std::string FileManager::myCopyFile()
{
    return "";
}

std::string FileManager::myCreateDirectory(char* path)
{
    std::string result="";
    printf_src("\n");
    std::vector<std::string> vPathList;
    SplitStringIntoVector(std::string(path), "/", vPathList);
    PrintVectorString(vPathList);
    for (size_t i = 0; i < vPathList.size(); i++)
    {
        if (vPathList[i].length() > MAXIMUM_FILENAME_LENGTH - 1) {
            printf_err("The directory/file name: %s is too long! Maximum length: %d", vPathList[i].c_str(), MAXIMUM_FILENAME_LENGTH - 1);
            return "";
        }
    }

   
    for (size_t i = 0; i < vPathList.size(); i++) {
        
        if (!disk.oCurrentInode.bIsDir) {
				printf("%s is a file! You can not create directory under here!\n", GetFileNameFromInode(disk.oCurrentInode).c_str());
				return "";
		}
        Directory dir = ReadFilesFromDirectoryFile(disk.oCurrentInode);
        int nextDirInodeId = dir.FindFiles(vPathList[i].c_str());

        if(nextDirInodeId != -1)//找到了目录
        {
            disk.oCurrentInode = disk.oBlockManager.ReadInode(nextDirInodeId);//指向下一个inode(目录)
        }
        else
        { //没有找到
            //找空闲inode和block
            //-1先创建新的dir file
            printf_src("creating new");
            int inodeId = disk.oBlockManager.GetNextFreeInode();
            int addrInt = disk.oBlockManager.GetNextFreeBlock();
            Directory newtDir;
            File temp = File(".",inodeId);
            newtDir.vFiles.push_back(temp);
            temp = File("..",disk.oCurrentInode.iInodeId);
            newtDir.vFiles.push_back(temp);
            Address blockAddr = Address(addrInt);
            size_t DirSize = newtDir.vFiles.size() * sizeof(File);
            Block block = Block(0);
            Inode inode = Inode(DirSize, inodeId, blockAddr, disk.oCurrentInode.iInodeId ,true);
            for (size_t i = 0; i < newtDir.vFiles.size(); i++)
            {
                memcpy(block.content+i*sizeof(File), &newtDir.vFiles[i], sizeof(File));
            }
            disk.oBlockManager.WriteNewBlock(blockAddr, block);
            disk.oBlockManager.WriteNewInode(inode);

            //-2然后写目录到当前目录中
            printf_src("putting current");
            if(disk.oCurrentInode.fileSize + sizeof(File) < disk.oSuperBlock.DATA_BLOCK_SIZE)
            {
                File newfile = File(vPathList[i].c_str(),0);
                dir.vFiles.push_back(newfile);
                Block block = Block(0); 
                for (size_t i = 0; i < newtDir.vFiles.size(); i++)
                {
                    memcpy(block.content+i*sizeof(File), &newtDir.vFiles[i], sizeof(File));
                }
                disk.oBlockManager.WriteBlock(disk.oCurrentInode.addrStart, block);
                disk.oBlockManager.WriteInode(inode);
            }

            if (i == vPathList.size() - 1) {
                return "create directory successfully!\n";
		    }
        }
        
        return "directory already exists\n";
    }







    return "";
}
std::string FileManager::myDeleteDirectory()
{
    return "";
}
std::string FileManager::myChangeDirectory()
{
    return "";
}
std::string FileManager::myPrintWorkingDirectory()
{
    printf("%s\n",GetWorkingDirectory().c_str());
    return "";
}

std::string FileManager::GetWorkingDirectory() {
    return GetFullFilePath(disk.oCurrentInode);
}
std::string FileManager::myListDirectory()
{
    std::string result = "";
	Directory dir = ReadFilesFromDirectoryFile(disk.oCurrentInode);
    result += "File(s) and directory(s) of " + GetFullFilePath(disk.oCurrentInode)
            + "\nFile Name\tFile Size\tFile Type\tCreate Time\t\t\tModified Time\t\t\tInode ID\n";

    std::string fileName;
    Inode fileInode;

    for (size_t i = 0; i < dir.vFiles.size(); i++)
    {
        fileName = dir.vFiles[i].fileName;
        fileInode = disk.oBlockManager.ReadInode(dir.vFiles[i].InodeId);
        for (size_t j = 0; j < (int)ceil((double)fileName.size() / 14); j++)
		{
			if (j == 0) {
                result += fileName.substr(0,14);
                result += fileName.size() >= 8 ? "\t" : "\t\t";
                result += std::to_string(fileInode.fileSize) + " B\t\t";
                if(fileInode.bIsDir) result += "Dir";
                else result += "File";
                result += "\t\t" + fileInode.GetCreatedTimeStr();
                result += "\t"   + fileInode.GetModifiedTimeStr();
                result += "\t"   + std::to_string(fileInode.iInodeId) + "\n";
            }
            else {
                result += fileName.substr(j * 14, 14) + "\n";
            }
        }
    }
    result += "\n";
    return result;
}

std::string FileManager::PrintDiskInfo()
{
    return "";
    
}

std::string FileManager::PrintHelp()
{
    return "";
}

std::string FileManager::SystemCheck()
{
        return "";
}


void FileManager::CmdParser()
{
    char* command = strtok(userInput, " ");
	if (command == NULL) {
		return;
	}
	
    if (strcmp(command, "mkdir")==0) {
        char* path = strtok(NULL, " ");
		if (path == NULL) {
			printf_err("lack of path");
			return;
		}
        CHECK_REDUNDANT_ARGS;

        if (!std::regex_match(std::string(path), fileNamePattern))
		{
			printf_err("Your directory name does not meet the regex specification '^([a-z]|[A-Z]|[_/.]|[0-9])*$' "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores");
			return;
		}
        printf_src("creating dir with path:%s\n", path);
        printf("%s",myCreateDirectory(path).c_str());
    }
    else if (strcmp(command, "rmdir")==0) {
        myDeleteDirectory();
    }
    else if (strcmp(command, "cd")==0) {
        myChangeDirectory();
    }
    else if (strcmp(command, "pwd")==0) { //-已完成
        myPrintWorkingDirectory();
    }
    else if (strcmp(command, "dir")==0 || strcmp(command, "ls")==0) {
        CHECK_REDUNDANT_ARGS;
		printf("Current directory file size: %d\n", disk.oCurrentInode.fileSize);

        std::string result = myListDirectory();
        printf("%s\n",result.c_str());
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
		return std::string("");
	}
	Inode parentInode = disk.oBlockManager.ReadInode(inode.iParent);
	Directory parentDir = ReadFilesFromDirectoryFile(parentInode);

	for (size_t i = 0; i < parentDir.vFiles.size(); i++)
	{
		if (parentDir.vFiles[i].InodeId == inode.iInodeId)
			return std::string(parentDir.vFiles[i].fileName);
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
    size_t fileSize = inode.fileSize; //文件大小
    Directory dir; //dir 存着文件列表
    File tempFile = File("-",999);

	File file;
    size_t remainingSize = fileSize; // 还未读取的大小
    size_t readSize = 0;
    size_t pos = 0;
    std::string allContent = "";

    Address currentAddr = inode.addrStart;
    Block block;
    // do
    // {

        //do something
        // if(remainingSize > disk.oSuperBlock.DATA_BLOCK_SIZE){readSize = disk.oSuperBlock.DATA_BLOCK_SIZE;}
        // else {readSize = remainingSize;}

        disk.oBlockManager.ReadBlock(currentAddr, block);
        int offset;
        int numOfFiles = fileSize/sizeof(File);
        for(size_t i = 0; i < numOfFiles; i++)
        {
            offset = i*sizeof(File);
            if(offset > disk.oSuperBlock.DATA_BLOCK_SIZE) // 需要多个block
            {
                int currentBlockLeft = disk.oSuperBlock.DATA_BLOCK_SIZE - (i-1)*sizeof(File);
                int nextBlockLeft = offset - disk.oSuperBlock.DATA_BLOCK_SIZE;
                memcpy(&tempFile, block.content+(i-1)*sizeof(File), currentBlockLeft);

                disk.oBlockManager.ReadBlock(block.next, block);
                memcpy(&tempFile+currentBlockLeft, block.content, nextBlockLeft);
                offset -= disk.oSuperBlock.DATA_BLOCK_SIZE;
                
            }
            else{
                memcpy(&tempFile, block.content+offset, sizeof(File));
            }
            dir.vFiles.push_back(tempFile);
        }

    //     memcpy(&dir+pos, block.content, readSize);
    //     pos+=readSize;
    //     remainingSize -= readSize;
    //     currentAddr = block.next;
    // } while (currentAddr.to_int() != 0);
        
    // for (size_t i = 0; i < (fileSize / sizeof(file)); i++)
    // {
    //     memcpy(&file, &allContent + i*sizeof(file), sizeof(file));
    //     dir.vFiles.push_back(file);    
    // }
    //memcpy(&dir, &allContent, sizeof(fileSize));

    return dir;
}


std::string FileManager::PrintWelcomeInfo()
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
    return "";
}

std::string FileManager::GetFullFilePath(Inode inode)
{
    std::string lastChar = (inode.bIsDir ? "/" : "");
    std::string result = "/";
    std::stack<std::string> directories;
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


