file(GLOB SOURCES "*.cpp")

add_custom_target(copy_shaders_materials ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Shader
    ${CMAKE_CURRENT_BINARY_DIR}/Shader
)

foreach(source_file ${SOURCES})
    get_filename_component(example ${source_file} NAME_WE)
    add_executable(${example} ${source_file})
    target_link_libraries(${example} glad::glad glfw glm::glm plog::plog)
endforeach()