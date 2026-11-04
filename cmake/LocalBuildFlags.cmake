include_guard(GLOBAL)


# -ffast-math implies -ffinite-math-only, which makes NaN/Inf operations undefined behavior:
# Every isnan/isinf test is then compiled out, and the result of any comparison involving a NaN
# becomes unpredicatable (compiler/arch-dependent). Set this to OFF to keep the rest of
# -ffast-math while restoring IEEE semantics for non-finite values.
option(FINITE_MATH_ONLY "Let -ffast-math assume that no NaN or Inf ever occurs" OFF)

set(FAST_MATH_FLAGS "-ffast-math")
if(NOT FINITE_MATH_ONLY)
  # Must follow -ffast-math, which would otherwise re-enable the assumption
  set(FAST_MATH_FLAGS "${FAST_MATH_FLAGS} -fno-finite-math-only")
endif()


# Build type compilation flags (if different from default or unknown to CMake)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${FAST_MATH_FLAGS}"
    CACHE STRING "Flags used by the compiler during release builds."
    FORCE)
set(CMAKE_C_FLAGS_RELEASE "-O3 ${FAST_MATH_FLAGS}"
    CACHE STRING "Flags used by the compiler during release builds."
    FORCE)

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 ${FAST_MATH_FLAGS} -g${DEBUG_FORMAT}${DEBUG_LEVEL}"
    CACHE STRING "Flags used by the compiler during Release with Debug Info builds."
    FORCE)
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 ${FAST_MATH_FLAGS} -g${DEBUG_FORMAT}${DEBUG_LEVEL}"
    CACHE STRING "Flags used by the compiler during Release with Debug Info builds."
    FORCE)
