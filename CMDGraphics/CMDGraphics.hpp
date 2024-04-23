#pragma once

#include <Windows.h>
#include <cstdlib>
#include <vector>
#include <format>

namespace CMDGraphics
{

class Color
{
public:

	enum class Foreground : WORD
	{
		Black             = 0,
		DarkBlue          = FOREGROUND_BLUE,
		DarkGreen         = FOREGROUND_GREEN,
		DarkCyan          = FOREGROUND_GREEN | FOREGROUND_BLUE,
		DarkRed           = FOREGROUND_RED,
		DarkMagenta       = FOREGROUND_RED | FOREGROUND_BLUE,
		DarkYellow        = FOREGROUND_RED | FOREGROUND_GREEN,
		DarkGray          = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		Gray              = FOREGROUND_INTENSITY,
		Blue              = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
		Green             = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		Cyan              = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
		Red               = FOREGROUND_INTENSITY | FOREGROUND_RED,
		Magenta           = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
		Yellow            = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
		White             = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	};

	enum class Background : WORD
	{
		Black             = 0,
		DarkBlue          = BACKGROUND_BLUE,
		DarkGreen         = BACKGROUND_GREEN,
		DarkCyan          = BACKGROUND_GREEN | BACKGROUND_BLUE,
		DarkRed           = BACKGROUND_RED,
		DarkMagenta       = BACKGROUND_RED | BACKGROUND_BLUE,
		DarkYellow        = BACKGROUND_RED | BACKGROUND_GREEN,
		DarkGray          = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
		Gray              = BACKGROUND_INTENSITY,
		Blue              = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
		Green             = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
		Cyan              = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
		Red               = BACKGROUND_INTENSITY | BACKGROUND_RED,
		Magenta           = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
		Yellow            = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
		White             = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	};

	Color() = default;

	Color(Foreground const &Other)
		: m_Value(static_cast<WORD>(Other) | static_cast<WORD>(Background::Black))
	{
	}

	Color(Background const &Other)
		: m_Value(static_cast<WORD>(Other) | static_cast<WORD>(Foreground::White))
	{
	}

	Color &operator |= (Color const &Other)
	{
		m_Value |= Other.m_Value;
		return *this;
	}

	Color operator | (Color const &Other) const
	{
		return Color(*this) |= Other;
	}

	bool operator == (Color const &Other) const
	{
		return m_Value == Other.m_Value;
	}

	friend static Color::Foreground &operator |= (Color::Foreground &Left, Color const &Right);
	friend static Color::Foreground operator | (Color::Foreground const &Left, Color const &Right);
	friend static Color::Background &operator |= (Color::Background &Left, Color const &Right);
	friend static Color::Background operator | (Color::Background const &Left, Color const &Right);

	WORD GetInner() const { return m_Value; }

private:

	WORD m_Value = static_cast<WORD>(Foreground::White) | static_cast<WORD>(Background::Black);
};

static Color::Foreground &operator |= (Color::Foreground &Left, Color::Foreground const &Right)
{
	*reinterpret_cast<WORD *>(&Left) |= *reinterpret_cast<WORD const *>(&Right);
	return Left;
}

static Color::Foreground operator | (Color::Foreground const &Left, Color::Foreground const &Right)
{
	return static_cast<Color::Foreground>(static_cast<WORD>(Left) | static_cast<WORD>(Right));
}

static Color::Background &operator |= (Color::Background &Left, Color::Background const &Right)
{
	*reinterpret_cast<WORD *>(&Left) |= *reinterpret_cast<WORD const *>(&Right);
	return Left;
}

static Color::Background operator | (Color::Background const &Left, Color::Background const &Right)
{
	return static_cast<Color::Background>(static_cast<WORD>(Left) | static_cast<WORD>(Right));
}

static Color::Foreground &operator |= (Color::Foreground &Left, Color::Background const &Right)
{
	*reinterpret_cast<WORD *>(&Left) |= *reinterpret_cast<WORD const *>(&Right);
	return Left;
}

static Color::Foreground operator | (Color::Foreground const &Left, Color::Background const &Right)
{
	return static_cast<Color::Foreground>(static_cast<WORD>(Left) | static_cast<WORD>(Right));
}

static Color::Background &operator |= (Color::Background &Left, Color::Foreground const &Right)
{
	*reinterpret_cast<WORD *>(&Left) |= *reinterpret_cast<WORD const *>(&Right);
	return Left;
}

static Color::Background operator | (Color::Background const &Left, Color::Foreground const &Right)
{
	return static_cast<Color::Background>(static_cast<WORD>(Left) | static_cast<WORD>(Right));
}

static Color::Foreground &operator |= (Color::Foreground &Left, Color const &Right)
{
	*reinterpret_cast<WORD *>(&Left) |= Right.m_Value;
	return Left;
}

static Color::Foreground operator | (Color::Foreground const &Left, Color const &Right)
{
	return static_cast<Color::Foreground>(static_cast<WORD>(Left) | Right.m_Value);
}

static Color::Background &operator |= (Color::Background &Left, Color const &Right)
{
	*reinterpret_cast<WORD *>(&Left) |= Right.m_Value;
	return Left;
}

static Color::Background operator | (Color::Background const &Left, Color const &Right)
{
	return static_cast<Color::Background>(static_cast<WORD>(Left) | Right.m_Value);
}


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
	std::vector<Color> m_FrameColorData;

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

