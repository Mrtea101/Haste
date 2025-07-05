#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <queue>
#include <array>

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#include "spdlog/common.h"
#include "spdlog/details/log_msg.h"
#include "spdlog/sinks/base_sink.h"
#pragma warning(pop)

#include "Pointers.h"

namespace Core {

	class Log
	{
	public:
		static void Init(const char* name);

		static Ref<spdlog::logger>& GetLogger() { return s_Logger; }
	private:
		static Ref<spdlog::logger> s_Logger;
	};
}

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

template <typename... Args>
inline std::string FormatString(const char* fmt, const Args & ... args)
{
	if (!sizeof...(Args)) return std::string(fmt);
	fmt::memory_buffer buf;
	fmt::format_to(buf, fmt, args...);
	return std::string(buf.data(), buf.size());
}

template <typename... Args>
inline std::string FormatString(const std::string& fmt, const Args & ... args)
{
	if (!sizeof...(Args)) return fmt;
	fmt::memory_buffer buf;
	fmt::format_to(buf, fmt.c_str(), args...);
	return std::string(buf.data(), buf.size());
}

// Core log macros
#define CORE_TRACE(...)    ::Core::Log::GetLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)     ::Core::Log::GetLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)     ::Core::Log::GetLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)    ::Core::Log::GetLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...) ::Core::Log::GetLogger()->critical(__VA_ARGS__)

// CORE_WARN but with condition
#define CORE_WARN_IF(check, ...) { if(!(check)) { CORE_WARN(__VA_ARGS__); } }

// (throws for all build configs)

#define CORE_THROW(...) do {								\
	const std::string fmt = FormatString(__VA_ARGS__);	\
	CORE_WARN(fmt);										\
	throw std::exception((fmt).c_str());				\
} while (0);

#define CORE_ENSURE(check, ...) do {						\
if(!(check)) { CORE_THROW(__VA_ARGS__) }					\
} while (0);									
