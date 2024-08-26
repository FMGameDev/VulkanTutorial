#include "graphics/Shader.hpp"
#include <fstream>
#include <stdexcept>
#include <vector>
#include <iostream>

// Constructor: Load and create shader modules
Shader::Shader(VkDevice device, const std::string &vertFilePath, const std::string &fragFilePath)
    : m_device(device), m_vertexShaderModule(VK_NULL_HANDLE), m_fragmentShaderModule(VK_NULL_HANDLE)
{

    auto vertShaderCode = readFile(vertFilePath);
    auto fragShaderCode = readFile(fragFilePath);

    m_vertexShaderModule = createShaderModule(vertShaderCode);
    m_fragmentShaderModule = createShaderModule(fragShaderCode);
}

// Destructor: Clean up shader modules
Shader::~Shader()
{
    cleanUp();
}

// Load a file and return its contents as a byte array
std::vector<char> Shader::readFile(const std::string &filePath)
{
    // "ate": Start reading at the end of the file
    // Read the file as binary file (avoid text transformations)
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filePath + "\n" +
                                 "Current working directory: " + std::filesystem::current_path().c_str());
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    // Seek back to the beginning of the file and read all of the bytes at once.
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
    return buffer;
}

// Create a Vulkan shader module from bytecode
VkShaderModule Shader::createShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module!");
    }

    return shaderModule;
}

// Get Vulkan shader stage info for the vertex shader
VkPipelineShaderStageCreateInfo Shader::getVertexShaderStageInfo() const
{
    VkPipelineShaderStageCreateInfo vertexShaderStageInfo{};
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = m_vertexShaderModule;
    vertexShaderStageInfo.pName = "main";

    return vertexShaderStageInfo;
}

// Get Vulkan shader stage info for the fragment shader
VkPipelineShaderStageCreateInfo Shader::getFragmentShaderStageInfo() const
{
    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = m_fragmentShaderModule;
    fragmentShaderStageInfo.pName = "main";

    return fragmentShaderStageInfo;
}

// Clean up the shader modules
void Shader::cleanUp()
{
    if (m_vertexShaderModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_device, m_vertexShaderModule, nullptr);
        m_vertexShaderModule = VK_NULL_HANDLE;
    }

    if (m_fragmentShaderModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(m_device, m_fragmentShaderModule, nullptr);
        m_fragmentShaderModule = VK_NULL_HANDLE;
    }
}
