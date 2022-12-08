#include "Kernel.h"
#include "Utility.h"
#include<fstream>

Kernel Kernel::instance;

/*
 * �豸�������ٻ������ȫ��manager
 */
DiskDriver g_DiskDriver;
BufferManager g_BufferManager;

/*
 * �ļ�ϵͳ���ȫ��manager
 */
FileSystem g_FileSystem;
InodeTable g_InodeTable;
FileManager g_FileManager;
OpenFileTable g_OpenFileTable;

/*
 * �û����ȫ��
 */
User g_User;

Kernel::Kernel()
{
}

Kernel::~Kernel()
{
}

Kernel& Kernel::Instance()
{
	return Kernel::instance;
}

void Kernel::InitBuffer()
{
	this->m_DiskDriver = &g_DiskDriver;
	this->m_BufferManager = &g_BufferManager;

	this->GetBufferManager().Initialize();
}

void Kernel::InitFileSystem()
{
	this->m_FileSystem = &g_FileSystem;
	this->m_FileManager = &g_FileManager;

	this->GetFileSystem().Initialize();
	this->GetFileManager().Initialize();
}

void Kernel::InitUser()
{
	this->m_User = &g_User;
	this->GetUser().Initialize();
}

void Kernel::FormatBuffer()
{
	this->GetBufferManager().Format();
}

void Kernel::FormatFileSystem()
{
	this->GetFileManager().Format();
	this->GetFileSystem().Format();
}

void Kernel::Initialize()
{
	InitBuffer();
	InitFileSystem();
	InitUser();
}


BufferManager& Kernel::GetBufferManager()
{
	return *(this->m_BufferManager);
}

DiskDriver& Kernel::GetDiskDriver()
{
	return *(this->m_DiskDriver);
}

FileSystem& Kernel::GetFileSystem()
{
	return *(this->m_FileSystem);
}

FileManager& Kernel::GetFileManager()
{
	return *(this->m_FileManager);
}

User& Kernel::GetUser()
{
	return *(this->m_User);
}

void Kernel::Format()
{
	//this->FormatBuffer();
	this->FormatFileSystem();
	this->Initialize();
}

std::string Kernel::Ls()
{
	/* �������error */
	return GetUser().Ls();
}

bool Kernel::CreateFile(const std::string path, const std::string mode)
{
	std::string smpPath = Utility::SimplifyPath(path);
	GetUser().SetDirp(smpPath);
	GetUser().u_arg[1] = Utility::GetInodeMode(mode);
	GetFileManager().Creat();

	if (GetUser().JudgeError())
		return false;
	return true;
}

bool Kernel::DeleteFile(const std::string path)
{
	std::string smpPath = Utility::SimplifyPath(path);
	GetUser().SetDirp(smpPath);
	GetFileManager().UnLink();

	if (GetUser().JudgeError())
		return false;
	return true;
}

bool Kernel::OpenFile(const std::string path, const std::string mode)
{
	std::string smpPath = Utility::SimplifyPath(path);
	GetUser().SetDirp(smpPath);
	GetUser().u_arg[1] = Utility::GetFileMode(mode);
	GetFileManager().Open();

	if (GetUser().JudgeError())
		return false;
	return true;
}

bool Kernel::CloseFile(const int fd)
{
	GetUser().u_arg[0] = fd;
	GetFileManager().Close();

	if (GetUser().JudgeError())
		return false;
	return true;
}

void Kernel::SeekFile(const int fd, const int offset, const int org)
{
	GetUser().u_arg[0] = fd;
	GetUser().u_arg[1] = offset;
	GetUser().u_arg[2] = org;
	GetFileManager().Seek();

    /* �ɹ�����Ҫ��ȷ��ʾ */
}

bool Kernel::MakeDir(const std::string path)
{
	std::string smpPath = Utility::SimplifyPath(path);

	GetUser().SetDirp(smpPath);
	GetUser().u_arg[1] |= Inode::IFDIR;
	GetFileManager().Creat();
	
	if (GetUser().JudgeError())
		return false;
	return true;
}

void Kernel::ChangeDir(const std::string path)
{
	std::string smpPath = Utility::SimplifyPath(path);
	GetUser().SetDirp(smpPath);
	GetFileManager().ChDir();
}

void Kernel::Read(const int fd, int size, const std::string file = "")
{
	char* Buffer = new char[size];

	GetUser().u_arg[0] = fd;
	GetUser().u_arg[1] = (long)Buffer;
	GetUser().u_arg[2] = size;
	GetFileManager().Read();

	if (!GetUser().JudgeError()) /* ��ȷִ���޴��� */
	{
		std::cout << "read " << Kernel::Instance().GetUser().u_ar0[User::EAX] << " bytes success  \n";

		if (file.empty()) /* �����cmd */
		{
			for (unsigned int i = 0; i < Kernel::Instance().GetUser().u_ar0[User::EAX]; ++i)
				std::cout << (char)Buffer[i];
			std::cout << " \n";
		}
		else /* �����ĳ�ļ� */
		{
			std::fstream fout(file, std::ios::out | std::ios::binary);
			if (!fout) {
				std::cout << "file " << file << " open failed ! \n";
			}
			else
			{
				fout.write(Buffer, GetUser().u_ar0[User::EAX]);
				fout.close();
				std::cout << "read to " << file << " done ! \n";
			}
		}
	}

	delete[] Buffer;
	return;
}

bool Kernel::Write(const int fd, const std::string file, int size)
{
	std::fstream fin(file, std::ios::in | std::ios::binary);
	if (!fin) 
		return false;
	
	char* Buffer = new char[size];
	fin.read(Buffer, size);
	fin.close();

	GetUser().u_arg[0] = fd;
	GetUser().u_arg[1] = (long)Buffer;
	GetUser().u_arg[2] = size;
	GetFileManager().Write();

	delete[]Buffer;

	if (GetUser().JudgeError())
		return false;
	return true;
}

bool Kernel::OutputError(std::ostream& out) 
{
	bool isErr = GetUser().JudgeError();
	if (isErr) /* ����д��� */
	{
		out << "error code: " << GetUser().u_error<<"\n";
		out << GetUser().ErrorStr()<<"\n";
		/* ��������� */
		GetUser().u_error = GetUser().U_NOERROR;
	}
	return isErr;
}

std::string Kernel::GetCurDir() 
{
	return GetUser().u_curdir;
}

