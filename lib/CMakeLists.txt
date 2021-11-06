set(BUILD_STATIC ON CACHE BOOL "" FORCE)
set(HIGHFIVE_USE_BOOST OFF)

add_subdirectory(ads EXCLUDE_FROM_ALL)
add_subdirectory(HighFive EXCLUDE_FROM_ALL)