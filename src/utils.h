#pragma once
#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
#include<vector>
#include<stack>
#include<regex>
#include<time.h>
#include<windows.h>
#include"MACROS.h"
#include <math.h>



#define VERBOSE false 
#define STEPS false
#define SHOW_ERR_MSG true
#define Print_Directory_File true
#define Print_Vector_String false

#define filename(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x
#define __SOURCE__ std::string(filename(__FILE__)) + "| " + std::string(__FUNCTION__) + ": " + std::to_string(__LINE__)

#define printf_v(format, argv...) if(VERBOSE) {printf(format, ##argv);} 
#define printf_src(format, argv...) if(STEPS) {printf("[%s]: ",std::string(__SOURCE__).c_str()); printf(format, ##argv);}
#define printf_err(format, argv...) if(SHOW_ERR_MSG) {printf_src("Error: "); printf(format, ##argv); printf("!\n");}

#define Fseek(pFile, offSet, fromWhere, argv...) _Fseek(pFile, offSet, fromWhere, __SOURCE__, ##argv);
#define Fopen(pFile, name, mode, argv...) _Fopen(pFile, name, mode, __SOURCE__, ##argv);
#define Fwrite(buffer, elementSize, elementCount, pFile, argv...) _Fwrite(buffer, elementSize, elementCount, pFile, __SOURCE__, ##argv);
#define Fread(buffer, elementSize, elementCount, pFile, argv...) _Fread(buffer, elementSize, elementCount, pFile, __SOURCE__, ##argv);


//#define __file__err_control(res, correct,pFile, msg, error_close_require) if (r!=correct) { perror(msg); if (error_close_require)fclose(pFile);}
static int _Fseek(FILE* pFile, long offSet, int fromWhere, std::string source="",
				std::string msg="no", bool verbose=VERBOSE, bool error_close_require=false)
{
    if(verbose) printf_v("[%s]: call Fseek: %s\n",source.c_str(), msg.c_str());
    int r = fseek(pFile, offSet, fromWhere);
    if (r) { perror("fseek()"); if (error_close_require)fclose(pFile); return r;}
	return 0;
}

static errno_t _Fopen(FILE** pFile, const char* name, const char* mode, std::string source="",
				std::string msg="no", bool verbose=VERBOSE, bool error_close_require=false)
{
    if(verbose) printf_v("[%s]: call Fopen: %s\n",source.c_str(), msg.c_str());
	errno_t r = fopen_s(pFile, name, mode);
    if (r) { perror("fopen_s()"); if (error_close_require)fclose(*pFile); return r;}
	return 0;
}

static size_t _Fread(void* buffer, size_t elementSize, size_t elementCount, FILE* pFile, std::string source="",
				 std::string msg="no", bool verbose=VERBOSE, bool error_close_require=false)
{
    if(verbose) printf_v("[%s]: call Fread: %s\n",source.c_str() ,msg.c_str());
    size_t r = fread(buffer, elementSize, elementCount, pFile);
	if (r != elementCount) { 
		perror("Fread()"); 
		if (feof(pFile)) printf("EOF error!\n");
		else printf("Error code: %d\n", ferror(pFile));
		if (error_close_require)fclose(pFile);
        return r;
	}
	return elementCount;
}

static size_t _Fwrite(const void* buffer, size_t elementSize, size_t elementCount, FILE* pFile, std::string source="",
				 std::string msg="no", bool verbose=VERBOSE, bool error_close_require=false)
{
    if(verbose) printf_v("[%s]: call Fwrite: %s\n",source.c_str() ,msg.c_str());
	size_t r = fwrite(buffer, elementSize, elementCount, pFile);
	if (r != elementCount) {
		perror("Fread()");
		if (feof(pFile)) printf("EOF error!\n");
		else printf("Error code: %d\n", ferror(pFile));
		if (error_close_require)fclose(pFile);
        return r;
	}
	return elementCount;
}

class File{
public:
	char fileName[MAXIMUM_FILENAME_LENGTH+1];
	unsigned int InodeId; 
	File() {memset(fileName,0,sizeof(fileName));}
	File(const char* fname, unsigned int InodeId){
		File();
		strcpy(fileName, fname);
		this->InodeId = InodeId;
	}
};

class Directory {
public:
	std::vector<File> vFiles;
	int FindFiles(const char* name){
		for (size_t i = 0; i < vFiles.size(); i++){
			// ??????0???????????????
			if (strcmp(vFiles[i].fileName, name)==0) return vFiles[i].InodeId;
		}
		return -1;
	}
};

static void PrintDirectoryFile(Directory& dir)
{
	if(Print_Directory_File){ return ;}
	printf("dir.vFiles.size: %d\n", dir.vFiles.size());
	for (size_t i = 0; i < dir.vFiles.size(); i++)
	{
		printf("%d: file name:%s file id: %d\n", i, dir.vFiles[i].fileName, dir.vFiles[i].InodeId);
	}
	
}

static void PrintVectorString(std::vector<std::string> v)
{
	if(Print_Vector_String){ return ;}
	for (size_t i = 0; i < v.size(); i++)
	{
		std::cout<< i <<": " << v[i] << std::endl; 
	}
	
}


