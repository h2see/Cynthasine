# Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
# Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
# copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/

add_library(Cynthasine STATIC
	${CMAKE_SOURCE_DIR}/src/Cynthasine/Cynthasine.cpp
)

target_include_directories(Cynthasine
	PUBLIC
		${CMAKE_SOURCE_DIR}/include/Cynthasine
)

if(BUILD_CYN_AUDIO)
	target_link_libraries(Cynthasine
		PUBLIC
			CynAudio
	)
	target_compile_definitions(Cynthasine PUBLIC CYN_LIB_H="CynAudio.h")
else()
	target_link_libraries(Cynthasine
		PUBLIC
			CynWave
	)
	target_compile_definitions(Cynthasine PUBLIC CYN_LIB_H="CynWave.h")
endif()
