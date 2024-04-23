#include "CMDGraphics.hpp"

#include <cstring>

CMDGraphics::Graphics::Graphics()
{
	m_FirstBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);

	m_SecondBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);

	m_BackBuffer = m_SecondBuffer;

	m_LastSizeFirst = GetScreenSize();
	m_LastSizeSecond = GetScreenSize();

	setlocale(LC_ALL, "en_US.utf8");
}

CMDGraphics::Graphics::~Graphics()
{
	CloseHandle(m_FirstBuffer);
	CloseHandle(m_SecondBuffer);
}

void CMDGraphics::Graphics::SwapBuffers()
{
	SetConsoleActiveScreenBuffer(m_BackBuffer);
	m_BackBuffer = m_BackBuffer == m_FirstBuffer ? m_SecondBuffer : m_FirstBuffer;
}

COORD CMDGraphics::Graphics::GetScreenSize()
{
	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(m_FirstBuffer, &bufferInfo);
	const auto newScreenWidth = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;
	const auto newscreenHeight = bufferInfo.srWindow.Bottom - bufferInfo.srWindow.Top + 1;

	return COORD{ static_cast<short>(newScreenWidth), static_cast<short>(newscreenHeight) };
}


CMDGraphics::Frame::Frame(Graphics &gfx)
	: m_Gfx{ gfx }
{
	m_ScreenSize = m_Gfx.GetScreenSize();

	/*if (m_Gfx.m_BackBuffer == m_Gfx.m_SecondBuffer && (m_ScreenSize.X != m_Gfx.m_LastSizeSecond.X || m_ScreenSize.Y != m_Gfx.m_LastSizeSecond.Y))
	{
		CloseHandle(m_Gfx.m_SecondBuffer);

		m_Gfx.m_SecondBuffer = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
		);

		m_Gfx.m_LastSizeSecond = m_ScreenSize;
	}

	if (m_Gfx.m_BackBuffer == m_Gfx.m_FirstBuffer && (m_ScreenSize.X != m_Gfx.m_LastSizeFirst.X || m_ScreenSize.Y != m_Gfx.m_LastSizeFirst.Y))
	{
		CloseHandle(m_Gfx.m_FirstBuffer);

		m_Gfx.m_FirstBuffer = CreateConsoleScreenBuffer(
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			NULL,
			CONSOLE_TEXTMODE_BUFFER,
			NULL
		);

		m_Gfx.m_LastSizeFirst = m_ScreenSize;
	}*/

	m_Gfx.m_FrameData.clear();
	m_Gfx.m_FrameData.resize(m_ScreenSize.X * m_ScreenSize.Y, L' ');

	m_Gfx.m_FrameColorData.clear();
	m_Gfx.m_FrameColorData.resize(m_ScreenSize.X * m_ScreenSize.Y);
}

CMDGraphics::Frame::~Frame()
{
	SetConsoleCursorPosition(m_Gfx.m_BackBuffer, { 0, 0 });

	Color color;
	size_t i = 0;

	while (i < m_Gfx.m_FrameData.size())
	{
		Color lastColor = color;
		size_t lastI = i;

		for (; i < m_Gfx.m_FrameData.size(); ++i)
		{
			if (m_Gfx.m_FrameColorData[i] != lastColor)
			{
				lastColor = m_Gfx.m_FrameColorData[i];
				break;
			}
		}

		SetConsoleTextAttribute(m_Gfx.m_BackBuffer, color.GetInner());
		WriteConsoleW(m_Gfx.m_BackBuffer, m_Gfx.m_FrameData.data() + lastI, static_cast<DWORD>(i - lastI), NULL, NULL);

		color = lastColor;
	}

	m_Gfx.SwapBuffers();
}

CMDGraphics::Vector CMDGraphics::Frame::Size()
{
	return { (size_t)m_ScreenSize.X, (size_t)m_ScreenSize.Y };
}


size_t CMDGraphics::Frame::Write(size_t x, size_t y, Color color, wchar_t c)
{
	if (x < (size_t)m_ScreenSize.X && y < (size_t)m_ScreenSize.Y)
	{
		m_Gfx.m_FrameData[y * m_ScreenSize.X + x] = c;
		m_Gfx.m_FrameColorData[y * m_ScreenSize.X + x] = color;
		return 1;
	}

	return 0;
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, Color color, wchar_t const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write(x + i, y, color, str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, Color color, wchar_t const *str)
{
	return Write(x, y, color, str, wcslen(str));
}

size_t CMDGraphics::Frame::Write(Color color, wchar_t c)
{
	if (c == L'\n')
	{
		m_Cursor.X = 0;
		++m_Cursor.Y;

		return 1;
	}

	if (Write(m_Cursor.X, m_Cursor.Y, color, c) == 1)
	{
		if (++m_Cursor.X == m_ScreenSize.X)
		{
			m_Cursor.X = 0;
			++m_Cursor.Y;
		}

		return 1;
	}

	return 0;
}

size_t CMDGraphics::Frame::Write(Color color, wchar_t const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write(color, str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(Color color, wchar_t const *str)
{
	return Write(color, str, wcslen(str));
}


size_t CMDGraphics::Frame::Write(size_t x, size_t y, Color color, char c)
{
	return Write(x, y, color, (wchar_t)c);
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, Color color, char const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write(x + i, y, color, (wchar_t)str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, Color color, char const *str)
{
	return Write(x, y, color, str, strlen(str));
}

size_t CMDGraphics::Frame::Write(Color color, char c)
{
	return Write(color, (wchar_t)c);
}

size_t CMDGraphics::Frame::Write(Color color, char const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write(color, (wchar_t)str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(Color color, char const *str)
{
	return Write(color, str, strlen(str));
}

