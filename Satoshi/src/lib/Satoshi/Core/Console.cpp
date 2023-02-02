#include "Satoshi/Core/Console.hpp"

std::shared_ptr<spdlog::logger> Satoshi::Console::s_CoreLogger;
std::shared_ptr<spdlog::logger> Satoshi::Console::s_ClientLogger;

#ifdef ST_PLATFORM_WINDOWS
#include <spdlog/sinks/msvc_sink.h>
#else
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

void Satoshi::Console::Init()
{
	std::vector<spdlog::sink_ptr> clientLogSinks;
	std::vector<spdlog::sink_ptr> coreLogSinks;

#ifdef ST_PLATFORM_WINDOWS
	clientLogSinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
	coreLogSinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#else
	clientLogSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	coreLogSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

	clientLogSinks[0]->set_pattern("[%l] %n: %v%$");
	coreLogSinks[0]->set_pattern("[core-%l] %n: %v%$");

	s_CoreLogger.reset(new spdlog::logger("Satoshi Core Console", begin(coreLogSinks), end(coreLogSinks)));
	spdlog::register_logger(s_CoreLogger);
	s_CoreLogger->set_level(spdlog::level::trace);
	s_CoreLogger->flush_on(spdlog::level::trace);

	s_ClientLogger.reset(new spdlog::logger("Satoshi Client Console", begin(clientLogSinks), end(clientLogSinks)));
	spdlog::register_logger(s_ClientLogger);
	s_ClientLogger->set_level(spdlog::level::trace);
	s_ClientLogger->flush_on(spdlog::level::trace);
}

void Satoshi::Console::End()
{
	s_CoreLogger.reset();
	s_ClientLogger.reset();
}
