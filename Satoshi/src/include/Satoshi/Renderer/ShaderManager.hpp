#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include "ShaderCompiler.hpp"
#include "Satoshi/Renderer/RendererAPI.hpp"

namespace Satoshi
{
	struct ShaderDetails
	{
		std::string_view Entrypoint;
		SHADER_KIND ShaderKind;

		ShaderDetails(SHADER_KIND shaderKind, std::string_view entrypoint = "main") :
			ShaderKind(shaderKind), Entrypoint(entrypoint)
		{

		}
	};
	class ShaderGroup
	{
	public:
		const std::vector<ShaderDetails>& GetShaderDetails() const { return m_ShadersDetails; }
		std::string_view GetBasePath() const { return m_BaseShaderPath; }
		SHADER_VERSION GetShaderVersion() const { return m_ShaderVersion; }

		ShaderGroup(std::string_view baseShaderPath, SHADER_VERSION shaderVersion, const std::initializer_list<ShaderDetails>& shadersDetails) :
			m_ShadersDetails(shadersDetails), m_ShaderVersion(shaderVersion), m_BaseShaderPath(baseShaderPath)
		{

		}

		ShaderGroup(std::string_view baseShaderName, GRAPHICS_API api, const std::initializer_list<ShaderDetails>& shadersDetails) :
			m_ShadersDetails(shadersDetails)
		{
			switch (api)
			{
				case Satoshi::GRAPHICS_API::GL4:
				{
					std::stringstream path;
					path << "assets/shaders/GLSL/" << baseShaderName;
					m_BaseShaderPath = path.str();
					m_ShaderVersion = SHADER_VERSION::GL_GLSL;
					break; 
				}
				case Satoshi::GRAPHICS_API::VK: 
				{
					std::stringstream path;
					path << "assets/shaders/GLSL/" << baseShaderName;
					m_BaseShaderPath = path.str();
					m_ShaderVersion = SHADER_VERSION::VK_GLSL;
					break;
				}
#ifdef ST_PLATFORM_WINDOWS
				case Satoshi::GRAPHICS_API::D3D11: 
				{
					std::stringstream path;
					path << "assets/shaders/HLSL/" << baseShaderName;
					m_BaseShaderPath = path.str();
					m_ShaderVersion = SHADER_VERSION::HLSL_5_1;
					break;
				}
				case Satoshi::GRAPHICS_API::D3D12: 
				{
					std::stringstream path;
					path << "assets/shaders/HLSL/" << baseShaderName;
					m_BaseShaderPath = path.str();
					m_ShaderVersion = SHADER_VERSION::HLSL_5_1;
					break;
				}
#endif
				default:
				{
					m_BaseShaderPath = "";
					m_ShaderVersion = SHADER_VERSION::HLSL_5_1;
					break; 
				}
			}
		}
	

	private:
		std::string m_BaseShaderPath;
		SHADER_VERSION m_ShaderVersion;
		std::vector<ShaderDetails> m_ShadersDetails;
	};

	class ShaderManager
	{
	public:
		virtual void CompileShaderFamily(const ShaderGroup& shaderGroup) = 0;
		virtual const std::string& BuildBlobFilename(std::string_view baseShaderPath, SHADER_KIND shaderKind, SHADER_VERSION shaderVersion) = 0;

		static ShaderManager* Create(GRAPHICS_API api);
	protected:
		static std::string s_Filename;
	};
}


#endif //SHADER_MANAGER_HPP
