#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "FileSystem.h"
#include "OpenFileManager.h"
#include "File.h"

/*
 * �ļ�������(FileManager)
 * ��װ���ļ�ϵͳ�ĸ���ϵͳ�����ں���̬�´�����̣�
 * ����ļ���Open()��Close()��Read()��Write()�ȵ�
 * ��װ�˶��ļ�ϵͳ���ʵľ���ϸ�ڡ�
 */
class FileManager
{
public:
    /* Ŀ¼����ģʽ������NameI()���� */
    enum DirectorySearchMode
    {
        OPEN = 0,		/* �Դ��ļ���ʽ����Ŀ¼ */
        CREATE = 1,		/* ���½��ļ���ʽ����Ŀ¼ */
        DELETE = 2		/* ��ɾ���ļ���ʽ����Ŀ¼ */
    };

    /* Functions */
public:
    /* Constructors */
    FileManager();
    /* Destructors */
    ~FileManager();

    /*
     * @comment ��ʼ����ȫ�ֶ��������
     */
    void Initialize();

    /*
     * @comment Open()���ô������
     */
    void Open();

    /*
     * @comment Creat()���ô������
     */
    void Creat();

    /*
     * @comment Open()��Creat()ϵͳ���õĹ�������
     */
    void Open1(Inode* pInode, int mode, int trf);

    /*
     * @comment Close()���ô������
     */
    void Close();

    /*
     * @comment Seek()���ô������
     */
    void Seek();

    /*
     * @comment Read()���ô������
     */
    void Read();

    /*
     * @comment Write()���ô������
     */
    void Write();

    /*
     * @comment ��д���ù������ִ���
     */
    void Rdwr(enum File::FileFlags mode);


    /*
     * @comment Ŀ¼��������·��ת��Ϊ��Ӧ��Inode��
     * �����������Inode
     */
    Inode* NameI(enum DirectorySearchMode mode);

    /*
     * @comment ��Creat()����ʹ�ã�����Ϊ�������ļ������ں���Դ
     */
    Inode* MakNode(unsigned int mode);

    /*
     * @comment ��Ŀ¼��Ŀ¼�ļ�д��һ��Ŀ¼��
     */
    void WriteDir(Inode* pInode);

    /*
     * @comment �����ļ���Ŀ¼������������Ȩ�ޣ���Ϊϵͳ���õĸ�������
     */
    int Access(Inode* pInode, unsigned int mode);
    
    /* ɾ���ļ� */
    void UnLink();


    /* �ı䵱ǰ����Ŀ¼ */
    void ChDir();


    /* ��ʽ����ԭV6++���� */
    void Format();

public:
    /* ��Ŀ¼�ڴ�INode */
    Inode* rootDirInode;

    /* ��ȫ�ֶ���g_FileSystem�����ã��ö���������ļ�ϵͳ�洢��Դ */
    FileSystem* m_FileSystem;

    /* ��ȫ�ֶ���g_InodeTable�����ã��ö������ڴ�INode��Ĺ��� */
    InodeTable* m_InodeTable;

    /* ��ȫ�ֶ���g_OpenFileTable�����ã��ö�������ļ�����Ĺ��� */
    OpenFileTable* m_OpenFileTable;
};

#endif