cmake_minimum_required(VERSION 4.2)
project(MePlayer VERSION 0.4.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "DEV_BUILD")

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Track.cpp
        include/Track.h
        src/PlaybackQueue.cpp
        include/PlaybackQueue.h
        src/LibraryHandler.cpp
        include/LibraryHandler.h
)

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

FetchContent_MakeAvailable(SDL3 SDL3_image)

# FFmpeg
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavutil libavformat libswscale libswresample)

target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL3_image::SDL3_image
        SDL3::SDL3
        PkgConfig::FFMPEG
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# For some reason, using Debug and Release (like a normal person) causes app-breaking behavior. Thanks, FFmpeg
if (CMAKE_BUILD_TYPE STREQUAL "DEV_BUILD")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "PUB_BUILD")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
        VERSION="${PROJECT_VERSION}"
)