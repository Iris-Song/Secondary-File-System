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

/*作用：进程打开文件描述符表中找的空闲项  之 下标  写入 u_ar0[EAX]*/
File* OpenFileTable::FAlloc() 
{
    int fd;
	User& u = Kernel::Instance().GetUser();

    /* 在进程打开文件描述符表中获取一个空闲项 */
	fd = u.u_ofiles.AllocFreeSlot();

	if (fd < 0)   /* 如果寻找空闲项失败 */
		return NULL;
	
	for (int i = 0; i < OpenFileTable::NFILE; i++) 
    {
        /* f_count==0表示该项空闲 */
		if (this->m_File[i].f_count == 0) 
        {
            /* 建立描述符和File结构的勾连关系 */
			u.u_ofiles.SetF(fd, &this->m_File[i]);
            /* 增加对file结构的引用计数 */
            this->m_File[i].f_count++;
            /* 清空文件读、写位置 */
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
         * 如果当前进程是最后一个引用该文件的进程，
         */
        g_InodeTable.IPut(pFile->f_inode);
    }

    /* 引用当前File的进程数减1 */
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

    /* 检查外存Inode是否有内存拷贝 */
    int index = IsLoaded(inumber);
    if (index >= 0) /* 找到内存拷贝 */
    {
        pInode = m_Inode + index;
        /*
         * 程序执行到这里表示：内存Inode高速缓存中找到相应内存Inode，
         * 增加其引用计数，返回之
         */
        ++pInode->i_count;
        return pInode;
    }
    else   /* 没有Inode的内存拷贝，则分配一个空闲内存Inode */
    {
        pInode = GetFreeInode();
        /* 若内存Inode表已满，分配空闲Inode失败 */
        if (NULL == pInode) 
        {
            Kernel::Instance().GetUser().u_error = User::U_ENFILE;
            return NULL;
        }
        else	/* 分配空闲Inode成功，将外存Inode读入新分配的内存Inode */
        {
            /* 设置新的外存Inode编号，增加引用计数，对索引节点上锁 */
            pInode->i_number = inumber;
            pInode->i_count++;

            BufferManager& bm = g_BufferManager;
            /* 将该外存Inode读入缓冲区 */
            Buf* pBuf = bm.Bread(FileSystem::INODE_ZONE_START_SECTOR + inumber / FileSystem::INODE_NUMBER_PER_SECTOR);
            /* 将缓冲区中的外存Inode信息拷贝到新分配的内存Inode中 */
            pInode->ICopy(pBuf, inumber);
            /* 释放缓存 */
            bm.Brelse(pBuf);
            return pInode;
        }
    }
}

/* close文件时会调用Iput
 * 主要做的操作：内存i节点计数 i_count--；若为0，释放内存 i节点、若有改动写回磁盘
 * 搜索文件途径的所有目录文件，搜索经过后都会Iput其内存i节点。路径名的倒数第2个路径分量一定是个
 * 目录文件，如果是在其中创建新文件、或是删除一个已有文件；再如果是在其中创建删除子目录。那么
 * 必须将这个目录文件所对应的内存 i节点写回磁盘。
 * 这个目录文件无论是否经历过更改，我们必须将它的i节点写回磁盘。
 */
void InodeTable::IPut(Inode* pInode) 
{
    /* 如果是根目录节点 不会释放*/
    if (Kernel::Instance().GetFileManager().rootDirInode == pInode)
    {
        return;
    }
        
    /* 当前进程为引用该内存INode的唯一进程，且准备释放该内存INode */
    if (pInode->i_count == 1)
    {
        /* 该文件已经没有目录路径指向它 */
        if (pInode->i_nlink <= 0) 
        {
            /* 释放该文件占据的数据盘块 */
            pInode->ITrunc();
            pInode->i_mode = 0;
            /* 释放对应的外存INode */
            this->m_FileSystem->IFree(pInode->i_number);
        }

        /* 更新外存INode信息 */
        pInode->IUpdate((int)Utility::Time(NULL));

        /* 清除内存INode的所有标志位 */
        pInode->i_flag = 0;
        /* 这是内存inode空闲的标志之一，另一个是i_count == 0 */
        pInode->i_number = -1;
    }

    /* 减少内存Inode的引用计数，唤醒等待进程 */
    pInode->i_count--;
}

/* 将所有被修改过的内存INode更新到对应外存INode中 */
void InodeTable::UpdateInodeTable()
{
    for (int i = 0; i < InodeTable::NINODE; i++)
    {
        /*
         * 如果Inode对象没有被上锁，即当前未被其它进程使用，可以同步到外存Inode；
         * 并且count不等于0，count == 0意味着该内存Inode未被任何打开文件引用，无需同步。
         */
        if (this->m_Inode[i].i_count)
        {
            /* 将内存Inode上锁后同步到外存Inode */
            this->m_Inode[i].IUpdate((int)Utility::Time(NULL));
        }
    }
}


int InodeTable::IsLoaded(int inumber) 
{
    /* 寻找指定外存Inode的内存拷贝 */
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
        /* 如果该内存Inode引用计数为零，则该Inode表示空闲 */
        if (this->m_Inode[i].i_count == 0) 
        {
            return m_Inode + i;
        }
    }
    return NULL;	/* 寻找失败 */
}

void InodeTable::Format()
{
    Inode emptyINode;
    for (int i = 0; i < InodeTable::NINODE; ++i) {
        Utility::MemCopy(m_Inode + i, &emptyINode, sizeof(Inode));
    }
}