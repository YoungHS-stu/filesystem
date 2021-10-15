#pragma once
#include<string>
#include <regex>
#include"SuperBlock.h"
#include"INode.h"
#include"utils.h"
#include"MACROS.h"


using namespace std;

class Disk {
public:
    Disk();
    ~Disk();
    int run();
    string getWelcomeMessage();
    // INode* 
private:
    INode* pINode;
    char* pDiskStart;
    SuperBlock* pSuperBlock;
    char* pINodeBitMap;
    char* pDataBitMap;
    char* pDataStart;
    regex fileNamePattern;
};