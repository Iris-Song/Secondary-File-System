#include "Shell.h"
#include "Utility.h"
#include <regex> //正则表达式
#include <sstream>

extern User g_User;

Shell::Shell()
{
	this->Initialize(std::cout);
	this->CommandLine(std::cin, std::cout);
}

Shell::~Shell()
{

}

void Shell::Initialize(std::ostream& out)
{
	out << "=================== UNIX FILE SYSTEM ===================" << "\n";
	out << "\n";
	out << "Welcome. Please enter build to initialize. Enter help to find more details." << "\n";
}

void Shell::cmdList(std::ostream& out)
{
	out << "Unix file system support commands:" << "\n"
		<< "fformat" << "\n"
		<< "exit" << "\n"
		<< "cd" << "\n"
		<< "ls" << "\n"
		<< "mkdir" << "\n"
		<< "fcreat" << "\n"
		<< "fdelete" << "\n"
		<< "fopen" << "\n"
		<< "fclose" << "\n"
		<< "fseek" << "\n"
		<< "fread" << "\n"
		<< "fwrite" << "\n"
		<< "Type `help -name' to find out more about the command `name'.\n";
}

void Shell::helpList(std::istream& in, std::ostream& out, std::string type = "global")
{
	if (type == "global") {
		cmdList(out);
	}
	else if (type.substr(1) == "exit") {
		out << "exit     : " << "\n"
			<< "discription :  exit this file system. \n"
			<< "               Remember use this way to exit. Otherwise some unintentional error may occur!!" << "\n"
			<< "usage       :  exit" << "\n";
	}
	else if (type.substr(1) == "fformat") {
		out << "fformat     : " << "\n"
			<< "discription :  format this file system" << "\n"
			<< "usage       :  fformat" << "\n";
	}
	else if (type.substr(1) == "cd") {
		out << "cd          : " << "\n"
			<< "discription :  change directory" << "\n"
			<< "usage       :  cd <dirname>" << "\n"
			<< "parameter   :  <dirname> a valid dirctory" << "\n"
			<< "example     :  cd mydir" << "\n"
			<< "               cd ../mysubdir/mydir" << "\n";
	}
	else if (type.substr(1) == "ls") {
		out << "ls          : " << "\n"
			<< "discription :  list files in current directory" << "\n"
			<< "usage       :  ls" << "\n";
	}
	else if (type.substr(1) == "mkdir") {
		out << "mkdir       : " << "\n"
			<< "discription :  make a new directory" << "\n"
			<< "usage       :  mkdir <dirname>" << "\n"
			<< "parameter   :  <dirname> a valid dirctory" << "\n"
			<< "example     :  mkdir mydir" << "\n"
			<< "               mkdir ../mysubdir/mydir" << "\n";
	}
	else if (type.substr(1) == "fcreat") {
		out << "fcreat      : " << "\n"
			<< "discription :  create a file" << "\n"
			<< "usage       :  fcreat <filepath> <mode>" << "\n"
			<< "parameter   :  <filepath> a valid file path" << "\n"
			<< "            :  <mode>     -r only read" << "\n"
			<< "            :  <mode>     -w only write" << "\n"
			<< "            :  <mode>     -rw -wr can read and write" << "\n"
			<< "example     :  fcreat myfile.txt -r" << "\n"
			<< "               fcreat ../subdir/myfile.txt - rw" << "\n";
	}
	else if (type.substr(1) == "fopen") {
		out << "fopen       : " << "\n"
			<< "discription :  open a file" << "\n"
			<< "            :  Remerber if you want to read or write a file. You have to open it first." << "\n"
			<< "usage       :  fopen <filepath> <mode>" << "\n"
			<< "parameter   :  <filepath> a valid file path" << "\n"
			<< "            :  <mode>     -r only read" << "\n"
			<< "            :  <mode>     -w only write" << "\n"
			<< "            :  <mode>     -rw -wr can read and write" << "\n"
			<< "example     :  fopen myfile.txt -r" << "\n"
			<< "               fopen ../subdir/myfile.txt - r" << "\n";
	}
	else if (type.substr(1) == "fclose") {
		out << "fclose      : " << "\n"
			<< "discription :  close a file" << "\n"
			<< "usage       :  fclose <file descriptor>" << "\n"
			<< "parameter   :  <file descriptor> a valid number to describle a file" << "\n"
			<< "example     :  fclose 1" << "\n";
	}
	else if (type.substr(1) == "fread") {
		out << "fread       : " << "\n"
			<< "discription :  read a opened file" << "\n"
			<< "usage       :  fread <file descriptor> [-o <out file name>] <size>" << "\n"
			<< "parameter   :  <file descriptor> a valid number to describle a file" << "\n"
			<< "               <out file name> a valid file path to output, default option is shell." << "\n"
			<< "               <size> read bytes" << "\n"
			<< "example     :  fread 1 100" << "\n"
			<< "            :  fread 1 -o myoutfile.txt 100" << "\n";
	}
	else if (type.substr(1) == "fwrite") {
		out << "fwrite      : " << "\n"
			<< "discription :  write a file" << "\n"
			<< "usage       :  fwrite <file descriptor> <in file name> <size>" << "\n"
			<< "parameter   :  <file descriptor> a valid number to describle a file" << "\n"
			<< "               <in file name> a opened valid file path to input." << "\n"
			<< "               <size> write bytes" << "\n"
			<< "example     :  fwrite 1 myinfile.txt 100" << "\n";
	}
	else if (type.substr(1) == "fseek") {
		out << "fseek       : " << "\n"
			<< "discription :  move file pointer" << "\n"
			<< "usage       :  fseek <file descriptor> <offset> <origin>" << "\n"
			<< "parameter   :  <file descriptor> a valid number to describle a file" << "\n"
			<< "               <offset> offest from original position. a number." << "\n"
			<< "               <original position>      0 -SEEK_SET" << "\n"
			<< "               <original position>      1 -SEEK_CUR" << "\n"
			<< "               <original position>      2 -SEEK_END" << "\n"
			<< "example     :  fseek 1 100 0" << "\n";
	}
	else if (type.substr(1) == "fdelete") {
		out << "fdelete     : " << "\n"
			<< "discription :  delete a file" << "\n"
			<< "usage       :  fdelete <filepath> " << "\n"
			<< "parameter   :  <filepath> a valid file path" << "\n"
			<< "example     :  fdelete myfile.txt -r" << "\n"
			<< "               fdelete ../subdir/myfile.txt - r" << "\n";
	}
	else if (type.substr(1) == "build") {
	out << "build     : " << "\n"
		<< "discription :  build the system. Generate file structure as follows" << "\n"
		<< "               +--- bin" << "\n"
		<< "               +--- etc" << "\n"
		<< "               +--- home" << "\n"
		<< "                    +--- texts" << "\n"
		<< "                         +--- ReadMe.txt" << "\n"
		<< "                    +--- reports" << "\n"
		<< "                         +--- report.pdf" << "\n"
		<< "                    +--- photos" << "\n"
		<< "                         +--- star.png" << "\n"
		<< "               +--- dev" << "\n"
		<< "example     :  build" << "\n";
	}
	else {
		out << "command \"" + type.substr(1) + "\" is not supported." << "\n";
		cmdList(out);
	}
}

