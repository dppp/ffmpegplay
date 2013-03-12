#ifndef PCMRENDER1_H
#define PCMRENDER1_H

#include <dsound.h>
#include <windows.h>

#define DSOUND_BUFFER_COUNT 4

class PcmRender1 
{
public:
	PcmRender1();
	virtual ~PcmRender1();
	
	// �����ֱ�Ϊͨ������������
	int init(int channels, int samplePerSec, int bitsPerSample, HWND hwnd);
	int clean();
	
	// д����
	int write(unsigned char *pcm, int len);
	
	// �������߳���صķ���
	void terminating(); // ��ֹ����
	void loop(); // ��ʼ����ѭ������һ�������̣߳�
	
	// ����Ƿ����
    BOOL inited();

protected:
	void reset(); // ���û����״̬
	int start(); // ����

	HRESULT destroyDSObjects();

private:
	LPDIRECTSOUND m_pDS;

	LPDIRECTSOUNDBUFFER m_pDSBuffer[DSOUND_BUFFER_COUNT];
	HANDLE m_hasData[DSOUND_BUFFER_COUNT];
	HANDLE m_hasNoData[DSOUND_BUFFER_COUNT];

	int m_readIndex;
	int m_writeIndex;

	PCMWAVEFORMAT m_waveFormat;

	BOOL m_terminating;
	HANDLE m_hThread;
	unsigned long m_threadId;
};
#endif // PCMRENDER1_H
