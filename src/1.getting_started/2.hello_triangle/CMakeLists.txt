file(GLOB SOURCES "*.cpp")

foreach(source_file ${SOURCES})
    get_filename_component(example ${source_file} NAME_WE)
    # 去除开头的数字和点，如 "1.hello_window" 变为 "hello_window"
    string(REGEX REPLACE "^[0-9]+\\.(.*)$" "\\1" example ${example})
    add_executable(${example} ${source_file})
    target_link_libraries(${example} glad::glad glfw glm::glm plog::plog)
endforeach()
