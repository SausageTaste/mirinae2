add_executable(mirinae_sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_compile_features(mirinae_sdl PUBLIC cxx_std_23)

target_link_libraries(mirinae_sdl PUBLIC
    spdlog::spdlog
)
