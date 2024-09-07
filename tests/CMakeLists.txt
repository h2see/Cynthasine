# Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
# Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
# copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/

# Install GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Add test executables
add_executable(
  wave_cyn_test
  "wave_cyn_test.cpp"
)

# Make SOURCE_DIR available to test_cynthasine
target_compile_definitions(wave_cyn_test PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Link to necessary libraries for testing
target_link_libraries(wave_cyn_test PRIVATE
  Cynthasine
  GTest::gtest_main
)

if(BUILD_CYN_AUDIO)
add_executable(
  audio_cyn_test
  "audio_cyn_test.cpp"
)
target_compile_definitions(audio_cyn_test PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}")
target_link_libraries(audio_cyn_test PRIVATE
  Cynthasine
  GTest::gtest_main
)
endif()

# Use GoogleTest to discover tests
include(GoogleTest)
gtest_discover_tests(wave_cyn_test)

if(BUILD_CYN_AUDIO)
gtest_discover_tests(audio_cyn_test)
endif()
