#ifndef GDIRENDER_H
#define GDIRENDER_H

#include "videorender.h"

#include <windows.h>

class GDIRender : public VideoRender
{
public:
	GDIRender();
	virtual ~GDIRender();

	virtual int init(int index, HWND hWnd, int width, int height, draw_callback cb, long udata);
	virtual int clean();
	virtual int render(unsigned char *py, unsigned char *pu, unsigned char *pv, int width, int height);
	virtual void resize();
	virtual BOOL SetString(char * pcStr,
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
	inline void colorConvert(
		unsigned char *py, 
		unsigned char *pu, 
		unsigned char *pv, 
		int width, int height);

	void ShowString(HDC hDC);

private:
	int m_index;
	HWND m_hWnd;
	draw_callback m_cb;
	long m_userData;

	bool m_flip;
	bool m_rgbReverseOrder;

	struct RGBPlane 
	{
        RGBPlane();
        ~RGBPlane();
        
        unsigned char * data;
        int w, h;
        
        void resize(int width,  int height);
        unsigned char * getLine(int y);
    }  m_rgb;

	//OSD����
	char m_pOSDString[256];//Ҫ���ӵ��ַ���
	int m_OSDStringX;//��ʼx����
	int m_OSDStringY;//��ʼy����
	HFONT m_OSDFont;//����
	COLORREF m_OSDColor;//��ɫ
	COLORREF m_BackColor;//����ɫ
	bool m_bTransparent;//�Ƿ�͸��
	HFONT m_FontPre;//����ǰ���������ʽ
	
	// �ֲ���ʾ����
	RECT m_partRect;
	
	int m_showMask;
	RECT m_maskRect[MAX_MASK_COUNT];
};

#endif /* GDIRENDER_H */
