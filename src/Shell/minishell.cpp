#include<iostream>
#include<string>
#include<vector>
#include <sstream>

using namespace std;

int main()
{
    cout << "------------Welcome to Young File System------------" << endl;
    bool bRunning = true;
    string sInput;
    string sCurrentDir = "/";
    string sCurrentUser = "root";
    vector<string> vCmdTokens;

    while (bRunning)
    {
        string sCmdHeader = "["+sCurrentUser+"@YFS "+sCurrentUser+"]$ ";
        cout << sCmdHeader;
        getline(cin, sInput);
        // use istringstream to separate input words with whitespace as delimiters
        istringstream inputStream(sInput);
        string sToken;
        while (inputStream >> sToken) { vCmdTokens.push_back(sToken);}
        if (vCmdTokens.empty())
        {
            continue;
        }
        if (vCmdTokens[0] == "exit") {
            if (vCmdTokens.size() == 1) {
                bRunning = false;
            }
            cout << sCmdHeader << "Command not found! Do you mean 'exit' ?\n" ;
        }
        else if (vCmdTokens[0] == "dir" || vCmdTokens[0] == "ls") {
            cout << "list all" << endl;
        }


        // for(string token : vCmdTokens) {
        //     cout << token << " | ";
        // }
        // cout << endl;
        vCmdTokens.clear();
    }
    
    cout << "Bye~" << endl;
    return 0;
}