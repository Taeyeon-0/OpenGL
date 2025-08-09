file(GLOB SOURCES "*.cpp")

foreach(source_file ${SOURCES})
    get_filename_component(example ${source_file} NAME_WE)
    add_executable(${example} ${source_file})
    target_link_libraries(${example} glad::glad glfw glm::glm plog::plog)
endforeach()