#include "pcmrender.h"
#include "..\..\..\..\DecodeSdk\Trunk\SRC\global.h"

#include <process.h> 
#include <string.h>
#include <stdio.h>

//#define DUMP_PCM

#ifdef DUMP_PCM
FILE *pcmdata = 0;
#endif

/***********************************************************************
 *  ���캯��
************************************************************************/
CSoundChannel_Win32::CSoundChannel_Win32(int dir)
{
	m_direction = dir;

	initParam();
#ifdef DUMP_PCM
	pcmdata = fopen("audio_r.pcm", "w+b");
#endif
}

/************************************************************************
 * ��������
************************************************************************/
CSoundChannel_Win32::~CSoundChannel_Win32()
{
	clean();

	CloseHandle(m_hEventDone);
	CloseHandle(m_mutex);

#ifdef DUMP_PCM
	fclose(pcmdata);
#endif
}

/**********************************************************************
 * �ڲ�ʹ�ã���ʼ������                                                                     
***********************************************************************/
void CSoundChannel_Win32::initParam()
{
	m_hWaveOut = NULL;
	m_hWaveIn = NULL;
	
	m_hEventDone = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_mutex = CreateMutex(NULL,FALSE,NULL);

	m_bufferIndex = 0;
}

void CSoundChannel_Win32::setFormat(int channels, int samplePerSec, int bitsPerSample)
{
	// ��һ��: ��ȡwaveformat��Ϣ
	m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels = channels;
	m_waveFormat.wBitsPerSample = bitsPerSample;
	m_waveFormat.nSamplesPerSec = samplePerSec;
	m_waveFormat.nBlockAlign = 
		m_waveFormat.nChannels * m_waveFormat.wBitsPerSample / 8;
	m_waveFormat.nAvgBytesPerSec = 
		m_waveFormat.nSamplesPerSec * m_waveFormat.nBlockAlign;
	m_waveFormat.cbSize = sizeof(m_waveFormat);

	m_bitsPerSample = bitsPerSample;
	m_samplesPerSecond = samplePerSec;
}

/***********************************************************************
 * ��ʼ��CSoundChannel_Win32����
 *
************************************************************************/
int CSoundChannel_Win32::init(int channels, int samplePerSec, int bitsPerSample, HWND hWnd)
{
	if (m_direction==0) {
		if (m_hWaveOut != NULL) {
			return 0;// �Ѿ������˳�ʼ��
		}
	} else {
		if (m_hWaveIn != NULL) {
			return 0;
		}
	}

	setFormat(channels,samplePerSec,bitsPerSample);
		
	MMRESULT ret = waveOutOpen(NULL, WAVE_MAPPER, &m_waveFormat, 
			NULL, NULL, WAVE_FORMAT_QUERY);
		
	if (MMSYSERR_NOERROR != ret) {
		return -1;
	}
		
	if (m_direction==0) {
		ret = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &m_waveFormat, 
			(DWORD)m_hEventDone,0,CALLBACK_EVENT);
	} else {
		ret = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &m_waveFormat, 
			(DWORD)m_hEventDone,0,CALLBACK_EVENT);
	}
		
	if (MMSYSERR_NOERROR != ret) {
		return -1;
	}

	reset();
	start();
    
    return 0;
}

/************************************************************************
 * ���û����������ԣ����������������ա�ָʾ�Լ���д��
************************************************************************/
void CSoundChannel_Win32::reset()
{
	m_bufferIndex = 0;
}

/***********************************************************************
 *  ����CSoundChannel_Win32����	
 *
 *  1. ���ȴ����߳��������												
 *  2. �ٴ���WaveOut��ض���
 *                                                                   
 ************************************************************************/
int CSoundChannel_Win32::clean()
{
	if (m_hWaveOut==NULL) {
		return -2;
	}
	
	Abort();
	
	if (m_hWaveOut != NULL) {
		while (waveOutClose(m_hWaveOut)==WAVERR_STILLPLAYING) {
			waveOutReset(m_hWaveOut);
		}
		m_hWaveOut = NULL;
	}
	
	if (m_hWaveIn != NULL) {
		while (waveInClose(m_hWaveIn)==WAVERR_STILLPLAYING) {
			waveInReset(m_hWaveIn);
		}
		m_hWaveOut = NULL;
	}
	
	Abort();

	return 0;
}

BOOL CSoundChannel_Win32::Abort()
{
	DWORD osError = MMSYSERR_NOERROR;
	
	if (m_hWaveOut != NULL) {
		osError = waveOutReset(m_hWaveOut);
	}
	
	if (m_hWaveIn != NULL) {
		osError = waveInReset(m_hWaveIn);
	}
	
	{
	WaitForSingleObject(m_mutex,INFINITE);
	
	if (m_hWaveOut!=NULL) {
		for (unsigned i = 0; i<PCM_BUF_COUNT; i++) {
			while (m_buffer[i].Release()==WAVERR_STILLPLAYING) {
				if (m_hWaveOut!=NULL) {
					waveOutReset(m_hWaveOut);
				}	
			}
		}
	}

	if (m_hWaveIn!=NULL) {
		for (unsigned i = 0; i<PCM_BUF_COUNT; i++) {
			while (m_buffer[i].Release()==WAVERR_STILLPLAYING) {
				if (m_hWaveIn!=NULL) {
					waveInReset(m_hWaveIn);
				}	
			}
		}
	}

	ReleaseMutex(m_mutex);

	m_bufferIndex = 0;
	
	// Signal any threads waiting on this event, they should then check
	// the bufferByteOffset variable for an abort.
	SetEvent(m_hEventDone);
	}
	
	if (osError != MMSYSERR_NOERROR)
		return FALSE;
	
	return TRUE;
}

