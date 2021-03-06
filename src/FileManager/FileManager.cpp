#include"FileManger.h"

#define CHECK_REDUNDANT_ARGS()  char* redundant = strtok(NULL, " ");if (redundant != NULL) {printf_err("more arguments than expected");return;}
#define CHECK_PATH_ARGS()       char* path = strtok(NULL, " ");if (path == NULL) {printf_err("lack of path");return;}
#define CHECK_SIZE_ARGS()       char* size = strtok(NULL, " ");if (size == NULL) {printf_err("lack of size");return;} unsigned fileSize = atof(size);if (fileSize == 0) {printf("Wrong file size format or file size is 0! Please check again!\n");return;}


static void PrintInode(Inode inode){
	printf("Inode.id %d\n", inode.iInodeId);
	printf("Inode.filesize %d\n", inode.fileSize);
	printf("Inode.addrStart %d\n", inode.addrStart.to_int());
	printf("Inode.ParentId %d\n", inode.iParent);
	printf("Inode.isDir %d\n", inode.bIsDir);
}

static void PrintBlock(Block block){
	printf("Block.nextaddr %d\n", block.next.to_int());
	char head[12];
	memcpy(head, block.content, sizeof(head));
	printf("Block content.head %s\n", head);
}


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
        //PrintInode(disk.oCurrentInode);
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

std::string FileManager::myModifyFile(char* path, std::string content)
{
    int inode_id = GetInodeIdFromPath(path);
    Inode inode = disk.oBlockManager.ReadInode(inode_id);
    if (content.length() > inode.fileSize)
    {
        return "content too long";
    }
    
    return "";
}


