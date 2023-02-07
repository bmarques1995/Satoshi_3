#include "Satoshi/Core/ApplicationStarter.hpp"
#include "Satoshi/Utils/FileHandler.hpp"

json Satoshi::ApplicationStarter::s_JSONProperties;

const json& Satoshi::ApplicationStarter::GetStartupJson()
{
    return s_JSONProperties;
}

void Satoshi::ApplicationStarter::BuildStarter()
{
    std::string jsonContent;
    if (FileHandler::ReadTextFile("settings.json", &jsonContent) && PropertiesPassed())
        s_JSONProperties = json::parse(jsonContent);
    else
        BuildStandardStarter();
}

bool Satoshi::ApplicationStarter::PropertiesPassed()
{
    bool properties = true;
    properties = properties && (s_JSONProperties["GraphicsAPI"].dump() == "null");
    return properties;
}

void Satoshi::ApplicationStarter::BuildStandardStarter()
{
    s_JSONProperties["GraphicsAPI"] = "GL4";
    FileHandler::WriteTextFile("settings.json", s_JSONProperties.dump(4));
}