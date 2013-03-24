#ifndef FIFOBUFFER_H
#define FIFOBUFFER_H
#include <windows.h>

#define  AUDIOBUFLEN (8*65536)
struct AVFrameBuffer 
{
	AVFrameBuffer *prev;
	AVFrameBuffer *next;	
	unsigned char *context;
	int width ;
	int height ;
	int size;
	//Frame bit rate
	int frameLen;
	int frameType;
	int sampleRate;
};

class FifoBuffer 
{
public:
	FifoBuffer();
	~FifoBuffer();

	int init(int count,int chunksize);
	int reset();
	int resize(int chunksize);
	int clear();
	bool write(AVFrameBuffer* vframe) ;
	bool read(AVFrameBuffer* vframe) ;
	int chunkCount();//��Ч���� ����
	void setlock(bool iflock) ;
	bool getlock();
	BYTE* GetLastFrame();
	BYTE* GetNextWritePos();

	void SetbReadNull(bool ifReadNull){m_bReadNull = ifReadNull;}
	bool GetIfReadNull(){return m_bReadNull;}

	void GetLastFrameBuf(unsigned char* pBuf){m_dataFrame.context = pBuf;}

private:		
	bool createFreeList();	
	void cleanFreeList();
	void cleanDataList();
	
	AVFrameBuffer *getFreeNode();
	AVFrameBuffer *getDataNode();
	
	void appendToFreeList(AVFrameBuffer *item);
	void appendToDataList(AVFrameBuffer *item);

private:

	unsigned char *m_mempool;//�ڴ��

	bool m_ifLock ;//�����������write������һֱ�ȴ��пյĻ���飬�ڶ�λ����ʱ��������ΪFalse,��ʹwrite�������췵��
	bool m_inited; // ��ʼ���ɹ���־
	int m_count; // �ڵ�����
	int m_chunksize;
	AVFrameBuffer *m_freeQueueFirst;
	AVFrameBuffer *m_freeQueueLast;
	int m_freeCount;//�յĿ����

	AVFrameBuffer *m_outQueueFirst;
	AVFrameBuffer *m_outQueueLast;
	int m_outCount;//��Ч���ݿ���

	AVFrameBuffer m_dataFrame;
	CRITICAL_SECTION m_DataAccessLock ;

	bool m_bReadNull;
};

#endif 




















