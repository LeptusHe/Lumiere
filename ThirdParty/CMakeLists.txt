set(better_enum_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
set(fmt_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fmt/include)
set(glfw_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/glfw/include)
set(glm_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/glm)
set(lua_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/lua)
set(sol2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/sol2/single/include)
set(stb_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/stb)

set(INCLUDE_DIRS
        ${better_enum_INCLUDE_DIRS}
        ${fmt_INCLUDE_DIRS}
        ${glfw_INCLUDE_DIRS}
        ${glm_INCLUDE_DIRS}
        ${lua_INCLUDE_DIRS}
        ${sol2_INCLUDE_DIRS}
        ${stb_INCLUDE_DIRS})
set(lumiere_third_party_INCLUDE_DIRS ${INCLUDE_DIRS} CACHE STRING "include directories for third party")


add_subdirectory(fmt EXCLUDE_FROM_ALL)
add_subdirectory(glfw EXCLUDE_FROM_ALL)
add_subdirectory(sol2 EXCLUDE_FROM_ALL)

aux_source_directory(lua lua-sources)
add_library(lua ${lua-sources})



