#pragma once
#include <windows.h>
#include <render.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include <atlstr.h>
#include "FifoBuffer.h"
extern "C" 
{
#include "avcodec.h"
#include "avformat.h"
#include "cmdutils.h "
}

#define MAX_FRAME_BUFFER_SIZE 50
#define MAX_IMAGE_WIDTH 1280
#define MAX_IMAGE_HEIGHT 720
typedef struct PacketQueue {
	AVPacketList *first_pkt, *last_pkt;
	int nb_packets;
	int size;
	int abort_request;
	CRITICAL_SECTION criticalSection;
} PacketQueue;
typedef struct VideoState {
	AVStream *audio_st;
	AVStream *video_st;
	AVFormatContext *ic;
	int video_stream;
	int audio_stream;
	SwsContext *img_convert_ctx;
	double video_current_pts;
	double audio_clock;
	PacketQueue videoq;
	PacketQueue audioq;
} VideoState;

class CffPlay
{
public:
	CffPlay(void);
	~CffPlay(void);
	void playMpegFile(char* fileName, HWND hWnd);
	void playPause();
	void playClose();
	double playGetTotalTime();
	double playGetCurrentTime();
	void playSetSeekPosition(unsigned int pos);
	void playOnlyAudio(BOOL isOnlyAudio);
private:
	AVPacket m_flushPkt;
	HANDLE m_hreadProcess;
	HANDLE m_ffmpegDecHandler;
	HANDLE m_ffmpegRenderHandler;
	VideoState m_currentStream;
	AVFrameBuffer* m_item ;
	FifoBuffer m_AvDataList;
	HWND m_hWnd;
	BOOL m_bDirectDrawInited;
	static DWORD WINAPI ffmpegReadPro(LPVOID pParam);
	static DWORD WINAPI ffmpegDecPro(LPVOID pParam);
	static DWORD WINAPI ffmpegRenderPro(LPVOID pParam);
	int stream_component_open(VideoState *is, int stream_index);
	inline int initDirectDraw(HWND hWnd, int nWidth, int nHight);
};