unsigned int Shell::CommandLine(std::istream& in, std::ostream& out)
{
	do {
		std::vector<std::string>args;
		out << "$root" << Kernel::Instance().GetCurDir() << ">";
		if (GetCommand(in, args))
		{
			/*for (int i = 0; i < args.size(); i++) {
				out << args[i] << " ";
			}*/
			std::string orgCmd = args[0];
			transform(args[0].begin(), args[0].end(), args[0].begin(), ::tolower);

			if (args[0] == "exit")break;
			else if (args[0] == "fformat") {
				Kernel::Instance().Format();
			}
			else if (args[0] == "cd") {
				if (JudgeDirPara(args, out))
				{
					Kernel::Instance().ChangeDir(args[1]);
				};
			}
			else if (args[0] == "ls") {
				out << Kernel::Instance().Ls() << "\n";
			}
			else if (args[0] == "mkdir") {
				if (JudgeDirPara(args, out))
				{
					if (Kernel::Instance().MakeDir(args[1]))
					{
						out << "create directory \'" + args[1] + "\' successfully.\n";
					};
				};
			}
			else if (args[0] == "fcreat") {
				if (JudgeFileModePara(args, out))
				{
					if (Kernel::Instance().CreateFile(args[1], args[2]))
					{
						out << "create file \'" + args[1] + "\' successfully.\n";
					};
				};
			}
			else if (args[0] == "fopen") {
				if (JudgeFileModePara(args, out))
				{
					if (Kernel::Instance().OpenFile(args[1], args[2]))
					{
						out << "open success, return fd="
							<< Kernel::Instance().GetUser().u_ar0[User::EAX] << "\n";
					};
				};
			}
			else if (args[0] == "fclose") {
				if (JudgeFd(args, out))
				{
					if (Kernel::Instance().CloseFile(std::stoi(args[1])))
					{
						out << "close file \'" + args[1] + "\' successfully.\n";
					};
				}
			}
			else if (args[0] == "fread") {
				if (JudgeRdWtPara(args, out))
				{
					std::string filepath = args[2] == "-o" ? args[3] : "";
					int size = args[2] == "-o" ? std::stoi(args[4]) : std::stoi(args[2]);
					/* 正确处理在Read函数内部解决 */
					Kernel::Instance().Read(std::stoi(args[1]), size, filepath);
				}
			}
			else if (args[0] == "fwrite") {
				if (JudgeRdWtPara(args, out))
				{
					if (Kernel::Instance().Write(std::stoi(args[1]), args[2], std::stoi(args[3])))
					{
						out << "write " << Kernel::Instance().GetUser().u_ar0[User::EAX]
							<< " bytes successfully. \n";
					};
				}
			}
			else if (args[0] == "fseek") {
				if (JudgeSeekPara(args, out))
				{
					Kernel::Instance().SeekFile(std::stoi(args[1]), std::stoi(args[2]), std::stoi(args[3]));
				}
			}
			else if (args[0] == "fdelete") {
				if (JudgeFilePara(args, out))
				{
					if (Kernel::Instance().DeleteFile(args[1]))
					{
						out << "delete \'" << args[1] << "\' success. \n";
					};
				};
			}
			else if (args[0] == "help") {
				if (args.size() > 1)
					helpList(std::cin, std::cout, args[1]);
				else
					helpList(std::cin, std::cout);
			}
			else if (args[0] == "build")
			{
				Build(out);
			}
			else {
				out << orgCmd << ": command not found\n";
			}
			Kernel::Instance().OutputError(out);
		}

	} while (true);

	return 0;
}

