#include "Utility.h"
#include "Kernel.h"
#include <vector>
#include <cstring>


void Utility::MemCopy(void *dst, const void *src, size_t size) 
{
	::memcpy(dst, src, size);
}

void Utility::MemSet(void *dst, int val, size_t size) 
{
	::memset(dst, val, size);
}

int Utility::MemCmp(const void *a, const void *b, unsigned int count)
{
	return ::memcmp(a, b, count);
}

time_t Utility::Time(time_t* t) 
{
	return ::time(t);
}

int Utility::Min(int a, int b)
{
	return a < b ? a : b;
}

std::string Utility::SimplifyPath(const std::string orgPath)
{
	std::vector<std::string> stk;   //vector模拟栈
	int n = orgPath.size(), i = 0;
	std::string str = "";
	while (i < n) {
		//遇到 / 一直往后，直到非斜杠
		if (orgPath[i] == '/')
			++i;
		else {
			//将两斜杠间的字符组成串，再进行判断
			for (; i < n && orgPath[i] != '/'; ++i)
				str += orgPath[i];
			if (str == ".");  //当前目录，什么都不做
			else if (str == "..") {
				//返回上级目录（弹出栈顶元素）
				if (!stk.empty()&&stk.back()!="..")
					stk.pop_back();
				else {
					stk.push_back(str);
				}
			}
			//其余情况都认为是文件或目录名
			else
				stk.push_back(str);
			//str置空，用于存储下一个文件名
			str = "";
		}
	}
	if (stk.empty()) //栈为空表示仍在根目录
		return "/";
	std::string ans = "";
	for (auto& s : stk)
		//注意补'/'
		ans += ('/' + s);
	return ans.substr(1);//去掉开头的"/"
}

int Utility::GetInodeMode(const std::string mode)
{
	int res = 0;
	for (int i = 1; i < mode.size(); i++) {
		if (tolower(mode[i]) == 'r')
			res |= Inode::IREAD;
		else if (tolower(mode[i]) == 'w')
			res |= Inode::IWRITE;
	}
	return res;
}

int Utility::GetFileMode(const std::string mode)
{
	int res = 0;
	for (int i = 1; i < mode.size(); i++) {
		if (tolower(mode[i]) == 'r')
			res |= File::FREAD;
		else if (tolower(mode[i]) == 'w')
			res |= File::FWRITE;
	}
	return res;
}

bool Utility::isDigit(const std::string str)
{
	for (int i = 0; i < str.size(); i++) {
		if (!isdigit(str[i]))
			return false;
	}
	return true;
}

void Utility::Panic(const std::string error, const int errorCode, std::ostream& out)
{
	out << error;
	exit(errorCode);
}