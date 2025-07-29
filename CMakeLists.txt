cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 选项：是否将SFML库构建为共享库，动态模式设为 ON；静态模式则设为 OFF
option(BUILD_SHARED_LIBS "Build SFML libraries as shared" OFF)

# 查找本地安装的 SFML 开发包
# set(SFML_STATIC_LIBRARIES TRUE) # 配合上面的 BUILD_SHARED_LIBS OFF 可实现静态链接
# find_package(SFML COMPONENTS Graphics Window System Audio Network REQUIRED)

# 获取SFML源码并构建
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_library(Game src/Game.cpp) # 创建一个库目标
# 设置 Game 目标的包含目录
target_include_directories(Game PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include  # 如果有自己的头文件目录
    ${SFML_INCLUDE_DIR}
)

add_executable(main src/main.cpp)   # 创建可执行文件目标
target_compile_features(main PRIVATE cxx_std_17)

# 链接库
target_link_libraries(Game
    PUBLIC
        SFML::Graphics
        SFML::Window
        SFML::System
        SFML::Audio
        SFML::Network
)

target_link_libraries(main
    PUBLIC
        Game
        # 针对MinGW，需要添加静态链接运行时库
        # -static-libgcc
        # -static-libstdc++
)


# 测试 & 打包
enable_testing()
add_test(NAME SmokeTest COMMAND main)

install(TARGETS main RUNTIME DESTINATION bin)
install(DIRECTORY "data/"
        DESTINATION "data"
        FILES_MATCHING PATTERN "*"
)

set(CPACK_PACKAGE_NAME "MineSweeper")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VENDOR "HDU")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CMake-SFML-MineSweeper")
set(CPACK_PACKAGE_CONTACT "contact@example.com")
include(CPack)