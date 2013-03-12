#ifndef DDRENDER_H
#define DDRENDER_H

#include "render.h"
#include "videorender.h"
#include "colorspace.h"

#include <windows.h>
#include <ddraw.h>
#include <stdio.h>

//#define PRINT_LOG
#define LPMYDIRECTDRAW			LPDIRECTDRAW7
#define LPMYDIRECTDRAWSURFACE	LPDIRECTDRAWSURFACE7
#define MYDDSURFACEDESC			DDSURFACEDESC2
#define IID_MyDirectDraw		IID_IDirectDraw7

class MyVideoRender : public VideoRender
{
public:
	MyVideoRender(int type=0, bool automode = true);// ���캯��
	virtual ~MyVideoRender();// ��������

	// �ӿں���

	virtual int init(
		int index, HWND hWnd, 
		int width, int height, 
		draw_callback cb, long udata);

	virtual int clean();

	virtual int render(
		unsigned char *py, unsigned char *pu, unsigned char *pv, 
		int width, int height);

	virtual void resize();
	virtual int SetString(char * pcStr,
					  int x,
					  int y, 
					  COLORREF OSDColor, 
					  bool bTransparent,
					  COLORREF BackColor,
					  HFONT OSDFont );

	virtual int SetShowRect(int left, int top, int right, int bottom);
	virtual int SetMaskRect(int maskid, int left, int top, int right, int bottom);
	virtual int CleanMaskRect();
	virtual HWND GetWind();

private:
	/***************************************************
	 * ����DIRECTDRAW
	 ***************************************************/
	
	int initDirectDraw(); // ��ʼ��DirectDraw
	HRESULT createDrawSurface(bool overlay=false); // ������ͼ����
	HRESULT destroyDDObjects(); // �ڲ�����
    BOOL hasOverLaySupport(); // ����Ƿ�֧��OVERLAY
	BOOL hasFourCCSupport(LPMYDIRECTDRAWSURFACE lpdds); // ���֧�ֵ�YUV��ʽ
	HRESULT createClipper(HWND hwnd); // ����������(�����Ͳ���Ҫ)
	void adjustRect(RECT &rcSrc, RECT &rcDest);
	DWORD convertGDIColor( COLORREF dwGDIColor);
	HRESULT RestoreAll();

	/****************************************************
	 * ��ͼ
	 ****************************************************/
	void drawMaskRect(
		unsigned char *py,unsigned char *pu,unsigned char *pv,
		int width, int height);

	void drawMotion(
		unsigned char *py,unsigned char *pu,unsigned char *pv,
		int width, int height);

	/********************************************************
	 *
	 ********************************************************/
	inline int DisplayBYGDI(
		unsigned char *py, unsigned char *pu, unsigned char *pv, 
		int width, int height);

	virtual int DisplayByOffscreen(
		unsigned char *py, unsigned char *pu, unsigned char *pv, 
		int width, int height);

	virtual int DisplayByOverlay(
		unsigned char *py, unsigned char *pu, unsigned char *pv, 
		int width, int height);
private:
	
	int		m_index;	// �������
	int		m_type;		// ������ͣ���Ҫ��0-Offscreen��1-Overlay��GDIΪ�����ǿ���.
	bool	m_automode;	// �Զ�ѡ��m_type��ʾ������ʵ������
	HWND	m_hWnd;		// ���ھ��

	int		m_width;	// Ҫ�����Ļ�ͼ����Ŀ��
	int		m_height;	// Ҫ�����Ļ�ͼ����ĸ߶�
	int		m_screenWidth;	// ��Ļʵ�ʿ�
	int		m_screenHeight;	// ��Ļʵ�ʸ�

	draw_callback	m_callback;	// ��ͼ�ص�����
	long			m_userData;	// �û��Զ�������

	LPMYDIRECTDRAW        m_pDD;				// DIRECTDRAW�������
	LPMYDIRECTDRAWSURFACE m_pDDSPrimary;		// ������
	LPMYDIRECTDRAWSURFACE m_pDDVideoSurface;	//��ͼ����
	LPMYDIRECTDRAWSURFACE m_pDDVideoSurfaceBack;//��ͼ����(��̨,˫����ģ��,����Ҫ)
	
	/*************************************************
	 * Overlay��ʽ��Ҫʹ�õĲ���
	/*************************************************/ 
    DDOVERLAYFX m_OverlayFX;
    DWORD m_dwOverlayFlags;
	DDBLTFX	m_ddbltfx;
	bool m_updataOverlay;
	DWORD m_colorKey;
	
	// Overlay֧�ֱ�ʶ
    BOOL m_hasOverlaySupport;
	
	DDCAPS  m_ddCaps; // ��ͼ������Ϣ

	BOOL m_hasFourCCSupport; // FourCC֧�ֱ�ʶ

	// ��ɫ�ռ�ת������
	color_convert_func m_colorConvert;

	RECT m_destRect;	// ���ڴ�С
	
	//OSD����
	class OSD {
	public:
		OSD();
		~OSD(){}

		void setText(const char *string);
		void setPosition(int xpos, int ypos);
		void setFont(HFONT f);
		void setColor(COLORREF f, COLORREF b, bool tp);
		void drawText(HDC hdc);

		char text[256];	//Ҫ���ӵ��ַ���,֧�ֵ���
		int x,y;
		HFONT font;
		COLORREF fg,bg;		// ǰ��ɫ,����ɫ
		bool isTransparent;	//�Ƿ�͸��
	};
	
	OSD m_osd;

	// �ֲ���ʾ����
	RECT m_partRect;
	
	// �ڵ�����
	int m_showMask;
	RECT m_maskRect[MAX_MASK_COUNT];

	/***************************************************
	 * GDI ��ʾ��ʽ���
	 ***************************************************/
	struct RGBPlane {
        RGBPlane();
        ~RGBPlane();
        
        unsigned char * data;
        int w, h;
        
        void resize(int width,  int height);
        unsigned char * getLine(int y);

		void copy(
			unsigned char *py,unsigned char *pu,unsigned char *pv,
			int width, int height);

    }  m_rgb;
};

#endif /* DDRENDER_H */

