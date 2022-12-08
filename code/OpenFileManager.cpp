#include "Utility.h"
#include "OpenFileManager.h"
#include "Kernel.h"

extern BufferManager g_BufferManager;
extern FileSystem g_FileSystem;
extern InodeTable g_InodeTable;

/*==============================class OpenFileTable===================================*/

OpenFileTable::OpenFileTable()
{
    //nothing to do here
}

OpenFileTable::~OpenFileTable()
{
    //nothing to do here
}

void OpenFileTable::Format() 
{
    File emptyFile;
    for (int i = 0; i < OpenFileTable::NFILE; ++i) {
        Utility::MemCopy(m_File + i, &emptyFile, sizeof(File));
    }
}

/*���ã����̴��ļ������������ҵĿ�����  ֮ �±�  д�� u_ar0[EAX]*/
File* OpenFileTable::FAlloc() 
{
    int fd;
	User& u = Kernel::Instance().GetUser();

    /* �ڽ��̴��ļ����������л�ȡһ�������� */
	fd = u.u_ofiles.AllocFreeSlot();

	if (fd < 0)   /* ���Ѱ�ҿ�����ʧ�� */
		return NULL;
	
	for (int i = 0; i < OpenFileTable::NFILE; i++) 
    {
        /* f_count==0��ʾ������� */
		if (this->m_File[i].f_count == 0) 
        {
            /* ������������File�ṹ�Ĺ�����ϵ */
			u.u_ofiles.SetF(fd, &this->m_File[i]);
            /* ���Ӷ�file�ṹ�����ü��� */
            this->m_File[i].f_count++;
            /* ����ļ�����дλ�� */
            this->m_File[i].f_offset = 0;
			return (&this->m_File[i]);
		}
	}

	u.u_error = User::U_ENFILE;
	return NULL;
}

void OpenFileTable::CloseF(File* pFile) 
{
    if (pFile->f_count <= 1)
    {
        /*
         * �����ǰ���������һ�����ø��ļ��Ľ��̣�
         */
        g_InodeTable.IPut(pFile->f_inode);
    }

    /* ���õ�ǰFile�Ľ�������1 */
    pFile->f_count--;
}


/*==============================class InodeTable===================================*/

InodeTable::InodeTable() 
{
    m_FileSystem = &g_FileSystem;
}

InodeTable::~InodeTable()
{
    //nothing to do here
}

Inode* InodeTable::IGet(int inumber) 
{
    Inode* pInode;

    /* ������Inode�Ƿ����ڴ濽�� */
    int index = IsLoaded(inumber);
    if (index >= 0) /* �ҵ��ڴ濽�� */
    {
        pInode = m_Inode + index;
        /*
         * ����ִ�е������ʾ���ڴ�Inode���ٻ������ҵ���Ӧ�ڴ�Inode��
         * ���������ü���������֮
         */
        ++pInode->i_count;
        return pInode;
    }
    else   /* û��Inode���ڴ濽���������һ�������ڴ�Inode */
    {
        pInode = GetFreeInode();
        /* ���ڴ�Inode���������������Inodeʧ�� */
        if (NULL == pInode) 
        {
            Kernel::Instance().GetUser().u_error = User::U_ENFILE;
            return NULL;
        }
        else	/* �������Inode�ɹ��������Inode�����·�����ڴ�Inode */
        {
            /* �����µ����Inode��ţ��������ü������������ڵ����� */
            pInode->i_number = inumber;
            pInode->i_count++;

            BufferManager& bm = g_BufferManager;
            /* �������Inode���뻺���� */
            Buf* pBuf = bm.Bread(FileSystem::INODE_ZONE_START_SECTOR + inumber / FileSystem::INODE_NUMBER_PER_SECTOR);
            /* ���������е����Inode��Ϣ�������·�����ڴ�Inode�� */
            pInode->ICopy(pBuf, inumber);
            /* �ͷŻ��� */
            bm.Brelse(pBuf);
            return pInode;
        }
    }
}

/* close�ļ�ʱ�����Iput
 * ��Ҫ���Ĳ������ڴ�i�ڵ���� i_count--����Ϊ0���ͷ��ڴ� i�ڵ㡢���иĶ�д�ش���
 * �����ļ�;��������Ŀ¼�ļ������������󶼻�Iput���ڴ�i�ڵ㡣·�����ĵ�����2��·������һ���Ǹ�
 * Ŀ¼�ļ�������������д������ļ�������ɾ��һ�������ļ���������������д���ɾ����Ŀ¼����ô
 * ���뽫���Ŀ¼�ļ�����Ӧ���ڴ� i�ڵ�д�ش��̡�
 * ���Ŀ¼�ļ������Ƿ��������ģ����Ǳ��뽫����i�ڵ�д�ش��̡�
 */
void InodeTable::IPut(Inode* pInode) 
{
    /* ����Ǹ�Ŀ¼�ڵ� �����ͷ�*/
    if (Kernel::Instance().GetFileManager().rootDirInode == pInode)
    {
        return;
    }
        
    /* ��ǰ����Ϊ���ø��ڴ�INode��Ψһ���̣���׼���ͷŸ��ڴ�INode */
    if (pInode->i_count == 1)
    {
        /* ���ļ��Ѿ�û��Ŀ¼·��ָ���� */
        if (pInode->i_nlink <= 0) 
        {
            /* �ͷŸ��ļ�ռ�ݵ������̿� */
            pInode->ITrunc();
            pInode->i_mode = 0;
            /* �ͷŶ�Ӧ�����INode */
            this->m_FileSystem->IFree(pInode->i_number);
        }

        /* �������INode��Ϣ */
        pInode->IUpdate((int)Utility::Time(NULL));

        /* ����ڴ�INode�����б�־λ */
        pInode->i_flag = 0;
        /* �����ڴ�inode���еı�־֮һ����һ����i_count == 0 */
        pInode->i_number = -1;
    }

    /* �����ڴ�Inode�����ü��������ѵȴ����� */
    pInode->i_count--;
}

/* �����б��޸Ĺ����ڴ�INode���µ���Ӧ���INode�� */
void InodeTable::UpdateInodeTable()
{
    for (int i = 0; i < InodeTable::NINODE; i++)
    {
        /*
         * ���Inode����û�б�����������ǰδ����������ʹ�ã�����ͬ�������Inode��
         * ����count������0��count == 0��ζ�Ÿ��ڴ�Inodeδ���κδ��ļ����ã�����ͬ����
         */
        if (this->m_Inode[i].i_count)
        {
            /* ���ڴ�Inode������ͬ�������Inode */
            this->m_Inode[i].IUpdate((int)Utility::Time(NULL));
        }
    }
}


int InodeTable::IsLoaded(int inumber) 
{
    /* Ѱ��ָ�����Inode���ڴ濽�� */
    for (int i = 0; i < NINODE; ++i) 
    {
        if (m_Inode[i].i_number == inumber&&m_Inode[i].i_count) 
        {
            return i;
        }
    }
    return -1;
}

Inode* InodeTable::GetFreeInode()
{
    for (int i = 0; i < InodeTable::NINODE; i++) 
    {
        /* ������ڴ�Inode���ü���Ϊ�㣬���Inode��ʾ���� */
        if (this->m_Inode[i].i_count == 0) 
        {
            return m_Inode + i;
        }
    }
    return NULL;	/* Ѱ��ʧ�� */
}

void InodeTable::Format()
{
    Inode emptyINode;
    for (int i = 0; i < InodeTable::NINODE; ++i) {
        Utility::MemCopy(m_Inode + i, &emptyINode, sizeof(Inode));
    }
}