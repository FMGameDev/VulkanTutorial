# CMakeLists.txt for shaders

cmake_minimum_required(VERSION 3.28)

# GLSL shader compiler
find_program(GLSLC_EXECUTABLE NAMES glslc HINTS ENV PATH REQUIRED)

# Output GLSLC_EXECUTABLE path for verification
message(STATUS "GLSLC_EXECUTABLE: ${GLSLC_EXECUTABLE}")

# Set the base output directory for SPIR-V files to mirror the source directory structure
set(SPIRV_BASE_DIR ${CMAKE_BINARY_DIR}/assets/shaders)

# Glob all shader files in their respective directories
file(GLOB VERTEX_SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/vertex/*.vert")
file(GLOB FRAGMENT_SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/fragment/*.frag")
file(GLOB COMPUTE_SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/compute/*.comp")

foreach(SHADER ${VERTEX_SHADERS} ${FRAGMENT_SHADERS} ${COMPUTE_SHADERS})
    # Extract the filename
    get_filename_component(FILE_NAME ${SHADER} NAME)
    # Get the directory path of the shader file
    get_filename_component(FILE_PATH ${SHADER} DIRECTORY)
    # Replace the source directory path with the SPIR-V output directory path
    string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${SPIRV_BASE_DIR} SPIRV_OUTPUT_DIR ${FILE_PATH})
    # Create the output directory structure
    file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})
    # Define the output SPIR-V file path, retaining the shader type extension
    set(SPIRV_OUTPUT ${SPIRV_OUTPUT_DIR}/${FILE_NAME}.spv)

    # Add a custom command to compile the shader to SPIR-V
    add_custom_command(
        OUTPUT ${SPIRV_OUTPUT}
        COMMAND ${GLSLC_EXECUTABLE} -o ${SPIRV_OUTPUT} ${SHADER}
        DEPENDS ${SHADER}
        COMMENT "Compiling ${SHADER} to SPIR-V as ${SPIRV_OUTPUT}"
        VERBATIM
    )

    # Append the output SPIR-V file to the list
    list(APPEND SPIRV_FILES ${SPIRV_OUTPUT})
endforeach()

# Create a custom target for all shaders
add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_FILES})



