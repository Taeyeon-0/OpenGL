add_executable(colors colors.cpp)
target_link_libraries(colors glad::glad glfw glm::glm plog::plog)

add_custom_target(copy_shaders_colors ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Shader
    ${CMAKE_CURRENT_BINARY_DIR}/Shader
)