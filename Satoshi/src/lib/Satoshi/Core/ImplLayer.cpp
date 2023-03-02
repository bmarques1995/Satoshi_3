#include "Satoshi/Core/ImplLayer.hpp"

#include "Satoshi.hpp"

std::unordered_map<Satoshi::SHADER_KIND, GLenum> Satoshi::ImplLayer::s_NativeShaderEnums = 
{
    {SHADER_KIND::SHADER_KIND_VERTEX, GL_VERTEX_SHADER},
    {SHADER_KIND::SHADER_KIND_PIXEL, GL_FRAGMENT_SHADER},
    {SHADER_KIND::SHADER_KIND_GEOMETRY, GL_GEOMETRY_SHADER},
    {SHADER_KIND::SHADER_KIND_HULL, GL_TESS_CONTROL_SHADER},
    {SHADER_KIND::SHADER_KIND_DOMAIN, GL_TESS_EVALUATION_SHADER},
    {SHADER_KIND::SHADER_KIND_COMPUTE, GL_COMPUTE_SHADER}
};

Satoshi::ImplLayer::ImplLayer(const ShaderGroup& shaderGroup, const std::initializer_list<BufferElement>& elements) :
    m_Layout(elements), m_ShaderGroup(shaderGroup)
{
    auto currentAPI = Application::GetInstance()->GetCurrentAPI();
    //m_Shader.reset(Shader::Create(shaderGroup, elements, currentAPI));
    //m_VertexBuffer.reset(VertexBuffer::Create(vBuffer, sizeof(vBuffer), m_Shader->GetStride(), currentAPI));
    //m_IndexBuffer.reset(IndexBuffer::Create(iBuffer, sizeof(iBuffer)/sizeof(uint32_t), currentAPI));

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
 
    m_RawShader = LinkShaders(rawShaders);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vBuffer), vBuffer, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iBuffer), iBuffer, GL_STATIC_DRAW);

    auto nativeElements = m_Layout.GetElements();

    for (size_t i = 0; i < nativeElements.size(); i++)
    {
        glVertexAttribPointer(i, nativeElements[i].GetElementCount(), GL_FLOAT, GL_FALSE, m_Layout.GetStride(), (void*)nativeElements[i].Offset);
        glEnableVertexAttribArray(i);
    }
    

    
}

void Satoshi::ImplLayer::OnAttach()
{
}

void Satoshi::ImplLayer::OnDetach()
{
}

void Satoshi::ImplLayer::OnUpdate()
{
	if (Input::IsKeyPressed(ST_KEY_C))
		Console::Log("Key C was pressed");
    //UINT offset = 0;
    //m_VertexBuffer->Stage();
    //m_IndexBuffer->Stage();
    //m_Shader->Stage();
    glUseProgram(m_RawShader);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    
}

void Satoshi::ImplLayer::OnEvent(Event& event)
{
}

uint32_t Satoshi::ImplLayer::CompileShader(std::string_view filepath, GLenum shaderKind)
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

bool Satoshi::ImplLayer::GotCompileErrors(uint32_t shaderID)
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

uint32_t Satoshi::ImplLayer::LinkShaders(std::list<uint32_t> shaders)
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

bool Satoshi::ImplLayer::GotLinkErrors(uint32_t programID)
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
