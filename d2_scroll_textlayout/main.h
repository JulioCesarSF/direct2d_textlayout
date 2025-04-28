#pragma once
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")

#define NOMINMAX
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include <string>

//app
#include "d2_engine.h"

//win32 messages callback
LRESULT CALLBACK window_proc(HWND, UINT, WPARAM, LPARAM);

//dummy text to draw
const std::wstring text_to_draw = L"BEGIN\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nHello World\nEND";
