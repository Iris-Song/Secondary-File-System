#include "FileSystem.h"
#include "Kernel.h"
#include "Utility.h"

extern DiskDriver g_DiskDriver;
extern BufferManager g_BufferManager;
extern InodeTable g_InodeTable;

/*==============================class SuperBlock===================================*/
/* ϵͳȫ�ֳ�����SuperBlock���� */
SuperBlock g_spb;

SuperBlock::SuperBlock()
{
    //nothing to do here
}

SuperBlock::~SuperBlock()
{
    //nothing to do here
}

void SuperBlock::Format() 
{
    s_isize = FileSystem::INODE_ZONE_SIZE;
    s_fsize = FileSystem::DISK_SIZE;
    s_nfree = 0;
    s_free[0] = -1;
    s_ninode = 0;
    s_flock = 0;
    s_ilock = 0;
    s_fmod = 0;
    s_ronly = 0;
}

/*==============================class FileSystem===================================*/
FileSystem::FileSystem()
{
    //nothing to do here
}

FileSystem::~FileSystem() 
{
    Update();
}

void FileSystem::Initialize() 
{
    m_DiskDriver = &g_DiskDriver;
    m_spb = &g_spb;
    m_BufferManager = &g_BufferManager;

    if (!m_DiskDriver->Exists()) 
        Format();
    else 
        LoadSuperBlock();
    
}

/* ��ʽ�������ļ�ϵͳ */
void FileSystem::Format() 
{
    this->m_spb->Format();
    m_DiskDriver->Mount();

    /* ---------------SuperBlock����ʽ��------------------ */
    m_DiskDriver->Write(m_spb, sizeof(SuperBlock), 0);

    /* --------------DiskInode����ʽ��-------------------- */
    DiskInode emptyDiskInode, rootDiskInode;
    /* ��Ŀ¼DiskInode��ʼ�� */
    rootDiskInode.d_mode |= Inode::IALLOC | Inode::IFDIR;
    rootDiskInode.d_nlink = 1;
    m_DiskDriver->Write(&rootDiskInode, sizeof(rootDiskInode));
    
    /* �ӵ�1��DiskInode��ʼ������0���̶����ڸ�Ŀ¼"/"�����ɸı� */
    for (int i = 1; i < FileSystem::INODE_NUMBER; ++i) 
    {
        if (m_spb->s_ninode < SuperBlock::NINODE)
        {
            m_spb->s_inode[m_spb->s_ninode++] = i;
        }
        m_DiskDriver->Write(&emptyDiskInode, sizeof(emptyDiskInode));
    }

    /* ----------------���ݿ�����ʽ��-------------------- */
    char emptyBlock[BLOCK_SIZE];
    Utility::MemSet(emptyBlock, 0, BLOCK_SIZE);

    for (int i = 0; i < FileSystem::DATA_ZONE_SIZE; ++i) 
    {
        if (m_spb->s_nfree >= SuperBlock::NFREE) 
        {
            Utility::MemCopy(emptyBlock, &m_spb->s_nfree, sizeof(int) + sizeof(m_spb->s_free));
            m_DiskDriver->Write(&emptyBlock, BLOCK_SIZE);
            m_spb->s_nfree = 0;
        }
        else 
        {
            m_DiskDriver->Write(emptyBlock, BLOCK_SIZE);
        }
        m_spb->s_free[m_spb->s_nfree++] = i + DATA_ZONE_START_SECTOR;
    }

    /* ���ø�ʽ��ʱ����ٴ�д��superblock */
    Utility::Time((time_t*)&m_spb->s_time);
    m_DiskDriver->Write(m_spb, sizeof(SuperBlock), 0);

}

/* ϵͳ��ʼ��ʱ����SuperBlock */
void FileSystem::LoadSuperBlock() 
{
    m_DiskDriver->Read(m_spb, sizeof(SuperBlock), SUPER_BLOCK_SECTOR_NUMBER*BLOCK_SIZE);
}

