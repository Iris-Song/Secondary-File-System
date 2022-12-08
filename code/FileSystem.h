#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "INode.h"
#include "DiskDriver.h"
#include "BufferManager.h"

/*
 * �ļ�ϵͳ�洢��Դ�����(Super Block)�Ķ��塣
 */
class SuperBlock 
{
public:
    const static int NFREE = 100;
    const static int NINODE = 100;
    /* Functions */
public:
    /* Constructors */
    SuperBlock();
    /* Destructors */
    ~SuperBlock();

    /* Members */
public:
    int		s_isize;		/* ���Inode��ռ�õ��̿��� */
    int		s_fsize;		/* �̿����� */

    int		s_nfree;		/* ֱ�ӹ���Ŀ����̿����� */
    int		s_free[NFREE];	/* ֱ�ӹ���Ŀ����̿������� */

    int		s_ninode;		/* ֱ�ӹ���Ŀ������Inode���� */
    int		s_inode[NINODE];	/* ֱ�ӹ���Ŀ������Inode������ */

    int	s_flock;		        // ���������̿��������־
    int	s_ilock;		        // ��������INode���־

    int	s_time;			        // ���һ�θ���ʱ��
    
    int		s_fmod;			/* �ڴ���super block�������޸ı�־����ζ����Ҫ��������Ӧ��Super Block */
    int		s_ronly;		/* ���ļ�ϵͳֻ�ܶ��� */
    int		padding[47];	/* ���ʹSuperBlock���С����1024�ֽڣ�ռ��2������ */

    /* Functions */
public:
    void Format();
};

class DirectoryEntry 
{
public:
    static const int DIRSIZ = 28;	/* Ŀ¼����·�����ֵ�����ַ������� */

public:
    int m_ino;		    /* Ŀ¼����INode��Ų��� */
    char m_name[DIRSIZ];	/* Ŀ¼����·�������� */
};

/*
 * �ļ�ϵͳ��(FileSystem)�����ļ��洢�豸��
 * �ĸ���洢��Դ�����̿顢���INode�ķ��䡢
 * �ͷš�
 */
class FileSystem 
{
public:
    /* static consts */
    // Block���С
    static const int BLOCK_SIZE = 512;

    // ����������������
    static const int DISK_SIZE = 16384;

    static const int SUPER_BLOCK_SECTOR_NUMBER = 0;	/* ����SuperBlockλ�ڴ����ϵ������ţ�ռ������������*/
    
    static const int ROOTINO = 0;			/* �ļ�ϵͳ��Ŀ¼���Inode��� */

    static const int INODE_NUMBER_PER_SECTOR = 8;		/* ���INode���󳤶�Ϊ64�ֽڣ�ÿ�����̿���Դ��512/64 = 8�����Inode */
    static const int INODE_ZONE_START_SECTOR = 2;		/* ���Inode��λ�ڴ����ϵ���ʼ������ */
    static const int INODE_ZONE_SIZE = 1024 - 2;		/* ���������Inode��ռ�ݵ������� */
    static const int INODE_NUMBER = INODE_ZONE_SIZE * INODE_NUMBER_PER_SECTOR;/*Inode����*/

    static const int DATA_ZONE_START_SECTOR = 1024;		/* ����������ʼ������ */
    static const int DATA_ZONE_END_SECTOR = DISK_SIZE - 1;	/* �������Ľ��������� */
    static const int DATA_ZONE_SIZE = DISK_SIZE - DATA_ZONE_START_SECTOR;	/* ������ռ�ݵ��������� */

    /* Functions */
public:
    /* Constructors */
    FileSystem();
    /* Destructors */
    ~FileSystem();

    /*
 * @comment ��ʼ����Ա����
 */
    void Initialize();

    /*
    * @comment ϵͳ��ʼ��ʱ����SuperBlock
    */
    void LoadSuperBlock();

    /*
     * @comment ��SuperBlock������ڴ渱�����µ�
     * ������ȥ
     */
    void Update();

    /*
     * @comment  ����һ������
     * ���INode��һ�����ڴ����µ��ļ���
     */
    Inode* IAlloc();
    /*
     * @comment  �ͷű��Ϊnumber
     * �����INode��һ������ɾ���ļ���
     */
    void IFree(int number);

    /*
     * @comment ������д��̿�
     */
    Buf* Alloc();
    /*
     * @comment �ͷű��Ϊblkno�Ĵ��̿�
     */
    void Free(int blkno);

    void Format();

private:
    DiskDriver* m_DiskDriver;
    SuperBlock* m_spb;/*��ԭ��Mount��ɾ��������һ��SuperBlock*/
	BufferManager* m_BufferManager;		/* FileSystem����Ҫ�������ģ��(BufferManager)�ṩ�Ľӿ� */
};

#endif