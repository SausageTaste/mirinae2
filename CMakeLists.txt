cmake_minimum_required(VERSION 3.16)

if (NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

project(mirinae2
    VERSION 0.1.0
    LANGUAGES CXX
)


find_package(spdlog CONFIG REQUIRED)


add_subdirectory(app/mirinae_sdl)
