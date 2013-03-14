/*************************************************************************
��������: 2005��12��2��
���ߣ� �뽭(feng_jiang@dhmail.com)

2005��12��2�գ� ����
*************************************************************************/

#ifndef DHRENDERLIB_H
#define DHRENDERLIB_H

#include <windows.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RENDER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RENDER_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef RENDER_EXPORTS
#define RENDER_API __declspec(dllexport)
#else
#define RENDER_API __declspec(dllimport)
#endif

// ����
#define RENDER_ERR_NOERROR	0

// ��������,����˵һ���Դ��������-1
// ������������ڴ����
#define RENDER_ERR_FATAL	(-1)
#define RENDER_ERR_MEMORY	(-1)

// ��ʹ��,�����Դ���
#define RENDER_ERR_WARNING	(-2)
#define RENDER_ERR_USED		(-2)

// ��Ƶ��ʾ��ش���,����DirectDraw�����
#define RENDER_ERR_VIDEO	(-3)
#define RENDER_ERR_DDRAW	(-3)

// ��Ƶ��ش���
#define RENDER_ERR_AUDIO	(-4)

//ͨ����������
#define RENDER_ERR_CHANNEL_INDEX	(-5)

enum VideoRenderMethod
{
	ByDDOffscreen = 0,	// Ĭ��, DirectDraw Offscreen��ʽ,֧��OSD
	ByDDOverlay = 1,	// Overlay��ʽ,��֧��OSD
	ByGDI = 2			// ���Ǳ�ѡ,֧��OSD
};

enum AudioRenderMethod
{
	DIRECTSOUND_WITH_LOOPBUFFER = 0,
	PCM_OUT = 1
};

typedef void (__stdcall *draw_callback)(int index, HDC hDc, long dwUser);

RENDER_API int render_init(long hWnd=0);
RENDER_API int render_open(int index, HWND hWnd, int width, int height, draw_callback dcb, long udata, VideoRenderMethod vrm, DWORD ck);
RENDER_API int render_close_all();
RENDER_API int render_close(int index);
RENDER_API int render_video(int index, unsigned char *py, unsigned char *pu, unsigned char *pv, int width, int height);
RENDER_API int render_audio(int index, unsigned char *pcm, int len, int bits, int sampling);
/*!
 * ץͼ����
 * ����������
 * handle: ���
 * path: ·��
 * basename: �����ļ�������������չ������չ�����ݸ�ʽ����
 * format: ��ʽ, 0��bmp��1��jpg
 * count������ץͼ��
**/
RENDER_API int render_grab_picture(int index, const char *path = "", const char *basename = "DHGRAB", int format = 0, int count = 1);

/*!
 * ��ȡ�汾��.
 */
RENDER_API int render_version(int *major, int *minor, int *patcher);

RENDER_API int render_set_volume(int index, DWORD lVolume );// ��Χ: 0 -- 0xFFFF, 0 ������С

//������Ƶ�����ʽ����2�����£�(Ĭ����PCM_OUT)
//DIRECTSOUND_WITH_LOOPBUFFER 
//PCM_OUT 
RENDER_API int render_audio_set_style(int style);

//�ַ�����
RENDER_API int render_show_string(int index, //ͨ����
								  char *pString, //�ַ���
								  int x, //��ʼx����
								  int y, //��ʼy����
								  COLORREF OSDColor, //������ɫ
								  bool bTransparent, //�Ƿ�͸��
								  COLORREF BackColor, //������ɫ
								  HFONT OSDFont); //������ɫ

// �ֲ��Ŵ�,�����Ĳ���ΪԭʼYUVͼ���һ������
RENDER_API int render_show_rect(int index, int left, int top, int right, int bottom);

// �ֲ��Ŵ�,�����Ĳ���ΪԭʼYUVͼ���һ������
RENDER_API int render_show_mask(int index, int enabled, int maskid, int left, int top, int right, int bottom);

#endif /* DHRENDERLIB_H */
