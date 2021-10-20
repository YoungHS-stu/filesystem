#include"FileManger.h"

#define CHECK_REDUNDANT_ARGS()  char* redundant = strtok(NULL, " ");if (redundant != NULL) {printf_err("more arguments than expected");return;}
#define CHECK_PATH_ARGS()       char* path = strtok(NULL, " ");if (path == NULL) {printf_err("lack of path");return;}
#define CHECK_SIZE_ARGS()       char* size = strtok(NULL, " ");if (size == NULL) {printf_err("lack of size");return;} unsigned fileSize = atof(size) * 1024;if (fileSize == 0) {printf("Wrong file size format or file size is 0! Please check again!\n");return;}
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

std::string FileManager::myCreateFile(char* path, int filesize)
{
    std::string result="";
    std::vector<std::string> pathList;
    SplitStringIntoVector(std::string(path), "/", pathList);

    for (size_t i = 0; i < pathList.size(); i++)
    {
        if (pathList[i].length() > MAXIMUM_FILENAME_LENGTH - 1) {
            printf_err("The directory/file name: %s is too long! Maximum length: %d", pathList[i].c_str(), MAXIMUM_FILENAME_LENGTH - 1);
            return "";
        }
    }

    int inode_id_ptr = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < pathList.size(); i++)
    {
        Inode inode_ptr = disk.oBlockManager.ReadInode(inode_id_ptr);
        if (!inode_ptr.bIsDir) {
            printf("%s is a file! You can not create directory under here!\n", GetFileNameFromInode(inode_ptr).c_str());
            return "";
        }
        Directory dir = ReadFilesFromDirectoryFile(inode_ptr);
        int nextInode = dir.FindFiles(pathList[i].c_str());
        if (nextInode != -1) {
            inode_id_ptr = nextInode;
        }
        else {
            if (i != pathList.size() - 1) {
                printf("keep parsing");
            }
            else {
            
                // 一个block就行
                int remainingSize = filesize;
                int rwSize = filesize;
                int inodeId, addrInt; Address blockAddr; Block block; Inode inode; //先声明
                inodeId = disk.oBlockManager.GetNextFreeInode();
                inode = Inode(filesize, inodeId, Address(addrInt), disk.oCurrentInode.iInodeId);
                disk.oBlockManager.WriteNewInode(inode);

                do {
                    if(filesize > disk.oSuperBlock.DATA_BLOCK_SIZE) {rwSize = disk.oSuperBlock.DATA_BLOCK_SIZE;}
                    else {rwSize=remainingSize;}

                    addrInt = disk.oBlockManager.GetNextFreeBlock();
                    printf("new inode: %d, new block:%d\n", inodeId, addrInt);
                    blockAddr = Address(addrInt);
                    block = Block(addrInt);
                    memset(block.content, '+', filesize);
                    disk.oBlockManager.WriteNewBlock(blockAddr, block);
                    remainingSize -= rwSize;
                } while(remainingSize > 0);
                

                // 写进当前目录，加一个entry

                File newfile = File(pathList[i].c_str(), inode.iInodeId);    
                dir.vFiles.push_back(newfile);
                PrintDirectoryFile(dir);

                Address currentAddr = inode_ptr.addrStart;
                Block block;
                disk.oBlockManager.ReadBlock(currentAddr, block);
                
                if(inode_ptr.fileSize + sizeof(File) < disk.oSuperBlock.DATA_BLOCK_SIZE)
                {

                }
                else //当前的不够了，剩余的小部分空间丢弃，新建一个block
                {
                    addrInt = disk.oBlockManager.GetNextFreeBlock();
                    block.next = Address(addrInt);
                    block=Block(addrInt);
                }
                    for (size_t i = 0; i < dir.vFiles.size(); i++)
                    {
                        memcpy(block.content+i*sizeof(File), &dir.vFiles[i], sizeof(File));
                    }
                    
                    inode_ptr.fileSize += sizeof(File);
                    disk.oBlockManager.WriteBlock(currentAddr, block);
                    disk.oBlockManager.WriteInode(inode_ptr);

                    disk.oCurrentInode = inode_ptr;
                
                printf("Create file successfully!\n");
                return "";
            }

        }
    }
    printf("File already exists!\n");
    return result;
}
std::string FileManager::myOpenFile()
{
    std::string result="";

    return result;
}
std::string FileManager::myDeleteFile()
{
    std::string result="";

    return result;
}
std::string FileManager::myCopyFile()
{
    std::string result="";

    return result;
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
            printf("new inode: %d, new block:%d\n", inodeId, addrInt);

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
            printf_src("putting into current\n");
            if(disk.oCurrentInode.fileSize + sizeof(File) < disk.oSuperBlock.DATA_BLOCK_SIZE)
            {
                File newfile = File(vPathList[i].c_str(),inode.iInodeId);
                dir.vFiles.push_back(newfile);
                Address currentAddr = disk.oCurrentInode.addrStart;
                Block dirblock;
                //disk.oBlockManager.ReadBlock(currentAddr, dirblock);

                PrintDirectoryFile(dir);
               
                for (size_t i = 0; i < dir.vFiles.size(); i++)
                {
                    memcpy(dirblock.content+i*sizeof(File), &dir.vFiles[i], sizeof(File));
                }
                printf("oCurrentInode filesize before: %d\n", disk.oCurrentInode.fileSize);
                disk.oCurrentInode.fileSize += sizeof(File);
                printf("oCurrentInode filesize after : %d\n", disk.oCurrentInode.fileSize);
                disk.oBlockManager.WriteBlock(disk.oCurrentInode.addrStart, dirblock);
                disk.oBlockManager.WriteInode(disk.oCurrentInode);
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
    std::string result="";

    return result;
}
std::string FileManager::myChangeDirectory(char* path)
{

    int nextINodeId = GetInodeIdFromPath(path);
    if(nextINodeId==-1) {return "Directory '"+ std::string(path) +"' not found";}

    Inode nextInode = disk.oBlockManager.ReadInode(nextINodeId);
    if (!nextInode.bIsDir) return  std::string(path) + "is a file! Not a directory!\n";
    
    disk.SetCurrentInode(nextINodeId);
    return "change directory success";
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
        CHECK_PATH_ARGS();
        CHECK_REDUNDANT_ARGS();

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
        printf("%s\n",myDeleteDirectory().c_str());
    }
    else if (strcmp(command, "cd")==0) { //-已完成
        CHECK_PATH_ARGS();
        CHECK_REDUNDANT_ARGS();

        printf("%s\n",myChangeDirectory(path).c_str());
    }
    else if (strcmp(command, "pwd")==0) { //-已完成
        printf("%s\n",GetWorkingDirectory().c_str());
    }
    else if (strcmp(command, "dir")==0 || strcmp(command, "ls")==0) { //-已完成
        CHECK_REDUNDANT_ARGS();
		printf("Current directory file size: %d\n", disk.oCurrentInode.fileSize);

        std::string result = myListDirectory();
        printf("%s\n",result.c_str());
    }
    else if (strcmp(command, "cp")==0) {
        printf("%s\n",myCopyFile().c_str());
    }
    else if (strcmp(command, "rmfile")==0) {
        printf("%s\n",myDeleteFile().c_str());
    }
    else if (strcmp(command, "mkfile")==0) {
        CHECK_PATH_ARGS();  //char *path在这
		CHECK_SIZE_ARGS();  //char *size和 unsigned filesize在这
        CHECK_REDUNDANT_ARGS();
        if (!std::regex_match(std::string(path), fileNamePattern))
		{
			printf("Your file name does not meet the specification. "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores\n");
			return;
		}
        printf("%s",myCreateFile(path, fileSize).c_str());
        return;
    }
    else if (strcmp(command, "cat")==0) {
        printf("%s\n",myOpenFile().c_str());
    }
    else if (strcmp(command, "help")==0) {
        printf("%s\n",PrintHelp().c_str());
    }
    else if (strcmp(command, "info")==0) {
        printf("%s\n",PrintDiskInfo().c_str());
    }
    else if (strcmp(command, "check")==0) {
        printf("%s\n",SystemCheck().c_str());
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


int FileManager::GetInodeIdFromPath(std::string path)
{
    std::vector<std::string> pathList;
    SplitStringIntoVector(path, "/", pathList);    
    PrintVectorString(pathList);
    int targetInodeId = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < pathList.size(); i++)
    {
        Inode targetInode = disk.oBlockManager.ReadInode(targetInodeId);
        if (!targetInode.bIsDir)
        {
            printf_err("Current directory is a file!")
            return -2;
        }

        Directory dir = ReadFilesFromDirectoryFile(targetInode);
		int nextDirectoryInodeId = dir.FindFiles(pathList[i].c_str());
		if (nextDirectoryInodeId != -1) {
            targetInodeId = nextDirectoryInodeId; //找到匹配的
		}
		else { 
			printf("Directory not found: %s\n", pathList[i].c_str());
			return -1; 
		}
    }
    
    return targetInodeId;
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


