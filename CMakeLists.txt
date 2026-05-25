cmake_minimum_required(VERSION 4.2)
project(MePlayer VERSION 0.4.2)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Track.cpp
        src/PlaybackQueue.cpp
        src/LibraryHandler.cpp
        src/Button.cpp
        src/TextButton.cpp
        include/Track.h
        include/PlaybackQueue.h
        include/LibraryHandler.h
        include/Button.h
        include/TextButton.h
)

# I feel like this probably isn't a great practice
include(FetchContent)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG d9d5536704d585616d4db3c8ba3c4ff6fc2757e1 # 3.4.8
)

FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG bec9134a26c7d0f31b36d6083c25296e04cabff5 # 3.4.4
)

FetchContent_Declare(
        SDL3_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG a1ce3670aec736ecbf0936c43f2f0cc53aa61e5b # 3.2.2
)

FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_ttf)

# FFmpeg
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavutil libavformat libswscale libswresample)

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        PkgConfig::FFMPEG
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# For some reason, using Release (like a normal person) causes app-breaking behavior, so this won't work yet. Thanks, FFmpeg
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VERSION="${PROJECT_VERSION}"
)