std::string FileManager::myCreateFile(char* path, int filesize, std::string content)
{
    printf("creating my file\n");
    std::string result="";
    std::vector<std::string> pathList;
    if (CheckPathHelper(path, pathList) != 0) return "Invalid file path! Maximum length for each level: 63";


    int inode_id_ptr = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < pathList.size(); i++)
    {
        Inode inode_ptr = disk.oBlockManager.ReadInode(inode_id_ptr);
        if (!inode_ptr.bIsDir) {
            printf("%s is a file! You can not create file under here!\n", GetFileNameFromInode(inode_ptr).c_str());
            return "";
        }
        Directory dir = ReadFilesFromDirectoryFile(inode_ptr);
        if (i!=pathList.size()-1){ //????????????????????????
            int nextInode = dir.FindFiles(pathList[i].c_str());
            if (nextInode != -1) {
                inode_id_ptr = nextInode;
                continue; //???????????????????????????, ???????????????
            }
            else{
                return "path not exits, mkdir first\n";
            }
        }
        else{//???????????????, ????????????: 1??????????????????, 2???????????????
            int nextInode = dir.FindFiles(pathList[i].c_str());
            if (nextInode != -1) { //??????????????????
                return "file already exits";
            }
            else{ //???????????????, ????????????
                int remainingSize = filesize;
                int rwSize = filesize; //??????buffer?????????
                int inodeId, addrInt; Address blockAddr; Block block; Inode inode; //?????????
                addrInt = disk.oBlockManager.GetNextFreeBlock();
                inodeId = disk.oBlockManager.GetNextFreeInode();
                inode = Inode(filesize, inodeId, Address(addrInt), inode_ptr.iInodeId); //startaddr ??????????????????
                printf("inode in %s before writing\n", __FUNCTION__);
                PrintInode(inode);
                printf("new inode: %d, new block:%d\n", inodeId, addrInt);
                disk.oBlockManager.WriteNewInode(inode); //??????inode
                //???????????????????????????
                do {
                    if(filesize > disk.oSuperBlock.DATA_BLOCK_SIZE) 
                    {rwSize = disk.oSuperBlock.DATA_BLOCK_SIZE;addrInt = disk.oBlockManager.GetNextFreeBlock();}
                    else {rwSize=remainingSize;}                    

                    blockAddr = Address(addrInt);
                    if(remainingSize == filesize){ //????????????block?????????
                        //????????????, ??????block.next??????0
                    }
                    else{
                        block.next = blockAddr;  //????????????next????????????
                    }
                    block = Block(0);

                    memset(block.content, '\0', filesize);//????????????\0
                    for (size_t i = 0; i < content.length(); i++)
                    {
                        memset(block.content+i, content[i], 1);
                    }
                    // memset(block.content+i, '\0', 1);
                    disk.oBlockManager.WriteNewBlock(blockAddr, block);
                    remainingSize -= rwSize;
                } while(remainingSize > 0);
                
                // ??????????????????????????????entry
                File newfile = File(pathList[i].c_str(), inode.iInodeId);    
                dir.vFiles.push_back(newfile);
                // PrintDirectoryFile(dir);

                Address currentAddr = inode_ptr.addrStart;
                Block currentblock;
                disk.oBlockManager.ReadBlock(currentAddr, currentblock);
                
                if(inode_ptr.fileSize + sizeof(File) < disk.oSuperBlock.DATA_BLOCK_SIZE)
                {
                    for (size_t i = 0; i < dir.vFiles.size(); i++)
                    {
                        memcpy(currentblock.content+i*sizeof(File), &dir.vFiles[i], sizeof(File));
                    }
                    disk.oBlockManager.WriteBlock(currentAddr, currentblock);

                }
                else //??????????????????????????????????????????????????????????????????????????????block
                {
                    //TODO ?????????
                    addrInt = disk.oBlockManager.GetNextFreeBlock();
                    currentblock.next = Address(addrInt);
                    currentAddr =  Address(addrInt);
                    Block newblock = Block(0); //???block???next?????????0
                    for (size_t i = 0; i < dir.vFiles.size(); i++)
                    {
                        memcpy(newblock.content+i*sizeof(File), &dir.vFiles[i], sizeof(File));
                    }
                    disk.oBlockManager.WriteNewBlock(currentAddr, newblock);
                    
                }
                //??????inode
                inode_ptr.fileSize += sizeof(File);
                disk.oBlockManager.WriteInode(inode_ptr);
                if(disk.oCurrentInode.iInodeId == inode_ptr.iInodeId){
                    disk.oCurrentInode = inode_ptr;
                }
                
                printf("Create file successfully!\n");
                return "";
            }
        }
    }
    
    return result;
}

