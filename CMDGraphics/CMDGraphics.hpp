#pragma once

#include <Windows.h>
#include <vector>

namespace CMDGraphics
{


class Graphics
{
	friend class Frame;

public:

	Graphics();
	~Graphics();

private:

	void SwapBuffers();
	COORD GetScreenSize();

	std::vector<wchar_t> m_FrameData;

	HANDLE m_FirstBuffer = NULL;
	HANDLE m_SecondBuffer = NULL;
	HANDLE m_BackBuffer = NULL;

	COORD m_LastSize = {};
};

struct Vector
{
	size_t x;
	size_t y;
};

class Frame
{
public:

	Frame(Graphics &gfx);
	~Frame();

	Vector Size();

	void Clear();

	size_t Write(size_t x, size_t y, wchar_t c);
	size_t Write(size_t x, size_t y, wchar_t const *str, size_t strSize);

private:

	Graphics &m_Gfx;
	COORD m_ScreenSize = {};
};


} // namespace CMDGraphics