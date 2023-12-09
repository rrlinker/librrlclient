cmake_minimum_required(VERSION 3.14)

project(
    librrlclient
    VERSION 0.1
    DESCRIPTION "Remote Run-time Linker client library."
    LANGUAGES CXX
)

set(SOURCE_FILES
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

set(HEADER_FILES
    "include/rrlinker/client/librarian.hpp"
    "include/rrlinker/client/library.hpp"
    "include/rrlinker/client/linkage.hpp"
    "include/rrlinker/client/linker.hpp"
    "include/rrlinker/client/locallibrary.hpp"
    "include/rrlinker/client/locallinker.hpp"
    "include/rrlinker/client/remotelibrary.hpp"
    "include/rrlinker/client/remotelinker.hpp"
    "include/rrlinker/client/win32_error.hpp"
    "include/rrlinker/client/ws_connection.hpp"
)

add_library(${PROJECT_NAME} STATIC
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

set_target_properties(
    ${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    PUBLIC_HEADER "${HEADER_FILES}"
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC ws2_32 librrlcom
)

include(GNUInstallDirs)
install(
    TARGETS ${PROJECT_NAME}
    EXPORT client
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/rrlinker/client"
)
install(EXPORT client DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
