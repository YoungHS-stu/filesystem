#pragma once
#include"MACROS.h"
#include"Address.h"
#include"utils.h"
class Inode {
public:
    Inode() {}
    ~Inode() {};
    Inode(unsigned fileSize, int parent, int inode_id, int owner_id=0, bool isDir=false)
    {   
        UpdateCreatedTime();
        UpdateModifiedTime();
        UpdateAccessedTime();
        this->fileSize = fileSize;
        this->iParent = parent;
        this->iInodeId = inode_id;
        this->bIsDir = isDir;
        this->iOwnerId = owner_id;
    }

	unsigned int iInodeId; //4B
    unsigned int fileSize;    //文件大小 4B
   	Address addrStart;    //3B,补齐到4B
	time_t tCreatedTime;  //8B
	time_t tModifiedTime; //8B
	time_t tAccessedTime; //8B
	bool bIsDir;   //大小1B，补齐到4B
	int  iParent;  //4B
    unsigned int  iOwnerId; //4B

    void UpdateCreatedTime()  {time(&tCreatedTime); }
    void UpdateModifiedTime() {time(&tModifiedTime);}
    void UpdateAccessedTime() {time(&tAccessedTime);}
    std::string GetCreatedTimeStr()  {return time_to_str(tCreatedTime); }
    std::string GetModifiedTimeStr() {return time_to_str(tModifiedTime);}
    std::string GetAccessedTimeStr() {return time_to_str(tAccessedTime);}
    std::string time_to_str(time_t time) {
        std::string time_str = ctime((time_t const*)&(time));
	    time_str = time_str.substr(0, time_str.size() - 1);
	    return time_str;
    }
};