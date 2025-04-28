#include "d2_engine.h"

std::unique_ptr<d2_engine_> d2_engine;

d2_engine_::d2_engine_(HWND window_)
{
	window = window_;
	if (!create_facs()) PostQuitMessage(3);
	if (!create_resources()) PostQuitMessage(4);
}

d2_engine_::~d2_engine_()
{
	destroy_resources();
	destroy_facs();
}

void d2_engine_::render()
{
	if (!create_resources()) return;
	render_target->BeginDraw();

	//1. clean background
	render_target->SetTransform(D2D1::IdentityMatrix()); //no rotations
	render_target->Clear(D2D1::ColorF(D2D1::ColorF::LightGray));

	const auto size = render_target->GetSize();

	text_layout_component_ text_layout_component;
	text_layout_component.render(size, render_target, dwrite_fac, p_text_format, text_to_draw, scroll_offset_y, p_black_brush);

	auto hr = render_target->EndDraw();
	if (FAILED(hr))
	{
		destroy_resources();
	}
}

void d2_engine_::resize(UINT32 w, UINT32 h)
{
	if (render_target == nullptr) return;
	const D2D1_SIZE_U size{ w, h };
	render_target->Resize(size);
}

void d2_engine_::set_wheel(const float wheel)
{
	scroll_offset_y -= (font_factor * (wheel / WHEEL_DELTA));
}

void d2_engine_::set_text_to_draw(const std::wstring text_to_draw_)
{
	text_to_draw = text_to_draw_;
}

bool d2_engine_::create_facs()
{
	auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2_fac);
	if (FAILED(hr)) return false;

	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwrite_fac));
	if (FAILED(hr)) return false;

	return true;
}

bool d2_engine_::create_resources()
{
	RECT rect = { 0 };
	if (GetClientRect(window, &rect) == FALSE) return false;

	const D2D1_SIZE_U size = D2D1::Size(static_cast<UINT32>(rect.right - rect.left), static_cast<UINT32>(rect.bottom - rect.top));
	auto hr = d2_fac->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(window, size), &render_target);
	if (FAILED(hr)) return false;

	hr = render_target->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &p_black_brush);
	if (FAILED(hr)) return false;

	hr = dwrite_fac->CreateTextFormat(L"Consolas", nullptr,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		default_font_size, L"en-use", &p_text_format);

	if (FAILED(hr)) return false;

	return true;
}

void d2_engine_::destroy_facs()
{
	safe_release(&d2_fac);
	safe_release(&dwrite_fac);
}

void d2_engine_::destroy_resources()
{
	safe_release(&p_black_brush);
	safe_release(&p_text_format);
	safe_release(&render_target);
}
