cmake_minimum_required(VERSION 3.13)
# ------------------------------------------------------------
# 默认属性
# ------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug) # 默认 Debug 模式
endif(NOT CMAKE_BUILD_TYPE)
# 使用 clang 编译器的额外配置
set(CLANG_GCC_TOOLCHAIN "<default>" CACHE PATH "gcc toolchain install prefix, used with clang --gcc-toolchain option")
if (NOT (CLANG_GCC_TOOLCHAIN STREQUAL "<default>"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --gcc-toolchain=${CLANG_GCC_TOOLCHAIN}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --gcc-toolchain=${CLANG_GCC_TOOLCHAIN}")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options("-std=c++17")
add_link_options("-static-libstdc++" "-pthread")
# ------------------------------------------------------------
# 项目
# ------------------------------------------------------------
project(ralgos)
add_executable(ralgos
    src/qqwry.cpp
    src/coroutine.cpp
    src/config.cpp
    src/ralgos.cpp
    src/runner.cpp
    src/server.cpp
    src/session.cpp
    src/handler/unknown.cpp
    src/handler/dummy.cpp
    src/handler/ping.cpp
    src/handler/segment.cpp
    src/handler/address.cpp
)
add_executable(raddrs
    src/qqwry.cpp
    src/raddrs.cpp
)
add_executable(rsegmt
    src/rsegmt.cpp
)
# 包含
target_include_directories(rsegmt SYSTEM PRIVATE
    /data/vendor/cppjieba-866d0e8/include
    /data/vendor/iconv-1.16.0/include
    /data/vendor/hiredis-0.14.0/include
    /data/vendor/boost-1.70.0/include
)
target_include_directories(raddrs SYSTEM PRIVATE
    /data/vendor/cppjieba-866d0e8/include
    /data/vendor/iconv-1.16.0/include
    /data/vendor/hiredis-0.14.0/include
    /data/vendor/boost-1.70.0/include
)
target_include_directories(ralgos SYSTEM PRIVATE
    /data/vendor/cppjieba-866d0e8/include
    /data/vendor/iconv-1.16.0/include
    /data/vendor/hiredis-0.14.0/include
    /data/vendor/boost-1.70.0/include
)
# 库
target_link_libraries(rsegmt
    /data/vendor/boost-1.70.0/lib/libboost_program_options.a
)
target_link_libraries(ralgos
    /data/vendor/iconv-1.16.0/lib/libiconv.a
    /data/vendor/hiredis-0.14.0/lib/libhiredis.a
    /data/vendor/boost-1.70.0/lib/libboost_program_options.a
    /data/vendor/boost-1.70.0/lib/libboost_context.a
    /data/vendor/boost-1.70.0/lib/libboost_system.a
    stdc++fs
)
target_link_libraries(raddrs
    /data/vendor/iconv-1.16.0/lib/libiconv.a
    /data/vendor/boost-1.70.0/lib/libboost_program_options.a
    stdc++fs
)
