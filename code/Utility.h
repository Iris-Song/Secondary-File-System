#ifndef UTILITY_H
#define UTILITY_H

#include<string>
#include<iostream>
#include<time.h>

class Utility
{
public:
	static void MemCopy(void* dst, const void* src, size_t size);
	static void MemSet(void* dst, int val, size_t size);
	static int MemCmp(const void* a, const void* b, unsigned int count);

	static time_t Time(time_t* t);

	static int Min(int a, int b);

	static bool isDigit(const std::string);

	/* 简化文件路径 */
	static std::string SimplifyPath(const std::string orgPath);
	/* 获得读取模式 */
	static int GetInodeMode(const std::string mode);
	static int GetFileMode(const std::string mode);

	/* 输出内核错误信息,严重错误，一般错误不适用此函数 */
	static void Panic(const std::string error, const int errorCode, std::ostream &out= std::cout);

};

#endif
