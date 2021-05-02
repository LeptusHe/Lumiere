cmake_minimum_required(VERSION 3.15)
project(Lumiere)
set(CMAKE_CXX_STANDARD 17)

set(NAMESPACE_NAME "Syrinx" CACHE STRING "namespace name")
option(LUMIERE_ENABLE_DEVICE_CODE "enable device code compile" ON)

string(TOUPPER ${NAMESPACE_NAME} NAMESPACE_NAME_UPPER)

configure_file(Sources/Common/LumiereConfig.h.in ${PROJECT_SOURCE_DIR}/Sources/Common/LumiereConfig.h)

add_subdirectory(ThirdParty)
add_subdirectory(Sources)

# build entityx
set(ENTITYX_BUILD_TESTING FALSE CACHE BOOL "disable build test")
set(ENTITYX_DT_TYPE float CACHE STRING "use float type for delta time")
set(ENTITYX_BUILD_SHARED FALSE CACHE BOOL "build shared library")
add_subdirectory(ThirdParty/entityx)

