#pragma once

#define NOMINMAX
#include <d2d1.h>
#include <dwrite.h>

#include <string>

struct text_layout_component_
{
	const float default_font_size = 26.0f;
	float font_factor = default_font_size * 1.2f;

	void render(const D2D1_SIZE_F size, ID2D1HwndRenderTarget* render_target, IDWriteFactory* dwrite_fac, IDWriteTextFormat* p_text_format,
		const std::wstring text_to_draw, float& scroll_offset_y, ID2D1SolidColorBrush* p_black_brush)
	{
		const UINT32 text_size = static_cast<UINT32>(text_to_draw.size());

		//to create a text "region" clip it
		D2D1_RECT_F clip_rect = { 0.0f, 0.0f, size.width, size.height };
		render_target->PushAxisAlignedClip(clip_rect, D2D1_ANTIALIAS_MODE_ALIASED);

		//2. Create a text layout
		IDWriteTextLayout* p_text_layout = nullptr;
		auto hr = dwrite_fac->CreateTextLayout(text_to_draw.c_str(), text_size, p_text_format, size.width, clip_rect.bottom, &p_text_layout);
		if (FAILED(hr)) return;

		//clamp scroll
		//1. update factor to the line height
		DWRITE_TEXT_METRICS text_metric = { 0 };
		hr = p_text_layout->GetMetrics(&text_metric);
		if (FAILED(hr)) return;
		font_factor = text_metric.height / text_metric.lineCount;
		if (text_metric.height > clip_rect.bottom)
		{
			const auto number_of_lines = std::count(text_to_draw.begin(), text_to_draw.end(), '\n');
			const auto content_height = number_of_lines * font_factor;
			const auto max_scroll = std::max(0.0f, content_height - clip_rect.bottom);
			scroll_offset_y = std::min(scroll_offset_y, max_scroll + font_factor); //bottom
			scroll_offset_y = std::max(scroll_offset_y, 0.0f); //top
		}
		else
		{
			scroll_offset_y = 0.0f;
		}

		//3. Draw with scroll to test
		D2D1_POINT_2F origin = { 0.0f, -scroll_offset_y }; // y (up/down / vertical)
		render_target->DrawTextLayout(origin, p_text_layout, p_black_brush);

		safe_release(&p_text_layout);
		render_target->PopAxisAlignedClip();
	}
};