/***********************************************************************
 * ��ֹ�����̣߳�
 * ���ȷ����źţ����߳��Լ���ֹ��
************************************************************************/
void CSoundChannel_Win32::terminating()
{
}

/************************************************************************/
/* ������                                                                     */
/************************************************************************/
int CSoundChannel_Win32::read(unsigned char *pcm, int *len)
{
	if (m_hWaveOut==NULL) {
		return -1;
	}
	
	unsigned char *ptr = pcm;
	
	WaitForSingleObject(m_mutex,INFINITE);
	
	DWORD osError = MMSYSERR_NOERROR;
	
	while (len>0) {
		DWORD flags = m_buffer[m_bufferIndex].header.dwFlags;
		while ((flags&WHDR_DONE) == 0) {
			
			//dbg_print(100,"AUDIO BUFFER OVERFLOW.\n");
			//return 0; // �������̫�࣬�����������.
			
			ReleaseMutex(m_mutex);
			
			if (WaitForSingleObject(m_hEventDone, INFINITE) != WAIT_OBJECT_0) {
				return 0;
			}
			
			WaitForSingleObject(m_mutex,INFINITE);
			
			flags = m_buffer[m_bufferIndex].header.dwFlags;
		}
		
		if (*len<m_buffer[m_bufferIndex].header.dwBufferLength) {
			*len = 0;
			return 0;
		}

		memcpy(ptr,m_buffer[m_bufferIndex].data ,m_buffer[m_bufferIndex].header.dwBytesRecorded);

		*len = m_buffer[m_bufferIndex].header.dwBytesRecorded;

		osError = m_buffer[m_bufferIndex].Prepare(m_hWaveIn, PCM_BUF_SIZE);
		if (osError!=MMSYSERR_NOERROR) {
			break;
		}
		
		osError = waveInAddBuffer(m_hWaveIn, &m_buffer[m_bufferIndex].header, sizeof(WAVEHDR));
		if (osError!= MMSYSERR_NOERROR) {
			break;
		}
		
		m_bufferIndex = (m_bufferIndex+1)%PCM_BUF_COUNT;
		
		break;
	}
	
	ReleaseMutex(m_mutex);
	
	return 0;
}

/************************************************************************
 * д����
************************************************************************/
int CSoundChannel_Win32::write(unsigned char *pcm, int len)
{
	if (m_hWaveOut==NULL) {
		return -1;
	}

	unsigned char * ptr = pcm;

	WaitForSingleObject(m_mutex,INFINITE);

	DWORD osError = MMSYSERR_NOERROR;

	while (len>0) {
		DWORD flags = m_buffer[m_bufferIndex].header.dwFlags;
		while ((flags&WHDR_DONE) == 0) {

			dbg_print(100,"AUDIO BUFFER OVERFLOW.\n");	

			ReleaseMutex(m_mutex);

		//	return 0; // �������̫�࣬�����������.

			if (WaitForSingleObject(m_hEventDone, INFINITE) != WAIT_OBJECT_0) {
				return -1;
			}

			WaitForSingleObject(m_mutex,INFINITE);

			flags = m_buffer[m_bufferIndex].header.dwFlags;
		}
		
		osError = m_buffer[m_bufferIndex].Prepare(m_hWaveOut, len);
		if (osError!=MMSYSERR_NOERROR) {
			break;
		}
		
		memcpy(m_buffer[m_bufferIndex].data, ptr, len);
		
		osError = waveOutWrite(m_hWaveOut, &m_buffer[m_bufferIndex].header, sizeof(WAVEHDR));
		if (osError!= MMSYSERR_NOERROR) {
			break;
		}
		
		m_bufferIndex = (m_bufferIndex+1)%PCM_BUF_COUNT;

		break;
	}

	ReleaseMutex(m_mutex);

	return 0;
}

/************************************************************************
 * �����߳�
************************************************************************/
int CSoundChannel_Win32::start()
{
	if (m_direction==1) {
		if (m_hWaveIn!=NULL) {
			for (unsigned i = 0; i<PCM_BUF_COUNT; i++) {
				if (m_buffer[i].Prepare(m_hWaveIn,PCM_BUF_SIZE)==MMSYSERR_NOERROR) {
					if ((waveInAddBuffer(m_hWaveIn, &m_buffer[i].header, sizeof(WAVEHDR)))!= MMSYSERR_NOERROR)
						return -1;
				} else return -1;
			}

			waveInStart(m_hWaveIn);
		}
	}

	return 0;
}

bool CSoundChannel_Win32::SetVolume(DWORD wNewVolume)
{
	MMRESULT ret = MMSYSERR_NOERROR;
	ret = waveOutSetVolume(m_hWaveOut, wNewVolume<<16|wNewVolume);

	return (MMSYSERR_NOERROR==ret)?true:false;
}

DWORD CSoundChannel_Win32::GetVolume()
{
	DWORD wOldVolume = 0;
	
	MMRESULT ret = MMSYSERR_NOERROR;
	
	ret = waveOutGetVolume(m_hWaveOut, &wOldVolume);
	
	return wOldVolume;
}

