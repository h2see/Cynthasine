# Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
# Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
# copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/

# Fetch PortAudio with a specific commit
FetchContent_Declare(
  PortAudio
  GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
  GIT_TAG        52d50e2f4d2f3ab815f64fe683d864d99011186d
)

FetchContent_MakeAvailable(
  PortAudio
)

# Add static CynAudio library
add_library(CynAudio STATIC
	${CMAKE_SOURCE_DIR}/src/CynAudio/CynPlayer.cpp
	${CMAKE_SOURCE_DIR}/src/CynAudio/CynAudio.cpp
)

# Include public lib interface
target_include_directories (CynAudio
	PUBLIC
		${CMAKE_SOURCE_DIR}/include/CynAudio
		${CMAKE_SOURCE_DIR}/src/CynAudio
)

target_link_libraries(CynAudio
	PUBLIC
		CynWave
	PRIVATE
		PortAudio
)
