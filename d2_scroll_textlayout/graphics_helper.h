#pragma once

//template to release resources
template<class T>
void safe_release(T** obj)
{
	if ((*obj) == nullptr) return;
	(*obj)->Release();
	(*obj) = nullptr;
}