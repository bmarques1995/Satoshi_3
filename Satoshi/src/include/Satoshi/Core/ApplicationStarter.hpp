#ifndef APPLICATION_STARTER_HPP
#define APPLICATION_STARTER_HPP

#include <json/json.h>

namespace Satoshi
{
    class ApplicationStarter
    {
	public:
		static const Json::Value& GetStartupJson();
		static void BuildStarter();
	private:
		static Json::Value s_JSONProperties;

		static bool PropertiesPassed();
		static void BuildStandardStarter();
    };
}


#endif //APPLICATION_STARTER_HPP
