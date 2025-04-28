#pragma once

#define NOMINMAX
#include <d2d1.h>
#include <dwrite.h>

#include <memory>

#include "graphics_helper.h"
#include "text_layout_component.h"

class d2_engine_
{
public:

	d2_engine_(HWND window_);
	~d2_engine_();

	void render();
	void resize(UINT32 w, UINT32 h);

	void set_wheel(const float wheel);
	void set_text_to_draw(const std::wstring text_to_draw_);
private:
	HWND window = nullptr;

	//text
	std::wstring text_to_draw;

	//defaults
	const float default_font_size = 26.0f;
	float font_factor = default_font_size * 1.2f;

	//scroll
	float scroll_offset_y = 0.0f;

	//direct2d facs
	ID2D1Factory* d2_fac = nullptr;
	IDWriteFactory* dwrite_fac = nullptr;

	//direct2d resources
	ID2D1HwndRenderTarget* render_target = nullptr;
	ID2D1SolidColorBrush* p_black_brush = nullptr;
	IDWriteTextFormat* p_text_format = nullptr;

	bool create_facs();
	bool create_resources();
	void destroy_facs();
	void destroy_resources();
};

extern std::unique_ptr<d2_engine_> d2_engine;