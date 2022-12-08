#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "Buf.h"
#include "DiskDriver.h"
#include <map>

class BufferManager 
{
public:
    /* static const member */
    static const int NBUF = 15;			/* ������ƿ顢������������ */
    static const int BUFFER_SIZE = 512;	/* ��������С�� ���ֽ�Ϊ��λ */

private:
    Buf bFreeList;							/* ������п��ƿ� */
    Buf m_Buf[NBUF];						/* ������ƿ����� */
    unsigned char Buffer[NBUF][BUFFER_SIZE];	/* ���������� */
    std::map<int, Buf*> bMap;     /* ʹ�õĻ����map */

public:
    BufferManager();
    ~BufferManager();

    void Initialize();					/* ������ƿ���еĳ�ʼ������������ƿ���b_addrָ����Ӧ�������׵�ַ��*/

    Buf* GetBlk(int blkno);	            /* ����һ�黺�棬���ڶ�д�豸�ϵ��ַ���blkno��*/
    void Brelse(Buf* bp);				/* �ͷŻ�����ƿ�buf */

    Buf* Bread(int blkno);	/* ��һ�����̿顣blknoΪĿ����̿��߼���š� */

    void Bwrite(Buf* bp);				/* дһ�����̿� */
    void Bdwrite(Buf* bp);              /* �ӳ�д���̿� */

    void ClrBuf(Buf* bp);				/* ��ջ��������� */
    void Bflush();				/* ���ӳ�д�Ļ���ȫ����������� */

    void Format();                      /* ��ʽ����ԭV6++�в����� */		
	
private:
    void GetError(Buf* bp);
    void NotAvail(Buf* pb);
};

#endif