#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "Buf.h"
#include "DiskDriver.h"
#include <map>

class BufferManager 
{
public:
    /* static const member */
    static const int NBUF = 15;			/* 缓存控制块、缓冲区的数量 */
    static const int BUFFER_SIZE = 512;	/* 缓冲区大小。 以字节为单位 */

private:
    Buf bFreeList;							/* 缓存队列控制块 */
    Buf m_Buf[NBUF];						/* 缓存控制块数组 */
    unsigned char Buffer[NBUF][BUFFER_SIZE];	/* 缓冲区数组 */
    std::map<int, Buf*> bMap;     /* 使用的缓存块map */

public:
    BufferManager();
    ~BufferManager();

    void Initialize();					/* 缓存控制块队列的初始化。将缓存控制块中b_addr指向相应缓冲区首地址。*/

    Buf* GetBlk(int blkno);	            /* 申请一块缓存，用于读写设备上的字符块blkno。*/
    void Brelse(Buf* bp);				/* 释放缓存控制块buf */

    Buf* Bread(int blkno);	/* 读一个磁盘块。blkno为目标磁盘块逻辑块号。 */

    void Bwrite(Buf* bp);				/* 写一个磁盘块 */
    void Bdwrite(Buf* bp);              /* 延迟写磁盘块 */

    void ClrBuf(Buf* bp);				/* 清空缓冲区内容 */
    void Bflush();				/* 将延迟写的缓存全部输出到磁盘 */

    void Format();                      /* 格式化，原V6++中不存在 */		
	
private:
    void GetError(Buf* bp);
    void NotAvail(Buf* pb);
};

#endif