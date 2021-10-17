class FileManger
{
public:
    FileManger(/* args */);
    ~FileManger();
    int CreateFile(); //newfile
    int OpenFile(); // cat
    int DeleteFile(); //rm
    int CopyFile(); //cp

    int CreateDirectory();//mkdir
    int DeleteDirectory();//rmdir
    int ChangeDirectory();//cd
    int PrintWorkingDirectory();//pwd
    int ListDirectory(); // ls or dir

    int PrintDiskInfo(); // info
    int SystemCheck(); // check
    int PrintHelp(); //help
};


