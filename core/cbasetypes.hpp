#ifndef CBASETYPES_HPP
#define CBASETYPES_HPP

#include <cstdint>
#include <cstddef>

#if defined(_WIN32) || defined(_WIN64)
#define PLATFORM_WINDOWS
#elif defined(__linux__)
#define PLATFORM_LINUX
#elif defined(__APPLE__)
#define PLATFORM_MACOS
#endif

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using opcode_t = uint16;

#define ARRAYLENGTH(A) (sizeof(A) / sizeof((A)[0]))

template<typename T>
constexpr T tmin(T a, T b) { return (a < b) ? a : b; }

#endif