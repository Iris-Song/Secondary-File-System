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
	/*-------������-------*/
	enum InputCode
	{
		FFORMAT_CODE = 0X1001,//��������������ģ��԰�������
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
    /* ��ʼ������ */
	void Initialize(std::ostream&);
	unsigned int CommandLine(std::istream&, std::ostream&);
	bool GetCommand(std::istream&, std::vector<std::string>&);

	/* �жϲ����Ƿ�Ϸ� */
	bool JudgeFileModePara(const std::vector<std::string>&, std::ostream&);
	bool JudgeDirPara(const std::vector<std::string>&, std::ostream&);
	bool JudgeFilePara(const std::vector<std::string>&, std::ostream&);
	bool JudgeFd(const std::vector<std::string>&, std::ostream&);
	bool JudgeSeekPara(const std::vector<std::string>&, std::ostream&);
	bool JudgeRdWtPara(const std::vector<std::string>&, std::ostream&);

	/* �ж��Ƿ��ǺϷ�·�� */
	bool isValidPath(const std::string, bool isDir);
	/* �ж��Ƿ��ǺϷ�ģʽ */
	bool isValidMode(const std::string);

	/* �����ʾ�б� */
	void helpList(std::istream&, std::ostream&, std::string);
	void cmdList(std::ostream&);

	/* ����kernel���� �Զ�����*/
	void Build(std::ostream&);
};
#endif
