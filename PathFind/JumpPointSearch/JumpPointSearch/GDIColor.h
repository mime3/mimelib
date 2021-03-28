#ifndef __MINLIB_GDI_COLOR__
#define __MINLIB_GDI_COLOR__
#pragma once
#include <windows.h>
namespace MinLib
{
	class GDIColor
	{
	private:
		GDIColor();
		~GDIColor();
	protected:
	public:
		COLORREF _color[15];
		HPEN	_pen[15];
		HPEN	_nullPen;
		HBRUSH	_brush[15];
		HBRUSH  _nullBrush;
		HFONT	_font[5];
		static GDIColor* GetInstance();
	};

#define GDICOLOR()	GDIColor::GetInstance()
#define COLOR(i)	GDIColor::GetInstance()->_color[i]
#define PEN(i)		GDIColor::GetInstance()->_pen[i]
#define BRUSH(i)	GDIColor::GetInstance()->_brush[i]
#define FONT(i)		GDIColor::GetInstance()->_font[i]
}
#endif // !__MINLIB_GDI_COLOR__