	size_t Write(size_t x, size_t y, Color color, wchar_t c);
	size_t Write(size_t x, size_t y, Color color, wchar_t const *str, size_t strSize);
	size_t Write(size_t x, size_t y, Color color, wchar_t const *str);

	size_t Write(Color color, wchar_t c);
	size_t Write(Color color, wchar_t const *str, size_t strSize);
	size_t Write(Color color, wchar_t const *str);

	size_t Write(size_t x, size_t y, Color color, char c);
	size_t Write(size_t x, size_t y, Color color, char const *str, size_t strSize);
	size_t Write(size_t x, size_t y, Color color, char const *str);

	size_t Write(Color color, char c);
	size_t Write(Color color, char const *str, size_t strSize);
	size_t Write(Color color, char const *str);


	size_t Write(size_t x, size_t y, wchar_t c) { return Write(x, y, Color{}, c); }
	size_t Write(size_t x, size_t y, wchar_t const *str, size_t strSize) { return Write(x, y, Color{}, str, strSize); }
	size_t Write(size_t x, size_t y, wchar_t const *str) { return Write(x, y, Color{}, str); }

	size_t Write(wchar_t c) { return Write(Color{}, c); }
	size_t Write(wchar_t const *str, size_t strSize) { return Write(Color{}, str, strSize); }
	size_t Write(wchar_t const *str) { return Write(Color{}, str); }

	size_t Write(size_t x, size_t y, char c) { return Write(x, y, Color{}, c); }
	size_t Write(size_t x, size_t y, char const *str, size_t strSize) { return Write(x, y, Color{}, str, strSize); }
	size_t Write(size_t x, size_t y, char const *str) { return Write(x, y, Color{}, str); }

	size_t Write(char c) { return Write(Color{}, c); }
	size_t Write(char const *str, size_t strSize) { return Write(Color{}, str, strSize); }
	size_t Write(char const *str) { return Write(Color{}, str); }


	template <class... _Types>
	size_t WriteF(Color color, std::wformat_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(color, out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}
	template <class... _Types>
	size_t WriteF(size_t x, size_t y, Color color, std::wformat_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(x, y, color, out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}
	template <class... _Types>
	size_t WriteF(Color color, std::format_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(color, out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}
	template <class... _Types>
	size_t WriteF(size_t x, size_t y, Color color, std::format_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		const auto func = [&](char const *out, size_t size) {
			return Write(x, y, color, out, size);
		};
		return WriteFCommon(func, _Fmt, _Args...);
	}


	template <class... _Types>
	size_t WriteF(std::wformat_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		return WriteF(Color{}, _Fmt, _Args...);
	}
	template <class... _Types>
	size_t WriteF(size_t x, size_t y, std::wformat_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		return WriteF(x, y, Color{}, _Fmt, _Args...);
	}
	template <class... _Types>
	size_t WriteF(std::format_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		return WriteF(Color{}, _Fmt, _Args...);
	}
	template <class... _Types>
	size_t WriteF(size_t x, size_t y, std::format_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		return WriteF(x, y, Color{}, _Fmt, _Args...);
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
