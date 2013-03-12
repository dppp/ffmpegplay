#ifndef AUDIORENDER_H
#define AUDIORENDER_H

#include "render.h"

struct AudioRender
{
	virtual int init(int channels, int samplePerSec, int bitPerSample, HWND hWnd)=0;
	virtual int clean() = 0;
	// д����
	virtual int write(unsigned char *pcm, int len) = 0;
	// �������߳���صķ���
	virtual void terminating() = 0; // ��ֹ����
	virtual bool SetVolume(DWORD wNewVolume) = 0;
	virtual WORD GetVolume() = 0;

	int m_bitsPerSample ;
	int m_samplesPerSecond ;
};

#endif // AUDIORNDER_H
