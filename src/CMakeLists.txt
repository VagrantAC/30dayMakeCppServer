file(GLOB_RECURSE pine_sources ${PROJECT_SOURCE_DIR}/src/*.cc)
add_library(pine_shared SHARED ${pine_sources})
