#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include "Core.hpp"
#include <spdlog/spdlog.h>
#include <string>
#include <memory>

template<typename... Args>
using format_string_t = std::string_view;

namespace Satoshi
{
    class SATOSHI_API Console
    {
    public:
        static void Init();
        static void End();

        template<typename... Args>
        static void CoreDebug(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void CoreLog(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void CoreWarn(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void CoreError(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void CoreCritical(format_string_t<Args...> fmt, Args &&... args)
        {
            s_CoreLogger->critical(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Debug(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->trace(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Log(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->info(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Warn(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->warn(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Error(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->error(fmt, std::forward<Args>(args)...);
        }

        template<typename... Args>
        static void Critical(format_string_t<Args...> fmt, Args &&... args)
        {
            s_ClientLogger->critical(fmt, std::forward<Args>(args)...);
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}


#endif //CONSOLE_HPP
