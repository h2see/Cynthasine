# Except where otherwise noted, Cynthasine © 2024 by https://github.com/h2see is licensed under Creative
# Commons Attribution-NonCommercial-ShareAlike 4.0 International. To view a
# copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/4.0/


# Add example executables and link to necessary libraries

if(BUILD_CYN_AUDIO)
  add_executable (twinkle_cyn_ex "twinkle_cyn_ex.cpp")
  target_link_libraries (twinkle_cyn_ex PRIVATE Cynthasine)
endif()