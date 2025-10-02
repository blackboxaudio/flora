cmake_minimum_required(VERSION 3.14)
project(Flora)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/neuron)
add_subdirectory(vendor/libDaisy)

function(add_patch platform patch)
    set(PATCH_DIR patches/${platform}/${patch})
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PATCH_DIR})
        set(TARGET_NAME ${platform}_${patch})
        file(GLOB_RECURSE PATCH_SOURCES ${PATCH_DIR}/*.cpp)
        add_executable(${TARGET_NAME} ${PATCH_SOURCES})
        target_include_directories(${TARGET_NAME} PRIVATE ${PATCH_DIR})
        target_link_libraries(${TARGET_NAME} PRIVATE neuron daisy)
    endif()
endfunction()

# patch.Init()
add_patch(init kudzu)
add_patch(init lotus)

# pod
add_patch(pod lily)

# seed
add_patch(seed blink)
add_patch(seed daisy)
