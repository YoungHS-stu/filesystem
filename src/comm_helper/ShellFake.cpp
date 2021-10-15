#include<iostream>
#include<string>
#include<windows.h>
using namespace std;

const int BUF_SIZE = 4096;
 
int main()
{
    cout << "This is a Fake Shell" << endl;
    char szBuffer[] = "Hello Shared Data";

    // 共享文件句柄
    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE, //物理文件句柄，INVALID_HANDLE_VALUE与物理文件无关
        NULL, //安全
        PAGE_READWRITE, //保护
        0, //高位
        BUF_SIZE, //低位
        "ShareMemory" //名称
    );
    // 映射缓存区视图 , 得到指向共享内存的指针
    LPVOID lpBase = MapViewOfFile(
        hMapFile,            // 共享内存的句柄
        FILE_MAP_ALL_ACCESS, // 可读写许可
        0,
        0,
        BUF_SIZE
        );

    // 将数据拷贝到共享内存
    strcpy((char*)lpBase,szBuffer);

    // 线程挂起等其他线程读取数据
    cout << "Sleep(20000);" << endl;
    Sleep(20000);

    // 解除文件映射
    UnmapViewOfFile(lpBase);
    // 关闭内存映射文件对象句柄
    CloseHandle(hMapFile);

    cout << "End of Fake Shell" << endl;

    return 0;
}