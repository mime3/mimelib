#include "GDIColor.h"
namespace MinLib
{
	GDIColor::GDIColor()
	{
		_color[0] = RGB(25, 25, 25);
		_color[1] = RGB(100, 100, 100);
		_color[2] = RGB(200, 200, 200);
		_color[3] = RGB(255, 255, 255);
		_color[4] = RGB(255, 94, 0);
		_color[5] = RGB(255, 187, 0);
		_color[6] = RGB(255, 228, 0);
		_color[7] = RGB(171, 242, 0);
		_color[8] = RGB(29, 219, 22);
		_color[9] = RGB(0, 216, 255);
		_color[10] = RGB(0, 84, 255);
		_color[11] = RGB(1, 0, 255);
		_color[12] = RGB(95, 0, 255);
		_color[13] = RGB(255, 0, 211);
		_color[14] = RGB(255, 0, 127);

		_nullPen = CreatePen(PS_NULL, 0, NULL);
		_nullBrush = CreateSolidBrush(NULL);

		for (int i = 0; i < 15; ++i)
		{
			_pen[i] = CreatePen(PS_SOLID, 2, _color[i]);
			_brush[i] = CreateSolidBrush(_color[i]);
		}

		_font[0] = CreateFont(15, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS | VARIABLE_PITCH, L"guide line font");
		_font[1] = CreateFont(20, 0, 0, 0, FW_MEDIUM, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS, L"2��");
		_font[2] = CreateFont(20, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS, L"3��");
		_font[3] = CreateFont(20, 0, 0, 0, FW_ULTRABOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SWISS, L"4��");
		_font[4] = CreateFont(20, 0, 0, 0, FW_HEAVY, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_SCRIPT | VARIABLE_PITCH, L"value in bar");
	}

	GDIColor::~GDIColor()
	{
		DeleteObject(_nullPen);
		DeleteObject(_nullBrush);
		for (int i = 0; i < 15; ++i)
		{
			DeleteObject(_pen[i]);
			DeleteObject(_brush[i]);
		}
		for (int i = 0; i < 5; ++i)
		{
			DeleteObject(_font[i]);
		}
	}

	GDIColor* GDIColor::GetInstance()
	{
		static GDIColor instance;
		return &instance;
	}
}