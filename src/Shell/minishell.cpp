#include<iostream>
#include<string>
#include<vector>
#include <sstream>
#include"FileManger.h"


int main()
{
    std::cout << "------------Welcome to Young File System------------" << std::endl;
    bool bRunning = true;
    std::string sInput;
    std::string sCurrentDir = "/";
    std::string sCurrentUser = "root";
    std::vector<std::string> vCmdTokens;
    FileManger fm;
    while (bRunning)
    {
        std::string sCmdHeader = "["+sCurrentUser+"@YFS "+sCurrentUser+"]$ ";
        std::cout << sCmdHeader;
        getline(std::cin, sInput);
        // use istringstream to separate input words with whitespace as delimiters
        std::istringstream inputStream(sInput);
        std::string sToken;
        while (inputStream >> sToken) { vCmdTokens.push_back(sToken);}
        if (vCmdTokens.empty())
        {
            continue;
        }
        if (vCmdTokens[0] == "exit") {
            if (vCmdTokens.size() == 1) {
                bRunning = false;
            }
            std::cout << sCmdHeader << "Command not found! Do you mean 'exit' ?\n" ;
        }
        else if (vCmdTokens[0] == "dir" || vCmdTokens[0] == "ls") {
            std::cout << "list all" << std::endl;
        }
        else if (vCmdTokens[0] == "cd") {
            fm.ChangeDirectory();
        }
        else if (vCmdTokens[0] == "mkdir") {
            fm.CreateDirectory();
        }
        else if (vCmdTokens[0] == "rmdir") {
            fm.DeleteDirectory();
        }
        else if (vCmdTokens[0] == "newfile") {
            fm.CreateFile();
        }
        else if (vCmdTokens[0] == "cp") {
            fm.CopyFile();    
        }
        else if (vCmdTokens[0] == "cat") {
            fm.OpenFile();   
        }
        else if (vCmdTokens[0] == "info") {
            fm.PrintDiskInfo();    
        }
        else if (vCmdTokens[0] == "check") {
            fm.SystemCheck();    
        }
        else if (vCmdTokens[0] == "help") {
            fm.PrintHelp();
        }
        vCmdTokens.clear();
    }
    
    std::cout << "Bye~" << std::endl;
    return 0;
}