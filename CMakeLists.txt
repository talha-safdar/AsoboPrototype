# CMakeList.txt : CMake project for AsoboPrototype, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("AsoboPrototype")

# Add source to this project's executable.
add_executable (AsoboPrototype "src/AsoboPrototype.cpp" "src/AsoboPrototype.h" "src/Core/Application.h" "src/Core/Application.cpp")

# GLAD
add_library(glad STATIC
    external/glad/src/gl.c
)

target_include_directories(glad PUBLIC
    external/glad/include
)

# SDL
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

target_link_libraries(AsoboPrototype
    PRIVATE
        SDL3::SDL3-static
        glad
)

add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AsoboPrototype PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