//读取一条指令，将其划分，返回读取是否成功
bool Shell::GetCommand(std::istream& in, std::vector<std::string>& args)
{
	std::string cmd;
	std::string arg;
	args.clear();

	getline(in, cmd);

	std::size_t previous = 0;
	std::size_t current = cmd.find(' ');
	while (current != std::string::npos) {
		if (current > previous) {
			args.push_back(cmd.substr(previous, current - previous));
		}
		previous = current + 1;
		current = cmd.find(' ', previous);
	}
	if (previous != cmd.size()) {
		args.push_back(cmd.substr(previous));
	}

	if (!args.size())return false;
	return true;
}

bool Shell::JudgeDirPara(const std::vector<std::string>& args, std::ostream& out)
{
	if (args.size() < 2) {
		out << "number of parameters is less than required. "
			<< "Use \'help -" + args[0] + "\' for more information." << "\n";
		return false;
	}
	if (!isValidPath(args[1], true)) {
		out << "path \'" + args[1] + "\' is not a valid path. "
			<< "Use \'help -" + args[0] + "\' for more information." << "\n";
		return false;
	}
	return true;
}

bool Shell::JudgeFilePara(const std::vector<std::string>& args, std::ostream& out)
{
	if (args.size() < 2) {
		out << "number of parameters is less than required. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (!isValidPath(args[1], false)) {
		out << "path \'" + args[1] + "\' is not a valid path. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	return true;
}

bool Shell::JudgeFileModePara(const std::vector<std::string>& args, std::ostream& out)
{
	if (args.size() < 3) {
		out << "number of parameters is less than required. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (!isValidPath(args[1], false)) {
		out << "path \'" + args[1] + "\' is not a valid path. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (!isValidMode(args[2])) {
		out << "mode \'" + args[2] + "\' is not a valid mode. "
			<< "Use \'help -" + args[0] + "\' for more information." << "\n";
		return false;
	}
	return true;
}

bool Shell::JudgeFd(const std::vector<std::string>& args, std::ostream& out)
{
	if (args.size() < 2) {
		out << "number of parameters is less than required. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (!Utility::isDigit(args[1])) {
		out << "fd \'" + args[1] + "\' is not a number. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (stoi(args[1])<0) {
		out << "fd \'" + args[1] + "\' less than 0. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	return true;
}

bool Shell::JudgeSeekPara(const std::vector<std::string>& args, std::ostream& out)
{
	if (args.size() < 4) {
		out << "number of parameters is less than required. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (!Utility::isDigit(args[1])) {
		out << "fd \'" + args[1] + "\' is not a number. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (stoi(args[1]) < 0) {
		out << "fd \'" + args[1] + "\' less than 0. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
	}
	if (!Utility::isDigit(args[2])) {
		out << "offset \'" + args[1] + "\' is not a number. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (!Utility::isDigit(args[3])) {
		out << "origin \'" + args[3] + "\' is not a number. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (stoi(args[3])>2|| stoi(args[3])<0) {
		out << "origin \'" + args[3] + "\' is not in range 0,1,2. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	return true;
}

bool Shell::JudgeRdWtPara(const std::vector<std::string>& args, std::ostream& out)
{
	if (args.size() < 3 || args.size() < 4 && (args[0] == "fwrite" || args[2] == "-o"))
	{
		out << "number of parameters is less than required. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (!Utility::isDigit(args[1]))
	{
		out << "fd \'" + args[1] + "\' is not a number. "
			<< "Use \'help -" + args[0] + "\' for more information." << "\n";
		return false;
	}
	if (std::stoi(args[1]) < 0)
	{
		out << "fd should not less than 0. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}

	if (args[0] == "fwrite")
	{
		if (FILE* file = fopen(args[2].c_str(), "r")) {
			fclose(file);
		}
		else {
			out << "cannot find file \'" + args[2] + "\'. "
				<< "Use \'help -" + args[0] + "\' for more information" << "\n";
			return false;
		}
	}

	std::string size = (args[0] == "fwrite") ? args[3] :
		(args[0] == "fread" && args[2] == "-o") ? args[4] : args[2];
	if (!Utility::isDigit(size)) {
		out << "size \'" + size + "\' is not a number. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}
	if (std::stoi(size) < 0)
	{
		out << "size \'" + size + "\' should not less than 0. "
			<< "Use \'help -" + args[0] + "\' for more information" << "\n";
		return false;
	}

	return true;
}

bool Shell::isValidPath(const std::string path, bool isDir)
{
	int pos = 0; /* 上一个'/'的下标 */
	for (int i = 0; i < path.size(); i++) {
		if (path[i] == '/')
		{
			/* 目录名过长 */
			if (i - pos > DirectoryEntry::DIRSIZ) {
				return false;
			}
			/* 目录名中有'.'，但不是'..'*/
			if (isDir && path.substr(pos, i - pos).find(".") < path.size()
				&& path.substr(pos, i - pos).find("..") > path.size()) {
				return false;
			}
			pos = i;
		}
	}
	if (path.size() - pos > DirectoryEntry::DIRSIZ)
		return false;
	if (isDir && path.substr(pos, path.size() - pos).find(".") < path.size()
		&& path.substr(pos, path.size() - pos).find("..") > path.size()) {
		return false;
	}

	std::regex r("^((\/)?(.|..|[a-zA-Z0-9]+))+$");
	return std::regex_match(path, r);
}

bool Shell::isValidMode(const std::string mode)
{
	if (mode[0] != '-' || mode.size() > 3)
		return false;
	for (int i = 1; i < mode.size(); i++) {
		if (tolower(mode[i]) != 'r' && tolower(mode[i]) != 'w')
			return false;
	}
	return true;
}

void Shell::Build(std::ostream& out)
{
	std::vector<std::string> buildCmd =
	{
		"fformat",
		"mkdir bin",
		"mkdir etc",
		"mkdir home",
		"mkdir dev",
		"ls",
		"cd home",
		"mkdir texts",
		"mkdir reports",
		"mkdir photos",
		"ls",
		"cd texts",
		"fcreat ReadMe.txt -rw",
		"fopen ReadMe.txt -rw",
		"fseek 8 0 0",
		"fwrite 8 ../ReadMe.txt 1000",
		"fclose 8",
		"cd ..",
		"cd reports",
		"fcreat report.pdf -rw",
		"fopen report.pdf -rw",
		"fseek 9 0 0",
		"fwrite 9 ../report.pdf 8000",
		"fclose 9",
		"cd ..",
		"cd photos",
		"fcreat star.png -rw",
		"fopen star.png -rw",
		"fseek 10 0 0",
		"fwrite 10 ../star.png 4000",
		"fclose 9",
		"cd /",
	};

	out << "build start" << "\n";
	/* 依次执行各个指令 */
	for (int i = 0; i < buildCmd.size(); i++)
	{
		std::istringstream in(buildCmd[i]);/* 字符串转输入流 */
		std::vector<std::string>args;
		if (GetCommand(in, args))
		{
			out << "$root" << Kernel::Instance().GetCurDir() << ">"
				<< buildCmd[i] << "\n";
			if (args[0] == "fformat") 
				Kernel::Instance().Format();
			else if (args[0] == "cd")
				Kernel::Instance().ChangeDir(args[1]);
			else if (args[0] == "ls")
				out << Kernel::Instance().Ls();
			else if (args[0] == "mkdir")
				Kernel::Instance().MakeDir(args[1]);
			else if (args[0] == "fcreat")
				Kernel::Instance().CreateFile(args[1], args[2]);
			else if (args[0] == "fopen") {
				if (Kernel::Instance().OpenFile(args[1], args[2]))
				{
					out << "open success, return fd="
						<< Kernel::Instance().GetUser().u_ar0[User::EAX] << "\n";
				}
				else {
					out << "build error.\n";
					Kernel::Instance().OutputError(out);
					return;
				}
			}
			else if (args[0] == "fclose")
				Kernel::Instance().CloseFile(std::stoi(args[1]));
			else if (args[0] == "fread") {
				std::string filepath = args[2] == "-o" ? args[3] : "";
				int size = args[2] == "-o" ? std::stoi(args[4]) : std::stoi(args[2]);
				/* 正确处理在Read函数内部解决 */
				Kernel::Instance().Read(std::stoi(args[1]), size, filepath);
			}
			else if (args[0] == "fwrite") {
				if (Kernel::Instance().Write(std::stoi(args[1]), args[2], std::stoi(args[3])))
				{
					out << "write " << Kernel::Instance().GetUser().u_ar0[User::EAX]
						<< " bytes successfully. \n";
				}
				else {
					out << "build error.\n";
					Kernel::Instance().OutputError(out);
					return;
				}
			}
			else if (args[0] == "fseek")
				Kernel::Instance().SeekFile(std::stoi(args[1]), std::stoi(args[2]), std::stoi(args[3]));
		};
	}
	out << "build success.\n";
}
