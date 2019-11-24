cmake_minimum_required(VERSION 3.15)
project(Lumiere)
set(CMAKE_CXX_STANDARD 17)

set(NAMESPACE_NAME "Syrinx" CACHE STRING "namespace name")
string(TOUPPER ${NAMESPACE_NAME} NAMESPACE_NAME_UPPER)

configure_file(Sources/Common/LumiereConfig.h.in ${PROJECT_SOURCE_DIR}/Sources/Common/LumiereConfig.h)

add_subdirectory(ThirdParty)
add_subdirectory(Sources)