std::string FileManager::myOpenFile(char* path)
{
    std::string result="";
    std::vector<std::string> pathList;
    if (CheckPathHelper(path, pathList) != 0) return "Invalid file path! Maximum length for each level: 63";

    int inode_id_ptr = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < pathList.size(); i++)
    {
        Inode inode_ptr = disk.oBlockManager.ReadInode(inode_id_ptr);
        Directory dir = ReadFilesFromDirectoryFile(inode_ptr);
        if(i!=pathList.size()-1) { //????????????????????????
            int nextInode = dir.FindFiles(pathList[i].c_str());
            if (nextInode != -1) { //??????????????????????????????????????????
                Inode tempInode = disk.oBlockManager.ReadInode(nextInode);
                {
                    if(!tempInode.bIsDir){
                        return "path not exits";
                    }
                }
                inode_id_ptr = nextInode;
                continue; //???????????????????????????, ???????????????
            }
            else{
                return "path not exits, mkdir first\n";
            }
        }
        else{
            int nextInode = dir.FindFiles(pathList[i].c_str());
            if (nextInode != -1) { //??????????????????????????????????????????
                Inode tempInode = disk.oBlockManager.ReadInode(nextInode);
                {
                    if(tempInode.bIsDir){
                        return "This is a direcotry, not a file";
                    }
                }
                
                Block block;
                disk.oBlockManager.ReadBlock(tempInode.addrStart, block);
                std::string file_content = "";
                do
                {
                    for (size_t i = 0; i < disk.oSuperBlock.DATA_BLOCK_SIZE; i++)
                    {
                        if(block.content[i] == '\0') return file_content;
                        file_content += block.content[i];
                    }
                    disk.oBlockManager.ReadBlock(block.next, block);
                } while (block.next.to_int()!=0);
                
                return file_content;
            }
            else{
                return "File not exits";
            }
        }
    }

    return result;
}
std::string FileManager::myDeleteFile(char* path)
{
    std::string result="";
    std::vector<std::string> pathList;
    if (CheckPathHelper(path, pathList) != 0) return "Invalid file path! Maximum length for each level: 63";


    int inode_id_ptr = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < pathList.size(); i++)
    {
        Inode inode_ptr = disk.oBlockManager.ReadInode(inode_id_ptr);
        Directory dir = ReadFilesFromDirectoryFile(inode_ptr);
        if(i!=pathList.size()-1) //????????????????????????
        {
            int nextInode = dir.FindFiles(pathList[i].c_str());
            if (nextInode != -1) { //??????????????????????????????????????????
                Inode tempInode = disk.oBlockManager.ReadInode(nextInode);
                {
                    if(!tempInode.bIsDir){
                        return "path not exits";
                    }
                }
                inode_id_ptr = nextInode;
                continue; //???????????????????????????, ???????????????
            }
            else{
                return "path not exits, mkdir first\n";
            }
        }
        else //???????????????, ????????????: 1??????????????????, 2???????????????
        {
            int nextInode = dir.FindFiles(pathList[i].c_str());
            if (nextInode != -1) { //????????????, ????????????

                // -1 clear file inode
                Inode inode = disk.oBlockManager.ReadInode(nextInode);
                if (inode.bIsDir)
                {
                    return "This is a directory. To delete directory, use [rmdir] instead";
                }
                //if (DeleteFileHelper(inode) < 0) return "Delete File Failure";

                PrintInode(inode);
                disk.oBlockManager.ClearInode(inode);
               
                // -2 clear file block
                printf("Addr of File block to be claer %d\n", inode.addrStart.to_int());
                Address blockAddr = Address(inode.addrStart.to_int());
                Block tempBlock;
                disk.oBlockManager.ReadBlock(inode.addrStart, tempBlock);
                PrintBlock(tempBlock);

                disk.oBlockManager.ClearBlock(blockAddr);

                // -3 remove file in dir
                printf("before removing file from dir vector\n");
                PrintDirectoryFile(dir);
                for (size_t i = 0; i < dir.vFiles.size(); i++)
                {
                    if (dir.vFiles[i].InodeId == nextInode)
                    {
                        dir.vFiles.erase(dir.vFiles.begin() + i);
                    }
                }
                printf("after removing file from dir vector\n");
                PrintDirectoryFile(dir);
                
                // -4 update dir inode
                printf("before inode size:%d  id: %d\n", inode_ptr.fileSize, inode_ptr.iInodeId);
                inode_ptr.fileSize -= sizeof(File);
                disk.oBlockManager.WriteInode(inode_ptr);
                if(disk.oCurrentInode.iInodeId == inode_ptr.iInodeId){
                    disk.oCurrentInode = inode_ptr;
                }
                printf("after  inode size:%d  id: %d\n", inode_ptr.fileSize, inode_ptr.iInodeId);
                printf("disk.oCurrentInode size:%d  id: %d\n", disk.oCurrentInode.fileSize, disk.oCurrentInode.iInodeId);

                // -5 update dir block
                Block dirBlock;
                printf("current inode start addrInt: %d \n", inode_ptr.addrStart.to_int());
                disk.oBlockManager.ReadBlock(inode_ptr.addrStart, dirBlock);
                dirBlock.next = Address(inode_ptr.addrStart.to_int());
                printf("block next addr: %d\n", dirBlock.next.to_int());

                // TODO ??????????????????????????????
                for (size_t i = 0; i < dir.vFiles.size(); i++)
                {
                    memcpy(dirBlock.content+i*sizeof(File), &dir.vFiles[i], sizeof(File));
                }
                disk.oBlockManager.WriteBlock(inode_ptr.addrStart, dirBlock);

                return "delete file success!";
            }
            else{
                return "file not exits";
            }
        }
        
    }



    return result;
}
int FileManager::CheckPathHelper(char* path, std::vector<std::string>& dst){
    std::vector<std::string> vPathList = SplitStringIntoVector(std::string(path), "/");
    PrintVectorString(vPathList);
    for (size_t i = 0; i < vPathList.size(); i++)
    {
        if (vPathList[i].length() > MAXIMUM_FILENAME_LENGTH) {
            return -1;
        }
        dst.push_back(vPathList[i]);
    }
    return 0;
}
std::string FileManager::myCopyFile(char* from_path, char* to_path)
{
    std::string result="";
    std::vector<std::string> fromPathList;
    std::vector<std::string> toPathList;

    if (CheckPathHelper(from_path, fromPathList) != 0) return "Invalid source path! Maximum length: 63";
    if (CheckPathHelper(to_path,   toPathList)   != 0) return "Invalid target path! Maximum length: 63";

    int from_inode_id = GetInodeIdFromPath(from_path);
    if (from_inode_id < 0); "Source file does not exist";
    
    int inode_id_ptr = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < toPathList.size(); i++) {
        Inode inode_ptr = disk.oBlockManager.ReadInode(inode_id_ptr);
        Directory dir = ReadFilesFromDirectoryFile(inode_ptr);

        int nextInode = dir.FindFiles(toPathList[i].c_str());
        if(i!=toPathList.size()-1){ //??????????????????
            if (nextInode != -1) { //??????????????????????????????????????????
                Inode tempInode = disk.oBlockManager.ReadInode(nextInode);
                {
                    if(!tempInode.bIsDir){ //???????????????
                        return "target path not exits";
                    }
                }
                inode_id_ptr = nextInode;
                continue; //???????????????????????????, ???????????????
            }
            return "target path not exits\n";
        }
        else{
            if (nextInode != -1) { //??????????????????????????????????????????
                return "There is file or dir in target path, use a new path\n";
            }
            // ??????????????????

            // ?????????????????????

            // ??????????????????inode
        }
    
    }

    return result;
}

