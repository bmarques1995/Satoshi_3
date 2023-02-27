#include "Satoshi/Core/ApplicationStarter.hpp"
#include "Satoshi/Utils/FileHandler.hpp"

Json::Value Satoshi::ApplicationStarter::s_JSONProperties;

const Json::Value& Satoshi::ApplicationStarter::GetStartupJson()
{
    return s_JSONProperties;
}

void Satoshi::ApplicationStarter::BuildStarter()
{
    Json::Reader reader;
    std::string jsonContent;
    FileHandler::ReadTextFile("settings.json", &jsonContent);
    reader.parse(jsonContent, s_JSONProperties);
    if (!PropertiesPassed())
        BuildStandardStarter();
}

bool Satoshi::ApplicationStarter::PropertiesPassed()
{
    bool properties = true;
    auto value = s_JSONProperties["GraphicsAPI"].asString();
    properties = properties && (s_JSONProperties["GraphicsAPI"].asString() != "");
    return properties;
}

void Satoshi::ApplicationStarter::BuildStandardStarter()
{
    s_JSONProperties["GraphicsAPI"] = "GL4";
    FileHandler::WriteTextFile("settings.json", s_JSONProperties.toStyledString());
}