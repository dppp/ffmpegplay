#ifndef DDOVERLAYRENDER_H
#define DDOVERLAYRENDER_H

#include "render.h"
#include "videorender.h"
#include "colorspace.h"

#include <windows.h>
#include <ddraw.h>
#include <stdio.h>

#define LPMYDIRECTDRAW			LPDIRECTDRAW7
#define IMyDirectDrawSurface	IDirectDrawSurface7
#define LPMYDIRECTDRAWSURFACE	LPDIRECTDRAWSURFACE7
#define MYDDSURFACEDESC			DDSURFACEDESC2
#define MYDDSCAPS				DDSCAPS2

class DDOverlayRender : public VideoRender
{
public:
	// ���캯��
    DDOverlayRender(DWORD color);

	// ��������
	virtual ~DDOverlayRender();

	// �ӿں���

	virtual int init(int index, HWND hWnd, int width, int height, draw_callback cb, long udata);
	virtual int clean();
    	virtual int render(unsigned char *py, unsigned char *pu, unsigned char *pv, int width, int height);
	virtual void resize();
	virtual int SetString(char * pcStr,
						  int x,
						  int y, 
						  COLORREF OSDColor, 
						  bool bTransparent,
						  COLORREF BackColor,
						  HFONT OSDFont);
	virtual int SetShowRect(int left, int top, int right, int bottom);
	virtual int SetMaskRect(int maskid, int left, int top, int right, int bottom);
	virtual int CleanMaskRect();
	virtual HWND GetWind();
private:
	// ��ʼ��DirectDraw
    int initDirectDraw();

	// ������ͼ����
    HRESULT createDrawSurface();

	// �������ֱ���, Ŀǰ������.
	// HRESULT createTextSurface(HWND hWnd, const char *text);

	// �ڲ�����
    HRESULT destroyDDObjects();

	// ����Ƿ�֧��Overlay
    BOOL hasOverLaySupport();

	// ����Ƿ�֧��FourCC (���ַ�����)
    BOOL hasFourCCSupport(LPMYDIRECTDRAWSURFACE lpdds);
    
	// ��ɫ��������
    DWORD colorMatch(IMyDirectDrawSurface * pdds, COLORREF rgb);
    DWORD convertGDIColor(COLORREF dwGDIColor);
	
	HRESULT createClipper(HWND hwnd);

	void adjustRect(RECT &rcSrc, RECT &rcDest);
	void ShowString();
private:
	// �����Ĳ���
	int				m_index;

	DWORD			m_colorKey;

	// ���ھ��
    HWND			m_hWnd;
	
	int				m_width;	// Ҫ�����Ļ�ͼ����Ŀ��
	int				m_height;	// Ҫ�����Ļ�ͼ����ĸ߶�
	
	draw_callback m_callback;
	long m_userData;

private: // DirectDraw ���
	// DirectDraw����
    LPMYDIRECTDRAW        m_pDD;						// DirectDraw����
    LPMYDIRECTDRAWSURFACE m_pDDSPrimary;				// ������

    LPMYDIRECTDRAWSURFACE m_pDDSVideoSurface;		// ��ͼ����
    LPMYDIRECTDRAWSURFACE m_pDDSVideoSurfaceBack;	// ��̨��ͼ����

	// Overlay��ʽ��Ҫʹ�õĲ���
    DDOVERLAYFX m_OverlayFX;
    DWORD m_dwOverlayFlags;
	DDBLTFX	m_ddbltfx;
	bool m_updataOverlay;
 
	// ��ͼ������Ϣ
    DDCAPS  m_ddCaps;
    
	// Overlay֧�ֱ�ʶ
    BOOL m_hasOverlaySupport;

	// FourCC֧�ֱ�ʶ
    BOOL m_hasFourCCSupport;

	color_convert_func m_colorConvert;

	//Ŀ�ı������������ϵ�λ��
	RECT m_destRect;
	RECT rcSrc;

	//��Ļ��͸�
	int	m_screenWidth;
	int	m_screenHeight;

	// �ֲ���ʾ����
	int m_showPart; // �ֲ��Ŵ�
	RECT m_partRect;
	
	int m_showMask;
	RECT m_maskRect[MAX_MASK_COUNT];
	
	unsigned char m_tempYuvData[704*576*3/2];
};

#endif /* DDOVERLAYRENDER_H */