std::string FileManager::myCreateDirectory(char* path)
{
    std::string result="";
    std::vector<std::string> vPathList;
    if (CheckPathHelper(path, vPathList) != 0) return "Invalid dir path! Maximum length: 63";
    
    int inode_id_ptr = disk.oCurrentInode.iInodeId;
    for (size_t i = 0; i < vPathList.size(); i++) {

        Inode inode_ptr = disk.oBlockManager.ReadInode(inode_id_ptr); 
        if (!disk.oCurrentInode.bIsDir) {
				result = GetFileNameFromInode(disk.oCurrentInode) + " is a file! You can not create directory under here!\n";
                return result;
		}
        Directory dir = ReadFilesFromDirectoryFile(inode_ptr);
        int nextDirInodeId = dir.FindFiles(vPathList[i].c_str());

        if(i!=vPathList.size()-1){ //??????????????????
            if(nextDirInodeId != -1) {//???????????????
                inode_id_ptr= nextDirInodeId; continue;//???????????????inode(??????) 
            }
        }
        else{   //????????????
                //?????????inode???block
            if(nextDirInodeId != -1) {//???????????????
                if(disk.oBlockManager.ReadInode(nextDirInodeId).bIsDir){ //???????????????????????????
                    return "Directory aldready exits";
                }
            }
            //-1???????????????dir file
            printf_src("creating new");
            int inodeId = disk.oBlockManager.GetNextFreeInode();
            int addrInt = disk.oBlockManager.GetNextFreeBlock();
            printf_src("new inode: %d, new block:%d\n", inodeId, addrInt);

            Directory newtDir;
            newtDir.vFiles.push_back(File(".", inodeId));
            newtDir.vFiles.push_back(File("..",inode_id_ptr)); //inode_id_ptr ?????? parent
            size_t DirSize = newtDir.vFiles.size() * sizeof(File);

            Address blockAddr = Address(addrInt);
            Block block = Block(0);
            Inode inode = Inode(DirSize, inodeId, blockAddr, inode_id_ptr ,true);
            for (size_t i = 0; i < newtDir.vFiles.size(); i++)
            {
                memcpy(block.content+i*sizeof(File), &newtDir.vFiles[i], sizeof(File));
            }
            disk.oBlockManager.WriteNewBlock(blockAddr, block);
            disk.oBlockManager.WriteNewInode(inode);

            //-2?????????????????????????????????
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
                printf_src("oCurrentInode filesize before: %d\n", disk.oCurrentInode.fileSize);
                inode_ptr.fileSize += sizeof(File);
                if(disk.oCurrentInode.iInodeId == inode_ptr.iInodeId){
                    disk.oCurrentInode = inode_ptr;
                }
                printf_src("oCurrentInode filesize after : %d\n", disk.oCurrentInode.fileSize);
                disk.oBlockManager.WriteBlock(inode_ptr.addrStart, dirblock);
                disk.oBlockManager.WriteInode(inode_ptr);
            }

        }
        
        
    }

    return result;
}
std::string FileManager::myDeleteDirectory(char* path)
{
    std::string result="";
    std::vector<std::string> pathList;
    if (CheckPathHelper(path, pathList) != 0) return "Invalid dir path! Maximum length: 63";


    int to_be_deleted_inode_id = GetInodeIdFromPath(path);   
    if (to_be_deleted_inode_id < 0){
        return "Directory not found!";
    }
    Inode to_be_deleted_dir_inode = disk.oBlockManager.ReadInode(to_be_deleted_inode_id);
    if (!to_be_deleted_dir_inode.bIsDir) {
        return "This is a file. You can use 'rmfile' command to delete a file!";
    }
    
    if (to_be_deleted_dir_inode.iInodeId == disk.oCurrentInode.iInodeId) {
            return "You cannot delete current working directory!";	
	}

    return RecursiveDeleteDirectory(to_be_deleted_dir_inode);

}
std::string FileManager::myChangeDirectory(char* path)
{

    int nextINodeId = GetInodeIdFromPath(path);
    if(nextINodeId==-1) {return "Directory '"+ std::string(path) +"' not found";}

    Inode nextInode = disk.oBlockManager.ReadInode(nextINodeId);
    PrintInode(nextInode);
    if (!nextInode.bIsDir) return  std::string(path) + "is a file! Not a directory!\n";
    
    disk.SetCurrentInode(nextINodeId);
    return "change directory success";
}
std::string FileManager::myPrintWorkingDirectory() //?????????
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
    std::string result="";
    result += "Create a file: mkfile fileName fileSize(in Byte) content \neg. mkfile /mydir/hello.txt 10 hello world^, use ^ to indicate end of input\n\n";
	result += "Delete a file: rmfile filename \neg. rmfile /mydir/hello.txt\n\n";
	result += "Create a directory: mkdir dirName\neg. mkdir /dir1/sub1\n\n";
	result += "Delete a directory: rmdir dirName\neg. rmdir /dir1/sub1\n\n";
	result += "Change current working directory: cd dirName\neg. cd dir2\n\n";
	result += "List directory: dir or ls \n\n";
	result += "Copy file: cp sourceFile targetFile\neg. cp /dir1/sub1/hello.txt /dir2/sub2/hello.txt\n\n";
	result += "Print out file contents: cat fileName\neg. cat /dir1/file1\n\n";
	result += "Display the info of storage space: info\n\n";
    return result;
}

