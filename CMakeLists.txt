cmake_minimum_required(VERSION 3.13)

project(zensh VERSION 0.1.0)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES src/*.c)

add_executable(zensh ${SRC_FILES})

target_link_libraries(zensh PRIVATE readline)

install(TARGETS zensh
        RUNTIME DESTINATION bin)

install(FILES man/zensh.1
        DESTINATION share/man/man1)

