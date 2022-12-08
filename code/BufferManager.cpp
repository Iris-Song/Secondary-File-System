#include "BufferManager.h"
#include "Kernel.h"
#include "Utility.h"

extern DiskDriver g_DiskDriver;

/*
 *	Bufferֻ�õ���������־��B_DONE��B_DELWRI���ֱ��ʾ�Ѿ����IO���ӳ�д�ı�־��
 *	����Buffer���κα�־
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
 * ��freeBufListͷ��ȡ��
 */
void BufferManager::NotAvail(Buf* pb)
{
	if (pb->b_back == NULL) //�Ѿ�������β
		return;

	pb->b_forw->b_back = pb->b_back;
	pb->b_back->b_forw = pb->b_forw;
	pb->b_back = NULL;
	pb->b_forw = NULL;
}


Buf* BufferManager::GetBlk(int blkno)
{
	Buf* bp;
	if (bMap.find(blkno) != bMap.end())  /* ��Bmap�в��ҵ� */
	{
		bp = bMap[blkno];
		NotAvail(bp);  /* ȡ���ҵ���bp */
		return bp;
	}

	/* ��Bmap��û���ҵ���ȡ���յĻ��� */
	bp = bFreeList.b_back;
	if (bp == &bFreeList)  /* ���ɶ���Ϊ�գ��޿ջ��� */
	{
		/* �������ʵ�ʲ�����֣������� */
		Utility::Panic("No available buffer", Kernel::Instance().GetUser().U_EIO);
		return NULL;
	}

	NotAvail(bp);
	bMap.erase(bp->b_blkno);
	if (bp->b_flags & Buf::B_DELWRI) /* ������ӳ�д��־��д����� */
		Kernel::Instance().GetDiskDriver().Write(bp->b_addr, BUFFER_SIZE, bp->b_blkno * BUFFER_SIZE);

	/* ����ӳ�д��I/O����������־ */
	bp->b_flags &= ~(Buf::B_DELWRI | Buf::B_DONE);
	bp->b_blkno = blkno;
	bMap[blkno] = bp;
	return bp;
}

/* �ͷŻ�����ƿ�buf */
void BufferManager::Brelse(Buf* bp)
{
	if (bp->b_back != NULL) {
		return;
	}
	//�������ɶ���
	bp->b_forw = bFreeList.b_forw;
	bp->b_back = &bFreeList;
	bFreeList.b_forw->b_back = bp;
	bFreeList.b_forw = bp;
}


Buf* BufferManager::Bread(int blkno)
{
	Buf* bp;
	/* �����ַ�������뻺�� */
	bp = this->GetBlk(blkno);
	/* ����ҵ����軺�棬��B_DONE�����ã��Ͳ������I/O���� */
	if (bp->b_flags & (Buf::B_DONE | Buf::B_DELWRI)) 
	{
		return bp;
	}
	/* û���ҵ���Ӧ���棬����I/O������� */
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

	/* ������������������ */
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