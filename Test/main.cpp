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

		{
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

			++counter;

			if ((counter * 2) > (size.x + size.y + 24))
				counter = 0;
		}

		{
			frame.Write(6, 3, "Hello World!");

			frame.Write("hellofffffffffff\n");

			frame.WriteF("hello {}\n", 123);
		}

		{
			using Color = CMDGraphics::Color;

			Color a = Color::Foreground::Red | Color(Color::Background::Yellow);
			Color b = Color(Color::Background::Yellow) | Color::Foreground::Red;

			using FGColor = Color::Foreground;
			using BGColor = Color::Background;

			Color c = BGColor::Yellow | FGColor::Red;
			Color d = FGColor::Red | BGColor::Yellow;

			frame.Write(10, 10, FGColor::Red | BGColor::Yellow, L'H');
			frame.Write(FGColor::Red | BGColor::Yellow, "Hello");
			frame.WriteF(FGColor::Red | BGColor::Yellow, "Hello {} {} {}", 123, "World", 3.3);
		}

		{
			frame.Write(L"\n\n\x2560\x2566\x2550\x2563\n");
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
	}
}
