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
    // disk.run();
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
    std::string workingPath;
    return workingPath;
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
