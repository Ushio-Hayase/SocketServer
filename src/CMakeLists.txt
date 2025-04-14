add_executable(SocketServer Server.cpp)
add_executable(SocketClient Client.cpp)

target_include_directories(SocketServer PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)
target_include_directories(SocketClient PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>)

target_link_libraries(SocketServer Ws2_32)
target_link_libraries(SocketClient Ws2_32)
