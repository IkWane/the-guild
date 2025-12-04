file(GLOB F_GLOB "*.cpp")
list(FILTER F_GLOB EXCLUDE REGEX "main.cpp")
list(FILTER F_GLOB EXCLUDE REGEX "gatherer.cpp")
list(LENGTH F_GLOB nb_e)
if(${nb_e} GREATER 0)
    message("-- Adding files in /src")
    add_library(main ${F_GLOB})
    target_link_libraries(${exec_name} main)
    target_link_libraries(${tag_getter_name} main)
else()
    message("-- No additional files found in /src.")
endif()