std::string FileManager::SystemCheck()
{
    std::string result="Doing system check!\n";
    
    return result;
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
        CHECK_PATH_ARGS();
        CHECK_REDUNDANT_ARGS();
        if (!std::regex_match(std::string(path), fileNamePattern))
		{
			printf_err("Your directory name does not meet the regex specification '^([a-z]|[A-Z]|[_/.]|[0-9])*$' "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores");
			return;
		}
        printf("%s\n",myDeleteDirectory(path).c_str());

    }
    else if (strcmp(command, "cd")==0) { //-?????????
        CHECK_PATH_ARGS();
        CHECK_REDUNDANT_ARGS();
        if (!std::regex_match(std::string(path), fileNamePattern))
		{
			printf("Your directory name does not meet the specification. regex specification '^([a-z]|[A-Z]|[_/.]|[0-9])*$' "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores\n");
			return;
		}

        printf("%s\n",myChangeDirectory(path).c_str());
    }
    else if (strcmp(command, "pwd")==0) { //-?????????
        printf("%s\n",GetWorkingDirectory().c_str());
    }
    else if (strcmp(command, "dir")==0 || strcmp(command, "ls")==0) { //-?????????
        CHECK_REDUNDANT_ARGS();
		printf("Current directory file size: %d\n", disk.oCurrentInode.fileSize);

        std::string result = myListDirectory();
        printf("%s\n",result.c_str());
    }
    else if (strcmp(command, "cp")==0) {
        CHECK_PATH_ARGS();
        CHECK_REDUNDANT_ARGS();
        printf("%s\n",myCopyFile(path,path).c_str());
    }
    else if (strcmp(command, "rmfile")==0) {
        
        CHECK_PATH_ARGS();
        CHECK_REDUNDANT_ARGS();
        if (!std::regex_match(std::string(path), fileNamePattern))
		{
			printf("Your file name does not meet the specification. "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores\n");
			return;
		}
        printf("%s\n",myDeleteFile(path).c_str());
    }
    else if (strcmp(command, "mkfile")==0) {
        CHECK_PATH_ARGS();  //char *path??????
		CHECK_SIZE_ARGS();  //char *size??? unsigned filesize??????
        char* content = strtok(NULL, "^");
        std::string content_str = "";
        if (content == NULL) {
            printf("no content, default empty file will be created\n"); 
            content_str = "default";
        }
        else {printf("content will be written: %s\n", content_str = std::string(path));}
        CHECK_REDUNDANT_ARGS();
        if (!std::regex_match(std::string(path), fileNamePattern))
		{
			printf("Your file name does not meet the specification. "
				"The file name can only consist of uppercase or lowercase English letters, numbers or underscores\n");
			return;
		}
        printf("%s",myCreateFile(path, fileSize, content_str).c_str());

        return;
    }
    else if (strcmp(command, "cat")==0) {
        CHECK_PATH_ARGS();
        CHECK_REDUNDANT_ARGS();
        printf("%s\n",myOpenFile(path).c_str());
    }
    else if (strcmp(command, "vim")==0) {  //TODO ?????????
        CHECK_PATH_ARGS();
        char* content = strtok(NULL, "^");
        std::string content_str = "";
        if (content == NULL) {
            printf("no content, default empty file will be created\n"); 
            content_str = "default";
        }
        else {printf("content will be written: %s\n", content_str = std::string(path));}
        CHECK_REDUNDANT_ARGS();
        printf("%s\n",myOpenFile(path).c_str());
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

std::vector<std::string> FileManager::SplitStringIntoVector(const std::string& str, const std::string& regexStr)
{
    std::vector<std::string> dst;
    std::regex re(regexStr);
    for (std::sregex_token_iterator it = std::sregex_token_iterator(str.begin(), str.end(), re, -1); 
         it != std::sregex_token_iterator(); ++it)
    {
        if((std::string) *it != "") {dst.push_back((std::string) *it);}
    }
    return dst;
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
            targetInodeId = nextDirectoryInodeId; //???????????????
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


std::string FileManager::RecursiveDeleteDirectory(Inode inode)
{
    //printf("inode to be deleted!\n");
    //PrintInode(inode);
    if (inode.fileSize == 2 * sizeof(File)) { //?????????????????????????????????
        std::string filePath = GetFullFilePath(inode);
        if (DeleteFileHelper(inode) < 0 ) {return "Delete directory" + GetFullFilePath(inode) + "Failed"; }
        else {return "Delete directory successful";}
    }

    Directory dir = ReadFilesFromDirectoryFile(inode); //???????????????????????????
    PrintDirectoryFile(dir);
    for (size_t i = 0; i < dir.vFiles.size(); i++)
    {
        if (!strcmp(dir.vFiles[i].fileName, ".") || !strcmp(dir.vFiles[i].fileName, "..")) 
			continue;
        
        Inode inode_to_delete = disk.oBlockManager.ReadInode(dir.vFiles[i].InodeId);
        if (inode_to_delete.bIsDir) { //?????????????????????????????????
            //printf("deletting dir\n");
			RecursiveDeleteDirectory(inode_to_delete);
		}
        else{
            //printf("deletting file\n");
            //std::string filePath = GetFullFilePath(inode_to_delete);
            if (DeleteFileHelper(inode_to_delete) < 0) {
                return "Delete File" + GetFullFilePath(inode_to_delete) + "Failed";
            }
        }
    }
    
    //????????????
    if (DeleteFileHelper(inode) < 0) return "delete directory failed";
    return "Delete directory successful";
}
int FileManager::WriteFilesToDirectoryFile(Directory dir, Inode inode)
{
    return 0;
}
Directory FileManager::ReadFilesFromDirectoryFile(Inode inode)
{
    size_t fileSize = inode.fileSize; //????????????
    Directory dir; //dir ??????????????????
    File tempFile = File("-",999);

	File file;
    size_t remainingSize = fileSize; // ?????????????????????
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
            if(offset > disk.oSuperBlock.DATA_BLOCK_SIZE) // ????????????block
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
    //??????
    while (inode.iInodeId != 0)
	{
		directories.push(GetFileNameFromInode(inode));
		inode = disk.oBlockManager.ReadInode(inode.iParent);
	}
    //??????
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


int FileManager::WriteDirFilesToDirBlock(Directory) 
{
    return 0;
}

int FileManager::CreateFileHelper(Inode inode) //?????????inode, ??????
{
    return 0;
}


int FileManager::DeleteFileHelper(Inode inode) //?????????inode, ?????????inode?????????block??????
{
    PrintInode(inode);
    disk.oBlockManager.ClearInode(inode);
    disk.oBlockManager.ClearBlock(inode.addrStart);

    Inode parent = disk.oBlockManager.ReadInode(inode.iParent);
    Directory dir = ReadFilesFromDirectoryFile(parent);
    PrintDirectoryFile(dir);
    for (size_t i = 0; i < dir.vFiles.size(); i++)
    {
        if(dir.vFiles[i].InodeId == inode.iInodeId) dir.vFiles.erase(dir.vFiles.begin()+i);
    }
    PrintDirectoryFile(dir);

    parent.fileSize -= sizeof(File);
    disk.oBlockManager.WriteInode(parent);
    if(disk.oCurrentInode.iInodeId == parent.iInodeId){
        disk.oCurrentInode = parent;
    }


    Block parentDirBlock;
    disk.oBlockManager.ReadBlock(parent.addrStart, parentDirBlock);
    
    for (size_t i = 0; i < dir.vFiles.size(); i++)
    {
        memcpy(parentDirBlock.content + i*sizeof(File), &dir.vFiles[i], sizeof(File));
    }
    disk.oBlockManager.WriteBlock(parent.addrStart, parentDirBlock);

    return 0;
}

static std::string CheckPathAndFilePattern(std::string path, std::regex fileNamePattern)
{
    if (!std::regex_match(std::string(path), fileNamePattern))
    {
        return "Your directory name does not meet the regex specification '^([a-z]|[A-Z]|[_/.]|[0-9])*$'\n \
        The file name can only consist of uppercase or lowercase English letters, numbers or underscores";
    }
    
    return "";
}