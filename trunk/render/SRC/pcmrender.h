#ifndef PCMRENDER_H
#define PCMRENDER_H

#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>

#define PCM_BUF_COUNT	32
#define PCM_BUF_SIZE	(1024*8)

class CAudioRender
{
public:
	virtual int init(int channels, int samplePerSec, int bitPerSample, HWND hWnd)=0;
	virtual int clean() = 0;
	// д����
	virtual int write(unsigned char *pcm, int len) = 0;
	virtual int read(unsigned char *pcm, int *len) = 0;
	// �������߳���صķ���
	virtual void terminating() = 0; // ��ֹ����
	virtual bool SetVolume(DWORD wNewVolume) = 0;
	virtual DWORD GetVolume() = 0;

	int m_bitsPerSample ;
	int m_samplesPerSecond ;
};

class CSoundChannel_Win32 : public CAudioRender
{
	struct PcmBuffer {
		PcmBuffer() 
		{
			hWaveOut = NULL;
			hWaveIn = NULL;

			memset(&header, 0, sizeof(header));
			header.dwFlags |= WHDR_DONE;
		}
		
		DWORD Prepare(HWAVEIN hIn,unsigned int count) 
		{
			Release();
			
			memset(&header, 0, sizeof(header));
			
			header.lpData = data;
			header.dwBufferLength = count;
			header.dwUser = (DWORD)this;
			
			hWaveIn = hIn;
			
			return waveInPrepareHeader(hWaveIn, &header, sizeof(header));
		}

		DWORD Prepare(HWAVEOUT hOut, unsigned int count)
		{
			Release();
			
			memset(&header, 0, sizeof(header));
			
			header.lpData = data;
			header.dwBufferLength = count;
			header.dwUser = (DWORD)this;

			hWaveOut = hOut;

			return waveOutPrepareHeader(hWaveOut, &header, sizeof(header));
		}

		DWORD Release()
		{
			DWORD err = MMSYSERR_NOERROR;
			
			if (hWaveOut!=NULL) {
				err = waveOutUnprepareHeader(hWaveOut, &header, sizeof(header));
				if (err== WAVERR_STILLPLAYING) {
					return err;
				}

				hWaveOut = NULL;
			}

			if (hWaveIn!=NULL) {
				err = waveInUnprepareHeader(hWaveIn, &header, sizeof(header));
				if (err== WAVERR_STILLPLAYING) {
					return err;
				}
				
				hWaveIn = NULL;
			}
			
			header.dwFlags |= WHDR_DONE;
			
			return err;
		}

		char data[PCM_BUF_SIZE];

		WAVEHDR header;

		HWAVEOUT hWaveOut;
		HWAVEIN	 hWaveIn;
	};
	
public:
	CSoundChannel_Win32(int dir = 0);
	~CSoundChannel_Win32();

	// �����ֱ�Ϊͨ������������
	virtual int init(int channels, int samplePerSec, int bitPerSample, HWND hWnd);
	virtual int clean();

	// д����
	virtual int write(unsigned char *pcm, int len);
	virtual int read(unsigned char *pcm, int *len);
	
	// �������߳���صķ���
	virtual void terminating(); // ��ֹ����

	virtual bool SetVolume(DWORD wNewVolume);
	virtual DWORD GetVolume();
	
private:
	// �ڲ�����
	void initParam(); // ��ʼ������
	void setFormat(int channels, int samplePerSec, int bitPerSample);
	
	void reset(); // ���û����״̬
	int start(); // ����
	
	int Abort();
private:
	// ������Ϣ

	int m_direction; // ����0-���ţ�1-¼��

	WAVEFORMATEX m_waveFormat;

    HWAVEOUT m_hWaveOut;	// ���ž��
	HWAVEIN  m_hWaveIn;		// ¼�����

	HANDLE m_hEventDone;	// Ӳ��֪ͨ������

	int m_bufferIndex;

	HANDLE m_mutex;
	
	// ���Ż����� 
	PcmBuffer m_buffer[PCM_BUF_COUNT];

};

#endif /* PCMRENDER_H */
