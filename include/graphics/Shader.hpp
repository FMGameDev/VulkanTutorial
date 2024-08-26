#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class Shader
{
public:
    Shader(VkDevice device, const std::string &vertFilePath, const std::string &fragFilePath);
    ~Shader();

    VkPipelineShaderStageCreateInfo getVertexShaderStageInfo() const;
    VkPipelineShaderStageCreateInfo getFragmentShaderStageInfo() const;

    void cleanUp();

private:
    VkDevice m_device;
    VkShaderModule m_vertexShaderModule;
    VkShaderModule m_fragmentShaderModule;

    VkShaderModule createShaderModule(const std::vector<char> &code);
    std::vector<char> readFile(const std::string &filePath);
};
