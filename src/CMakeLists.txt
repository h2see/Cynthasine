# Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
# Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
# copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/

add_subdirectory(CynWave)

if(BUILD_CYN_AUDIO)
  add_subdirectory(CynAudio)
endif()

add_subdirectory(Cynthasine)
