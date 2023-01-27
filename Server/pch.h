#pragma once

#define WIN32_LEAN_AND_MEAN

#include <coroutine>
#include <chrono>
#include <cstdint>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <ranges>
#include <semaphore>
#include <string>
#include <thread>
#include <vector>

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")
