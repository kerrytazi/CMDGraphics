#include "../CMDGraphics/CMDGraphics.hpp"

#include <chrono>
#include <thread>

#define CHAR_SQUARE L'\x2588'

int main()
{
	CMDGraphics::Graphics gfx;

	size_t counter = 0;

	while (true)
	{
		CMDGraphics::Frame frame(gfx);

		auto size = frame.Size();

		size_t dist = counter;

		for (size_t i = 0; i <= dist; ++i)
		{
			size_t x = i * 2;
			size_t y = dist - i;

			if (x < size.x && y < size.y)
			{
				frame.Write(x    , y, CHAR_SQUARE);
				frame.Write(x + 1, y, CHAR_SQUARE);
			}
		}

		frame.Write(6, 3, "Hello World!");

		frame.Write("hellofffffffffff\n");
		frame.Write("hellofffffffffff\n");
		frame.Write("hellofffffffffff\n");
		frame.Write("hellofffffffffff\n");
		frame.Write("hellofffffffffff\n");
		frame.Write("hellofffffffffff\n");

		++counter;

		if ((counter * 2) > (size.x + size.y + 24))
			counter = 0;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
}
