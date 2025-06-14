#include <thread>
#include <chrono>

#include "usleep_.h"

void usleep_(int us)
{
	std::this_thread::sleep_for(std::chrono::microseconds(us));
}


