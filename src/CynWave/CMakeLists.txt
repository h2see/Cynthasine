# Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
# Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
# copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/

FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG        3.4.0
)

FetchContent_MakeAvailable(
  eigen
)

FetchContent_Declare(
  pocketfft
  GIT_REPOSITORY https://github.com/mreineck/pocketfft.git
  GIT_TAG        bb87ca50df0478415a12d9011dc374eeed4e9d93
)

FetchContent_MakeAvailable(
  pocketfft
)

# Check if the fetch was successful
if(NOT pocketfft_POPULATED)
  # Fallback repository
  FetchContent_Declare(
    pocketfft
    GIT_REPOSITORY https://github.com/h2see/pocketfft.git
    GIT_TAG        bb87ca50df0478415a12d9011dc374eeed4e9d93
  )
  FetchContent_MakeAvailable(
    pocketfft
  )
endif()

# Add header only CynWave library
add_library(CynWave INTERFACE)

# Include public lib interface and Eigen headers
target_include_directories (CynWave
	INTERFACE
		${CMAKE_SOURCE_DIR}/include/CynWave
		${CMAKE_SOURCE_DIR}/src/CynWave
		${eigen_SOURCE_DIR}
		${pocketfft_SOURCE_DIR}
)
