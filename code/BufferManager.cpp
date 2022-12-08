#include "BufferManager.h"
#include "Kernel.h"
#include "Utility.h"

extern DiskDriver g_DiskDriver;

/*
 *	Buffer只用到了两个标志，B_DONE和B_DELWRI，分别表示已经完成IO和延迟写的标志。
 *	空闲Buffer无任何标志
*/
BufferManager::BufferManager()
{
	//nothing to do here
}

BufferManager::~BufferManager()
{
	//nothing to do here
}

void BufferManager::Initialize()
{
	for (int i = 0; i < NBUF; ++i) {
		if (i) {
			m_Buf[i].b_forw = m_Buf + i - 1;
		}
		else {
			m_Buf[i].b_forw = &bFreeList;
			bFreeList.b_back = m_Buf + i;
		}

		if (i + 1 < NBUF) {
			m_Buf[i].b_back = m_Buf + i + 1;
		}
		else {
			m_Buf[i].b_back = &bFreeList;
			bFreeList.b_forw = m_Buf + i;
		}
		m_Buf[i].b_addr = Buffer[i];
	}
}

void BufferManager::Format()
{
	Buf emptyBuf;
	for (int i = 0; i < NBUF; ++i)
	{
		Utility::MemCopy(m_Buf + i, &emptyBuf, sizeof(Buf));
	}
	Initialize();
}


/*
 * 从freeBufList头部取出
 */
void BufferManager::NotAvail(Buf* pb)
{
	if (pb->b_back == NULL) //已经在链表尾
		return;

	pb->b_forw->b_back = pb->b_back;
	pb->b_back->b_forw = pb->b_forw;
	pb->b_back = NULL;
	pb->b_forw = NULL;
}


Buf* BufferManager::GetBlk(int blkno)
{
	Buf* bp;
	if (bMap.find(blkno) != bMap.end())  /* 在Bmap中查找到 */
	{
		bp = bMap[blkno];
		NotAvail(bp);  /* 取出找到的bp */
		return bp;
	}

	/* 在Bmap中没查找到，取出空的缓存 */
	bp = bFreeList.b_back;
	if (bp == &bFreeList)  /* 自由队列为空，无空缓存 */
	{
		/* 这种情况实际不会出现，仅测试 */
		Utility::Panic("No available buffer", Kernel::Instance().GetUser().U_EIO);
		return NULL;
	}

	NotAvail(bp);
	bMap.erase(bp->b_blkno);
	if (bp->b_flags & Buf::B_DELWRI) /* 如果有延迟写标志，写入磁盘 */
		Kernel::Instance().GetDiskDriver().Write(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);

	/* 清除延迟写和I/O操作结束标志 */
	bp->b_flags &= ~(Buf::B_DELWRI | Buf::B_DONE);
	bp->b_blkno = blkno;
	bMap[blkno] = bp;
	return bp;
}

/* 释放缓存控制块buf */
void BufferManager::Brelse(Buf* bp)
{
	if (bp->b_back != NULL) {
		return;
	}
	//插入自由队列
	bp->b_forw = bFreeList.b_forw;
	bp->b_back = &bFreeList;
	bFreeList.b_forw->b_back = bp;
	bFreeList.b_forw = bp;
}


Buf* BufferManager::Bread(int blkno)
{
	Buf* bp;
	/* 根据字符块号申请缓存 */
	bp = this->GetBlk(blkno);
	/* 如果找到所需缓存，即B_DONE已设置，就不需进行I/O操作 */
	if (bp->b_flags & (Buf::B_DONE | Buf::B_DELWRI)) 
	{
		return bp;
	}
	/* 没有找到相应缓存，构成I/O读请求块 */
	bp->b_flags |= Buf::B_DONE;
	bp->b_wcount = BufferManager::BUFFER_SIZE;
	Kernel::Instance().GetDiskDriver().Read(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);
	return bp;
}

void BufferManager::Bwrite(Buf* bp)
{
	bp->b_flags &= ~(Buf::B_DELWRI);
	Kernel::Instance().GetDiskDriver().Write(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);
	bp->b_flags |= (Buf::B_DONE);
	this->Brelse(bp);
}

void BufferManager::Bdwrite(Buf* bp)
{
	bp->b_flags |= (Buf::B_DELWRI | Buf::B_DONE);
	this->Brelse(bp);
	return;
}

void BufferManager::ClrBuf(Buf* bp)
{
	int* pInt = (int*)bp->b_addr;

	/* 将缓冲区中数据清零 */
	for (unsigned int i = 0; i < BufferManager::BUFFER_SIZE / sizeof(int); i++)
	{
		pInt[i] = 0;
	}
	return;
}


void BufferManager::Bflush()
{
	Buf* pb = NULL;
	for (int i = 0; i < NBUF; ++i) {
		pb = m_Buf + i;
		if ((pb->b_flags & Buf::B_DELWRI))
		{
			pb->b_flags &= ~(Buf::B_DELWRI);
			Kernel::Instance().GetDiskDriver().Write(pb->b_addr, BUFFER_SIZE, pb->b_blkno * BUFFER_SIZE);
			pb->b_flags |= (Buf::B_DONE);
		}
	}
}

void BufferManager::GetError(Buf* bp)
{
	User& u = Kernel::Instance().GetUser();

	if (bp->b_flags & Buf::B_ERROR)
	{
		u.u_error = User::U_EIO;
	}
	return;
}