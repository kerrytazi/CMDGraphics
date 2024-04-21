#pragma once

#include <Windows.h>
#include <cstdlib>
#include <vector>
#include <format>

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

	COORD m_LastSizeFirst = {};
	COORD m_LastSizeSecond = {};
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

	size_t Write(size_t x, size_t y, wchar_t c);
	size_t Write(size_t x, size_t y, wchar_t const *str, size_t strSize);
	size_t Write(size_t x, size_t y, wchar_t const *str);

	size_t Write(wchar_t c);
	size_t Write(wchar_t const *str, size_t strSize);
	size_t Write(wchar_t const *str);

	size_t Write(size_t x, size_t y, char c);
	size_t Write(size_t x, size_t y, char const *str, size_t strSize);
	size_t Write(size_t x, size_t y, char const *str);

	size_t Write(char c);
	size_t Write(char const *str, size_t strSize);
	size_t Write(char const *str);


	template <class... _Types>
	size_t WriteF(std::wformat_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}

	template <class... _Types>
	size_t WriteF(size_t x, size_t y, std::wformat_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(x, y, out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}


	template <class... _Types>
	size_t WriteF(std::format_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}

	template <class... _Types>
	size_t WriteF(size_t x, size_t y, std::format_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(x, y, out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}

private:

	template <typename TFunc, typename TChar, class... _Types>
	size_t WriteFCommon(const TFunc &func, std::basic_format_string<TChar, const _Types &...> _Fmt, const _Types &... _Args)
	{
		const size_t maxBufferSize = 1024;
		TChar buff[maxBufferSize];

		TChar *out = buff;

		auto r = std::format_to_n(out, maxBufferSize - 1, _Fmt, _Args...);

		if (r.size + 1 > maxBufferSize)
		{
			out = (TChar *)malloc(r.size + 1);
			r = std::format_to_n(out, r.size, _Fmt, _Args...);
		}

		out[r.size] = (TChar)'\0';

		auto result = func(out, r.size);

		if (out != buff)
			free(out);

		return result;
	}

	Graphics &m_Gfx;
	COORD m_ScreenSize = {};
	COORD m_Cursor = {};
};


} // namespace CMDGraphics
