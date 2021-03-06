#pragma once
#include"Address.h"
#include"utils.h"
class Block{
public:
    Address next; //3 bytes
	char content[INITIAL_DATA_BLOCK_SIZE]; //1021 bytes
	Block() {memset(content, 0, INITIAL_DATA_BLOCK_SIZE);}
	Block(int addrInt) {
		Block();
		next = Address(addrInt);
	}
	
	Block(Address addr) {
		Block();
		next = addr;
	}
	
    void refreshContent() {memset(content, 0, sizeof content);}

	// load a block from disk given an address to the content buffer
	void load(int addrInt, FILE* pFile=NULL, int numByte=INITIAL_BLOCK_SIZE) {

		refreshContent();
		if(pFile==NULL) Fopen(&pFile,DISK_FILE_NAME, "rb+");
		Fseek(pFile, addrInt, SEEK_SET, __FUNCTION__);
		Fread(content, numByte, 1, pFile, __FUNCTION__);
		if (pFile!=NULL) fclose(pFile);
	}

	void load(Address addr, FILE* pFile = NULL, int numByte=INITIAL_BLOCK_SIZE) {
		int addrInt = addr.to_int();
		load(addrInt, pFile);
	}		

	// write the content buffer to the specific disk block
	void write(int addrInt, FILE* pFile=NULL, int numByte=INITIAL_BLOCK_SIZE) {
		if(pFile==NULL) Fopen(&pFile, DISK_FILE_NAME, "rb+");
		Fseek(pFile, addrInt, SEEK_SET);
		Fwrite(content, numByte, 1, pFile);
		if (pFile!=NULL) fclose(pFile);
		refreshContent();
	}		
	void write(Address addr, FILE* pFile = NULL, int numByte=INITIAL_BLOCK_SIZE) {
		int addrInt = addr.to_int();
		write(addrInt, pFile);
	}	
	
};