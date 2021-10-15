#pragma once
#include"MACROS.h"

class INode {
public:
    INode();
    ~INode();
    unsigned fileSize; //文件大小
	time_t tCreatedTime;
	time_t tAccessedTime;
	time_t tModifiedTime;
	bool bIsDir; //默认不是目录
	int iParent; //暂时不知道用处
	int iInodeId;
    int iOwnerId;

   	Address direct[DIRECT_ADDRESS_NUMBER];
	Address indirect;
    INode();
    INode(unsigned, int, int, bool=false);
    int UpdateCreatedTime();
    int UpdateModifiedTime();
    int UpdateAccessedTime();
    std::string GetCreatedTimeStr();
    std::string GetModifiedTimeStr();
    std::string GetAccessedTimeStr();
};