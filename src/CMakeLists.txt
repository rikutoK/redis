# Compile the lib directory
file (GLOB_RECURSE LIB_SOURCE_FILES CONFIGURE_DEPENDS lib/*.cpp lib/*.hpp)
add_library(redis_lib STATIC "${LIB_SOURCE_FILES}")
target_include_directories(redis_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Compile the server directory
file(GLOB_RECURSE SERVER_SOURCE_FILES CONFIGURE_DEPENDS server/*.cpp server/*.hpp)
list(FILTER SERVER_SOURCE_FILES EXCLUDE REGEX main.cpp)

find_package(Threads REQUIRED)
find_package(asio CONFIG REQUIRED)

add_library(redis_server_lib STATIC "${SERVER_SOURCE_FILES}")
target_include_directories(redis_server_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/server")
target_link_libraries(redis_server_lib PUBLIC redis_lib)
target_link_libraries(redis_server_lib PUBLIC asio::asio)
target_link_libraries(redis_server_lib PUBLIC Threads::Threads)

add_executable(redis_server server/main.cpp)
target_link_libraries(redis_server PUBLIC redis_server_lib)
