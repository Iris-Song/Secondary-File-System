#include "Utility.h"
#include "File.h"
#include "Kernel.h"


/*==============================class File===================================*/
File::File() 
{
	f_flag = 0;
	f_count = 0;
	f_offset = 0;
	f_inode = NULL;
}

File::~File() 
{
	//nothing to do here
}

/*==============================class OpenFiles===================================*/
OpenFiles::OpenFiles() 
{
	
}

OpenFiles::~OpenFiles() 
{
}

void OpenFiles::Initialize() 
{
	Utility::MemSet(ProcessOpenFileTable, NULL, sizeof(ProcessOpenFileTable));
}

int OpenFiles::AllocFreeSlot() 
{
    User& u = Kernel::Instance().GetUser();

    for (int i = 0; i < OpenFiles::NOFILES; i++) 
	{
        /* ���̴��ļ������������ҵ�������򷵻�֮ */
        if (this->ProcessOpenFileTable[i] == NULL)
		{
			/* ���ú���ջ�ֳ��������е�EAX�Ĵ�����ֵ����ϵͳ���÷���ֵ */
            u.u_ar0[User::EAX] = i;
            return i;
        }
    }

    u.u_ar0[User::EAX] = -1;   /* Open1����Ҫһ����־�������ļ��ṹ����ʧ��ʱ�����Ի���ϵͳ��Դ*/
    u.u_error = User::U_EMFILE;
    return -1;
}

/* �����û�ϵͳ�����ṩ���ļ�����������fd���ҵ���Ӧ�Ĵ��ļ����ƿ�File�ṹ */
File* OpenFiles::GetF(int fd) 
{
	File* pFile;
	User& u = Kernel::Instance().GetUser();

	/* ������ļ���������ֵ�����˷�Χ */
	if (fd < 0 || fd >= OpenFiles::NOFILES) 
	{
		u.u_error = User::U_EBADF;
		return NULL;
	}

	pFile = this->ProcessOpenFileTable[fd];
	if (pFile == NULL) 
		u.u_error = User::U_EBADF;

	return pFile;	/* ��ʹpFile==NULLҲ���������ɵ���GetF�ĺ������жϷ���ֵ */
}

/* Ϊ�ѷ��䵽�Ŀ���������fd���ѷ���Ĵ��ļ����п���File������������ϵ */
void OpenFiles::SetF(int fd, File* pFile) 
{
	if (fd < 0 || fd >= OpenFiles::NOFILES) 
	{
		return;
	}
	/* ���̴��ļ�������ָ��ϵͳ���ļ�������Ӧ��File�ṹ */
	this->ProcessOpenFileTable[fd] = pFile;
}

/*==============================class IOParameter===================================*/
IOParameter::IOParameter()
{
	//nothing to do here
}

IOParameter::~IOParameter()
{
	//nothing to do here
}

void IOParameter::Initialize()
{
	this->m_Base = 0;
	this->m_Count = 0;
	this->m_Offset = 0;
}