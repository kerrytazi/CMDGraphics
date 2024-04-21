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

	m_LastSize = GetScreenSize();
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

	if (m_ScreenSize.X != m_Gfx.m_LastSize.X || m_ScreenSize.Y != m_Gfx.m_LastSize.Y)
	{
		if (m_Gfx.m_BackBuffer == m_Gfx.m_SecondBuffer)
		{
			CloseHandle(m_Gfx.m_SecondBuffer);

			m_Gfx.m_SecondBuffer = CreateConsoleScreenBuffer(
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_WRITE | FILE_SHARE_READ,
				NULL,
				CONSOLE_TEXTMODE_BUFFER,
				NULL
			);

			m_Gfx.m_LastSize = m_ScreenSize;
		}
	}

	m_Gfx.m_FrameData.clear();
	m_Gfx.m_FrameData.resize(m_ScreenSize.X * m_ScreenSize.Y, L' ');
}

CMDGraphics::Frame::~Frame()
{
	SetConsoleCursorPosition(m_Gfx.m_BackBuffer, { 0, 0 });
	WriteConsoleW(m_Gfx.m_BackBuffer, m_Gfx.m_FrameData.data(), static_cast<DWORD>(m_Gfx.m_FrameData.size()), NULL, NULL);
	m_Gfx.SwapBuffers();
}

CMDGraphics::Vector CMDGraphics::Frame::Size()
{
	return { (size_t)m_ScreenSize.X, (size_t)m_ScreenSize.Y };
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, wchar_t c)
{
	if (x < (size_t)m_ScreenSize.X && y < (size_t)m_ScreenSize.Y)
	{
		m_Gfx.m_FrameData[y * m_ScreenSize.X + x] = c;
		return 1;
	}

	return 0;
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, wchar_t const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write(x + i, y, str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, wchar_t const *str)
{
	return Write(x, y, str, wcslen(str));
}

size_t CMDGraphics::Frame::Write(wchar_t c)
{
	if (c == L'\n')
	{
		m_Cursor.X = 0;
		++m_Cursor.Y;

		return 1;
	}

	if (Write(m_Cursor.X, m_Cursor.Y, c) == 1)
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

size_t CMDGraphics::Frame::Write(wchar_t const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write(str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(wchar_t const *str)
{
	return Write(str, wcslen(str));
}


size_t CMDGraphics::Frame::Write(size_t x, size_t y, char c)
{
	return Write(x, y, (wchar_t)c);
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, char const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write(x + i, y, (wchar_t)str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(size_t x, size_t y, char const *str)
{
	return Write(x, y, str, strlen(str));
}

size_t CMDGraphics::Frame::Write(char c)
{
	return Write((wchar_t)c);
}

size_t CMDGraphics::Frame::Write(char const *str, size_t strSize)
{
	size_t wrote = 0;

	for (size_t i = 0; i < strSize; ++i)
		wrote += Write((wchar_t)str[i]);

	return wrote;
}

size_t CMDGraphics::Frame::Write(char const *str)
{
	return Write(str, strlen(str));
}

