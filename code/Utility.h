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

	/* ���ļ�·�� */
	static std::string SimplifyPath(const std::string orgPath);
	/* ��ö�ȡģʽ */
	static int GetInodeMode(const std::string mode);
	static int GetFileMode(const std::string mode);

	/* ����ں˴�����Ϣ,���ش���һ��������ô˺��� */
	static void Panic(const std::string error, const int errorCode, std::ostream &out= std::cout);

};

#endif
