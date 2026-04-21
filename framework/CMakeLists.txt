cmake_minimum_required(VERSION 3.30)
project(peter_game_framework VERSION 0.0.1 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SDL3 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG a962f40bbba175e9716557a25d5d7965f134a3d3)
FetchContent_MakeAvailable(SDL3)

set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)

FetchContent_Declare(SDL3_image GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git GIT_TAG ad58ecfc27a1bd09e510ceff8bbbdb1094806476)
FetchContent_MakeAvailable(SDL3_image)

FetchContent_Declare(imgui GIT_REPOSITORY https://github.com/ocornut/imgui.git GIT_TAG 6ded5230d043aa32c755e65c910c2af5002fb9f9)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)

target_include_directories(imgui 
    PUBLIC
    ${SDL3_INCLUDE_DIRS}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    PRIVATE

)

target_link_libraries(imgui
    PUBLIC
        SDL3::SDL3
)

set(BUILD_SHARED_LIBS OFF)

FetchContent_Declare(glm GIT_REPOSITORY https://github.com/g-truc/glm.git GIT_TAG 8d1fd52e5ab5590e2c81768ace50c72bae28f2ed)
FetchContent_MakeAvailable(glm)

file(GLOB CPP_FILES CONFIGURE_DEPENDS src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE

)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        imgui
        glm::glm
    PRIVATE
        
)