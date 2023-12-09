cmake_minimum_required(VERSION 3.14)

project(
    librrlclient
    VERSION 0.1
    DESCRIPTION "Remote Run-time Linker client library."
    LANGUAGES CXX
)

file(GLOB SOURCE_FILES
    "lib/librarian.cpp"
    "lib/library.cpp"
    "lib/linker.cpp"
    "lib/locallibrary.cpp"
    "lib/locallinker.cpp"
    "lib/remotelibrary.cpp"
    "lib/remotelinker.cpp"
    "lib/win32_error.cpp"
    "lib/ws_connection.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC include
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC ws2_32 librrlcom
)
