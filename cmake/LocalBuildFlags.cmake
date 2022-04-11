include_guard(GLOBAL)


# Build type compilation flags (if different from default or unknown to CMake)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math"
    CACHE STRING "Flags used by the compiler during release builds."
    FORCE)
set(CMAKE_C_FLAGS_RELEASE "-O3 -ffast-math"
    CACHE STRING "Flags used by the compiler during release builds."
    FORCE)
    
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -ffast-math -g${DEBUG_FORMAT}${DEBUG_LEVEL}"
    CACHE STRING "Flags used by the compiler during Release with Debug Info builds."
    FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -ffast-math -g${DEBUG_FORMAT}${DEBUG_LEVEL}"
    CACHE STRING "Flags used by the compiler during Release with Debug Info builds."
    FORCE)
