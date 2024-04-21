#pragma once

#include <format>

template <size_t N = 128>
struct BufferedCStr
{
	char _buff[N];
	char *_str;
	size_t _size;

	constexpr bool is_allocated() const { return _str != _buff; }

	constexpr BufferedCStr(char const *str_, size_t size_)
	{
		_size = size_;

		_str = _buff;

		if (_size + 1 > N)
			_str = (char *)malloc(_size + 1);

		for (size_t i = 0; i < _size; ++i)
			_str[i] = (char)str_[i];

		_str[_size] = '\0';
	}

	constexpr BufferedCStr(wchar_t const *str_, size_t size_)
	{
		_size = size_;

		_str = _buff;

		if (_size + 1 > N)
			_str = (char *)malloc(_size + 1);

		for (size_t i = 0; i < _size; ++i)
			_str[i] = (char)str_[i];

		_str[_size] = '\0';
	}

	BufferedCStr(BufferedCStr &other) = delete;
	BufferedCStr &operator = (BufferedCStr &other) = delete;

	constexpr BufferedCStr(BufferedCStr &&other)
	{
		_size = other._size;

		if (other.is_allocated())
		{
			_str = other._str;
			other._str = nullptr;
		}
		else
		{
			_str = _buff;

			for (size_t i = 0; i < _size; ++i)
				_str[i] = other._buff[i];

			_str[_size] = '\0';
		}
	}
	constexpr BufferedCStr &operator = (BufferedCStr &&other)
	{
		_size = other._size;

		if (other.is_allocated())
		{
			if (is_allocated())
			{
				std::swap(_str, _str);
			}
			else
			{
				_str = other._str;
				other._str = nullptr;
			}
		}
		else
		{
			if (is_allocated())
				free(_str);

			_str = _buff;

			for (size_t i = 0; i < _size; ++i)
				_str[i] = other._buff[i];

			_str[_size] = '\0';
		}

		return *this;
	}

	constexpr ~BufferedCStr()
	{
		if (is_allocated())
			free(_str);
	}

	[[nodiscard]] constexpr char const *c_str() const { return _str; }

	template <class... _Types>
	[[nodiscard]]
	static constexpr BufferedCStr format(std::format_string<const _Types &...> _Fmt, const _Types &... _Args)
	{
		BufferedCStr result((char const *)nullptr, 0);

		result._str = result._buff;
		auto r = std::format_to_n(result._str, (size_t)N - 1, _Fmt, _Args...);

		result._size = r.size;

		if (result._size + 1 > N)
		{
			result._str = (char *)malloc(result._size + 1);
			auto r = std::format_to_n(result._str, (size_t)N - 1, _Fmt, _Args...);
		}

		result._str[result._size] = '\0';

		return result;
	}
};
