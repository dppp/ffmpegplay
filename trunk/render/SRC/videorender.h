 #ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include "render.h"

#define MAX_MASK_COUNT 32

struct VideoRender
{
	/*!
	 * hWnd: �����Ĵ��ھ��
	 * width, height: һ��Ϊ��Ƶͼ��Ŀ����
	 * cb: ��ͼ�ص�, ��Ҫ��ͳһ�������.
	 */
	virtual int init(int index, HWND hWnd, int width, int height, draw_callback cb, long udata) = 0;
	virtual int clean() = 0;
    virtual int render(unsigned char *py, unsigned char *pu, unsigned char *pv, int width, int height) = 0;
	virtual void resize() = 0;
	virtual int SetString(char * pcStr,
						  int x,
						  int y, 
						  COLORREF OSDColor, 
						  bool bTransparent,
						  COLORREF BackColor,
						  HFONT OSDFont) = 0;
	virtual int SetShowRect(int left, int top, int right, int bottom) = 0;
	virtual int SetMaskRect(int maskid, int left, int top, int right, int bottom) = 0;
	virtual int CleanMaskRect()=0;
	virtual HWND GetWind() = 0;
};


#endif // VIDEORENDER_H
