#pragma once

#ifndef KERNEL_H
#define KERNEL_H

#define _CRT_SECURE_NO_WARNINGS

#include "BufferManager.h"
#include "DiskDriver.h"
#include "OpenFileManager.h"
#include "FileSystem.h"
#include "User.h"


class Kernel
{
public:
	friend class Shell;

public:
	Kernel();
	~Kernel();
	static Kernel& Instance();
	void Initialize();		/* 该函数完成初始化内核大部分数据结构的初始化 */

	BufferManager& GetBufferManager();
	DiskDriver& GetDiskDriver();
	FileSystem& GetFileSystem();
	FileManager& GetFileManager();
	User& GetUser();

public:
	void Format();          /* 格式化，原V6++中不存在*/
	std::string Ls();
	bool MakeDir(const std::string);
	void ChangeDir(const std::string);
	bool CreateFile(const std::string, const std::string);
	bool DeleteFile(const std::string);
	bool OpenFile(const std::string, const std::string);
	bool CloseFile(const int);
	void SeekFile(const int, const int, const int);
	void Read(const int, int,const std::string);
	bool Write(const int, const std::string, int);
	bool OutputError(std::ostream&);
	std::string GetCurDir();

private:
	void InitBuffer();
	void InitFileSystem();
	void InitUser();

	void FormatBuffer();
	void FormatFileSystem();

private:
	static Kernel instance;		/* Kernel单体类实例 */

	BufferManager* m_BufferManager;
	DiskDriver* m_DiskDriver;
	FileSystem* m_FileSystem;
	FileManager* m_FileManager;
	User* m_User;

};

#endif
