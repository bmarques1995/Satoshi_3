#include "Platform/Graphics/GL4/GL4Shader.hpp"
#include "Satoshi.hpp"

std::unordered_map<Satoshi::SHADER_KIND, GLenum> Satoshi::GL4Shader::s_NativeShaderEnums =
{
    {SHADER_KIND::SHADER_KIND_VERTEX, GL_VERTEX_SHADER},
    {SHADER_KIND::SHADER_KIND_PIXEL, GL_FRAGMENT_SHADER},
    {SHADER_KIND::SHADER_KIND_GEOMETRY, GL_GEOMETRY_SHADER},
    {SHADER_KIND::SHADER_KIND_HULL, GL_TESS_CONTROL_SHADER},
    {SHADER_KIND::SHADER_KIND_DOMAIN, GL_TESS_EVALUATION_SHADER},
    {SHADER_KIND::SHADER_KIND_COMPUTE, GL_COMPUTE_SHADER}
};

Satoshi::GL4Shader::GL4Shader(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements) :
    m_Layout(elements), m_ShaderGroup(shaderGroup)
{
    std::list<uint32_t> rawShaders;
    auto details = shaderGroup.GetShaderDetails();
    auto shaderManager = Application::GetInstance()->GetShaderManager();
    for (size_t i = 0; i < details.size(); i++)
    {
        //rawShaders.push_back(shaderManager->BuildBlobFilename(shaderGroup.GetBasePath(), details[i].ShaderKind, m_ShaderGroup.GetShaderVersion()), device);
        rawShaders.push_back
        (
            CompileShader
            (
                shaderManager->BuildBlobFilename(shaderGroup.GetBasePath(), details[i].ShaderKind, m_ShaderGroup.GetShaderVersion()),
                s_NativeShaderEnums[details[i].ShaderKind]
            )
        );
    }

    m_Shader = LinkShaders(rawShaders);
}

Satoshi::GL4Shader::~GL4Shader()
{
    glDeleteProgram(m_Shader);
}

void Satoshi::GL4Shader::Stage() const
{
    glUseProgram(m_Shader);
}

const uint32_t Satoshi::GL4Shader::GetOffset() const
{
	return 0;
}

const Satoshi::BufferLayout& Satoshi::GL4Shader::GetLayout() const
{
	return m_Layout;
}

uint32_t Satoshi::GL4Shader::CompileShader(std::string_view filepath, GLenum shaderKind)
{
    uint32_t shader = glCreateShader(shaderKind);
    std::string objShaderSource;
    FileHandler::ReadTextFile(filepath, &objShaderSource);
    const char* shaderSource = objShaderSource.c_str();
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);
    assert(!GotCompileErrors(shader));
    return shader;
}

bool Satoshi::GL4Shader::GotCompileErrors(uint32_t shaderID)
{
    GLint isCompiled = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled != GL_TRUE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);

        Console::CoreCritical("{0}", infoLog.data());
        return 0;
    }

    return isCompiled != GL_TRUE;
}

uint32_t Satoshi::GL4Shader::LinkShaders(std::list<uint32_t> shaders)
{
    GLuint program = glCreateProgram();

    for (auto& shader : shaders)
        glAttachShader(program, shader);

    glLinkProgram(program);
    assert(!GotLinkErrors(program));

    for (auto& shader : shaders)
        glDeleteShader(shader);

    return program;
}

bool Satoshi::GL4Shader::GotLinkErrors(uint32_t programID)
{
    int isLinked;
    glGetProgramiv(programID, GL_LINK_STATUS, &isLinked);

    if (isLinked != GL_TRUE)
    {
        GLint maxLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

        Console::CoreCritical("{0}", infoLog.data());
    }

    return isLinked != GL_TRUE;
}
