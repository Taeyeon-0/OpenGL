file(GLOB SOURCES "*.cpp")

foreach(source_file ${SOURCES})
    get_filename_component(example ${source_file} NAME_WE)
    add_executable(${example} ${source_file})
    target_link_libraries(${example} glad::glad glfw glm::glm plog::plog stb::stb)
endforeach()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Shader
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Shader)
