#pragma once

#ifdef IS_DEBUG
#define CORE_ENABLE_ASSERTS
#endif

#include "Platform/Platform.h"

#ifdef IS_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#ifdef IS_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef IS_DEBUG
#if defined(IS_PLATFORM_WINDOWS)
#define CORE_DEBUGBREAK() __debugbreak()
#elif defined(IS_PLATFORM_LINUX)
#include <signal.h>
#define CORE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define ENABLE_ASSERTS
#else
#define CORE_DEBUGBREAK()
#endif

#define CORE_EXPAND_MACRO(x) x
#define CORE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define CORE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Log.h"
#include "Assert.h"
#include "Debug/Instrumentor.h"

namespace Haste {}
using namespace Core;
using namespace Haste;
