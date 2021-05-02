list(APPEND include_dirs ${CMAKE_CURRENT_SOURCE_DIR} ${lumiere_third_party_INCLUDE_DIRS})

if (LUMIERE_ENABLE_DEVICE_CODE)
    find_package(CUDA REQUIRED)
    list(APPEND include_dirs ${CUDA_TOOLKIT_INCLUDE})
endif()


list(APPEND sub_dirs Common Container Exception FileSystem Image Input Logging Math Script Serializer Streaming Time)
foreach(sub_dir ${sub_dirs})
    aux_source_directory(${sub_dir} sources_founded)
    list(APPEND sources ${sources_founded})
endforeach()

if (BUILD_SHARED_LIBRARY)
    add_library(Lumiere SHARED ${sources})
else()
    add_library(Lumiere STATIC ${sources})
endif()
target_include_directories(Lumiere PUBLIC ${include_dirs})
target_link_libraries(Lumiere glfw fmt-header-only entityx -lstdc++fs)