void FileSystem::Update() 
{
	Buf* pBuf;
	m_spb->s_fmod = 0;
    m_spb->s_time = (int)Utility::Time(NULL);

	for (int j = 0; j < 2; j++)
    {
        /* ��һ��pָ��SuperBlock�ĵ�0�ֽڣ��ڶ���pָ���512�ֽ� */
		int* p = (int *)m_spb + j * 128;

        /* ��Ҫд�뵽�豸dev�ϵ�SUPER_BLOCK_SECTOR_NUMBER + j������ȥ */
		pBuf = this->m_BufferManager->GetBlk(FileSystem::SUPER_BLOCK_SECTOR_NUMBER + j);
        
        /* ��SuperBlock�е�0 - 511�ֽ�д�뻺���� */
        Utility::MemCopy(pBuf->b_addr, p, BLOCK_SIZE);
        
        /* ���������е�����д�������� */
        this->m_BufferManager->Bwrite(pBuf);
	}
    /* ͬ���޸Ĺ����ڴ�Inode����Ӧ���Inode */
	g_InodeTable.UpdateInodeTable();
    /* ���ӳ�д�Ļ����д�������� */
	this->m_BufferManager->Bflush();
}


Inode* FileSystem::IAlloc() 
{
    Buf* pBuf;
    Inode* pInode;
    User& u = Kernel::Instance().GetUser();
    int ino;	/* ���䵽�Ŀ������Inode��� */

    /*
     * SuperBlockֱ�ӹ����Ŀ���Inode�������ѿգ�
     * ���뵽��������������Inode���ȶ�inode�б�������
     */
    if (m_spb->s_ninode <= 0) 
    {
        /* ���Inode��Ŵ�0��ʼ���ⲻͬ��Unix V6�����Inode��1��ʼ��� */
        ino = -1;
        for (int i = 0; i < m_spb->s_isize; i++) 
        {
            pBuf = this->m_BufferManager->Bread(FileSystem::INODE_ZONE_START_SECTOR + i);
            
            /* ��ȡ��������ַ */
            int* p = (int*)pBuf->b_addr;
            
            /* ���û�������ÿ�����Inode��i_mode != 0����ʾ�Ѿ���ռ�� */
            for (int j = 0; j < FileSystem::INODE_NUMBER_PER_SECTOR; j++) 
            {
                ino++;

                int mode = *(p + j * sizeof(DiskInode) / sizeof(int));
                
                /* �����Inode�ѱ�ռ�ã����ܼ������Inode������ */
                if (mode != 0)
                {
                    continue;
                }

                /*
                 * ������inode��i_mode==0����ʱ������ȷ��
                 * ��inode�ǿ��еģ���Ϊ�п������ڴ�inodeû��д��
                 * ������,����Ҫ���������ڴ�inode���Ƿ�����Ӧ����
                 */
                if (g_InodeTable.IsLoaded(ino) == -1) 
                {
                    /* �����Inodeû�ж�Ӧ���ڴ濽��������������Inode������ */
                    m_spb->s_inode[m_spb->s_ninode++] = ino;
                    
                    /* ��������������Ѿ�װ�����򲻼������� */
                    if (m_spb->s_ninode >= SuperBlock::NINODE) 
                    {
                        break;
                    }
                }
            }

            /* �����Ѷ��굱ǰ���̿飬�ͷ���Ӧ�Ļ��� */
            this->m_BufferManager->Brelse(pBuf);

            /* ��������������Ѿ�װ�����򲻼������� */
            if (m_spb->s_ninode >= SuperBlock::NINODE) 
                break;
            
        }

        /* ����ڴ�����û���������κο������Inode������NULL */
        if (m_spb->s_ninode <= 0) {
            u.u_error = User::U_ENOSPC;
            return NULL;
        }
    }

    ino = m_spb->s_inode[--m_spb->s_ninode];
    pInode = g_InodeTable.IGet(ino);
    if (NULL == pInode) {
        u.u_error = User::U_ENOSPC;
        return NULL;
    }

    pInode->Clean();
    m_spb->s_fmod = 1;
    return pInode;
}

