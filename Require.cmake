include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

file(GLOB_RECURSE CURRENT_SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp"
        )

set(PORTAIBLE_SOURCES ${PORTAIBLE_SOURCES} ${CURRENT_SOURCES})