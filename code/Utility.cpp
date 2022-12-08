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
	std::vector<std::string> stk;   //vectorģ��ջ
	int n = orgPath.size(), i = 0;
	std::string str = "";
	while (i < n) {
		//���� / һֱ����ֱ����б��
		if (orgPath[i] == '/')
			++i;
		else {
			//����б�ܼ���ַ���ɴ����ٽ����ж�
			for (; i < n && orgPath[i] != '/'; ++i)
				str += orgPath[i];
			if (str == ".");  //��ǰĿ¼��ʲô������
			else if (str == "..") {
				//�����ϼ�Ŀ¼������ջ��Ԫ�أ�
				if (!stk.empty()&&stk.back()!="..")
					stk.pop_back();
				else {
					stk.push_back(str);
				}
			}
			//�����������Ϊ���ļ���Ŀ¼��
			else
				stk.push_back(str);
			//str�ÿգ����ڴ洢��һ���ļ���
			str = "";
		}
	}
	if (stk.empty()) //ջΪ�ձ�ʾ���ڸ�Ŀ¼
		return "/";
	std::string ans = "";
	for (auto& s : stk)
		//ע�ⲹ'/'
		ans += ('/' + s);
	return ans.substr(1);//ȥ����ͷ��"/"
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