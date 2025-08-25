cmake_minimum_required(VERSION 3.13)

project(redis-cpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -O1")
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json"
         DESTINATION "${CMAKE_SOURCE_DIR}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
add_subdirectory(src)
add_subdirectory(test)

if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/src/*.[ch]pp"
        "${CMAKE_SOURCE_DIR}/test/*.[ch]pp"
    )
    add_custom_target(
        format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_CXX_SOURCE_FILES}
        COMMENT "Running clang-format on source files"
    )
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy)
if(CLANG_TIDY_EXE)
    file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
        "${CMAKE_SOURCE_DIR}/src/*.[ch]pp"
        "${CMAKE_SOURCE_DIR}/test/*.[ch]pp"
    )

    add_custom_target(tidy
        COMMAND ${CLANG_TIDY_EXE}
                -p ${CMAKE_BINARY_DIR}
                ${ALL_CXX_SOURCE_FILES}
        COMMENT "Running clang-tidy on all source files"
    )
endif()
