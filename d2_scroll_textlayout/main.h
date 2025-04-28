#pragma once
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

#define NOMINMAX
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include <string>

//template to release resources
template<class T>
void safe_release(T** obj)
{
	if ((*obj) == nullptr) return;
	(*obj)->Release();
	(*obj) = nullptr;
}

//win32 messages callback
LRESULT CALLBACK window_proc(HWND, UINT, WPARAM, LPARAM);

//direct2d facs
ID2D1Factory* d2_fac = nullptr;
IDWriteFactory* dwrite_fac = nullptr;

//direct2d resources
ID2D1HwndRenderTarget* render_target = nullptr;
ID2D1SolidColorBrush* p_black_brush = nullptr;
IDWriteTextFormat* p_text_format = nullptr;

//dummy text to draw
const std::wstring text_to_draw = L"BEGIN\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nEND";
const UINT32 text_size = static_cast<UINT32>(text_to_draw.size());

//defaults
const float default_font_size = 26.0f;
float font_factor = default_font_size * 1.2f;

//scroll
float scroll_offset_y = 0.0f;

bool create_facs()
{
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2_fac);
	if (FAILED(hr)) return false;

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwrite_fac));
	if (FAILED(hr)) return false;

	return true;
}

bool create_resources(HWND w)
{
	RECT rect = { 0 };
	if (GetClientRect(w, &rect) == FALSE) return false;

	const D2D1_SIZE_U size = D2D1::Size(static_cast<UINT32>(rect.right - rect.left), static_cast<UINT32>(rect.bottom - rect.top));
	auto hr = d2_fac->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(w, size), &render_target);
	if (FAILED(hr)) return false;

	hr = render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &p_black_brush);
	if (FAILED(hr)) return false;

	hr = dwrite_fac->CreateTextFormat(L"Consolas", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		default_font_size, L"en-use", &p_text_format);

	if (FAILED(hr)) return false;

	return true;
}

void destroy_facs()
{
	safe_release(&d2_fac);
	safe_release(&dwrite_fac);
}

void destroy_resources()
{
	safe_release(&p_black_brush);
	safe_release(&p_text_format);
	safe_release(&render_target);
}

void render(HWND w)
{
	if (!create_resources(w)) return;
	render_target->BeginDraw();

	//1. clean background
	render_target->SetTransform(D2D1::IdentityMatrix()); //no rotations
	render_target->Clear(D2D1::ColorF(D2D1::ColorF::LightGray));

	const auto size = render_target->GetSize();


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

	hr = render_target->EndDraw();
	if (FAILED(hr))
	{
		destroy_resources();
	}
}

void resize(UINT32 w, UINT32 h)
{
	if (render_target == nullptr) return;
	const D2D1_SIZE_U size{ w, h };
	render_target->Resize(size);
}