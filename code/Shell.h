#pragma once

#ifndef SHELL_H
#define SHELL_H

#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<string>
#include<vector>
#include<algorithm>

#include"Kernel.h"

class Shell
{
public:
	Shell();
	~Shell();

private:
	/*-------输入码-------*/
	enum InputCode
	{
		FFORMAT_CODE = 0X1001,//数字是我随意设的，以帮助调试
		EXIT_CODE =0X0000,
		LS_CODE = 0X2001,
		CD_CODE = 0X2002,
		MKDIR_CODE = 0X2101,
		FCREAT_CODE = 0X2201,
		FDELETE_CODE = 0X2002,
		FOPEN_CODE = 0X2211,
		FCLOSE_CODE = 0X2212,
		FSEEK_CODE = 0X2213,
		FREAD_CODE = 0X2221,
		FWRITE_CODE = 0X2222
	};

private:
    /* 初始化函数 */
	void Initialize(std::ostream&);
	unsigned int CommandLine(std::istream&, std::ostream&);
	bool GetCommand(std::istream&, std::vector<std::string>&);

	/* 判断参数是否合法 */
	bool JudgeFileModePara(const std::vector<std::string>&, std::ostream&);
	bool JudgeDirPara(const std::vector<std::string>&, std::ostream&);
	bool JudgeFilePara(const std::vector<std::string>&, std::ostream&);
	bool JudgeFd(const std::vector<std::string>&, std::ostream&);
	bool JudgeSeekPara(const std::vector<std::string>&, std::ostream&);
	bool JudgeRdWtPara(const std::vector<std::string>&, std::ostream&);

	/* 判断是否是合法路径 */
	bool isValidPath(const std::string, bool isDir);
	/* 判断是否是合法模式 */
	bool isValidMode(const std::string);

	/* 输出提示列表 */
	void helpList(std::istream&, std::ostream&, std::string);
	void cmdList(std::ostream&);

	/* 调用kernel函数 自动构建*/
	void Build(std::ostream&);
};
#endif