void FileSystem::IFree(int number) 
{
    /*
     * ���������ֱ�ӹ����Ŀ������Inode����100����
     * ͬ�����ͷŵ����Inodeɢ���ڴ���Inode���С�
     */
    if (m_spb->s_ninode >= SuperBlock::NINODE) 
        return;
    
    m_spb->s_inode[m_spb->s_ninode++] = number;
    
    /* ����SuperBlock���޸ı�־ */
    m_spb->s_fmod = 1;
}

/* �ڴ洢�豸�Ϸ�����д��̿� */
Buf* FileSystem::Alloc() 
{
	int blkno;	/* ���䵽�Ŀ��д��̿��� */
	Buf* pBuf;
	User& u = Kernel::Instance().GetUser();

    /* ����������ջ������ȡ���д��̿��� *///!!
	blkno = m_spb->s_free[--m_spb->s_nfree];

    /*
     * ����ȡ���̿���Ϊ�㣬���ʾ�ѷ��価���еĿ��д��̿顣
     * ���߷��䵽�Ŀ��д��̿��Ų����������̿�������(��BadBlock()���)��
     * ����ζ�ŷ�����д��̿����ʧ�ܡ�
     */
	if (blkno <= 0) 
    {
		m_spb->s_nfree = 0;
		u.u_error = User::U_ENOSPC;
		return NULL;
	}

    /*
     * ջ�ѿգ��·��䵽���д��̿��м�¼����һ����д��̿�ı��,
     * ����һ����д��̿�ı�Ŷ���SuperBlock�Ŀ��д��̿�������s_free[100]�С�
     */
    if (m_spb->s_nfree <= 0) 
    {
        /* ����ÿ��д��̿� */
		pBuf = this->m_BufferManager->Bread(blkno);
		
        /* �Ӹô��̿��0�ֽڿ�ʼ��¼����ռ��4(s_nfree)+400(s_free[100])���ֽ� */
        int* p = (int *)pBuf->b_addr;
		
        /* ���ȶ��������̿���s_nfree */
        m_spb->s_nfree = *p++;

        /* ��ȡ�����к���λ�õ����ݣ�д�뵽SuperBlock�����̿�������s_free[100]�� */
        Utility::MemCopy(m_spb->s_free, p, sizeof(m_spb->s_free));//!
		
        this->m_BufferManager->Brelse(pBuf);
	}
    /* ��ͨ����³ɹ����䵽һ���д��̿� */
	pBuf = this->m_BufferManager->GetBlk(blkno);
	if (pBuf) 
		this->m_BufferManager->ClrBuf(pBuf);   /* ��ջ����е����� */
	
	m_spb->s_fmod = 1;	/* ����SuperBlock���޸ı�־ */
	
    return pBuf;
}


/* �ͷŴ洢�豸dev�ϱ��Ϊblkno�Ĵ��̿� */
void FileSystem::Free(int blkno)
{
	Buf* pBuf;
	User& u = Kernel::Instance().GetUser();

	if (m_spb->s_nfree >=SuperBlock::NFREE )
    {
        /*
         * ʹ�õ�ǰFree()������Ҫ�ͷŵĴ��̿飬���ǰһ��100������
         * ���̿��������
         */
		pBuf = this->m_BufferManager->GetBlk(blkno);

        /* �Ӹô��̿��0�ֽڿ�ʼ��¼����ռ��4(s_nfree)+400(s_free[100])���ֽ� */
        int *p = (int*)pBuf->b_addr;

        /* ����д������̿��������˵�һ��Ϊ99�飬����ÿ�鶼��100�� */
		*p++ = m_spb->s_nfree;
        /* ��SuperBlock�Ŀ����̿�������s_free[100]д�뻺���к���λ�� */
		Utility::MemCopy(p, m_spb->s_free, sizeof(int)*SuperBlock::NFREE);
		
        m_spb->s_nfree = 0;
        /* ����ſ����̿��������ġ���ǰ�ͷ��̿顱д����̣���ʵ���˿����̿��¼�����̿�ŵ�Ŀ�� */
		this->m_BufferManager->Bwrite(pBuf);
	}

	m_spb->s_free[m_spb->s_nfree++] = blkno;	/* SuperBlock�м�¼�µ�ǰ�ͷ��̿�� */
	m_spb->s_fmod = 1;
}