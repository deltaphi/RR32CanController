# Now simply link against gtest or gtest_main as needed. Eg
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/lib/RR32CanLibrary/lib")
#include_directories("${PROJECT_SOURCE_DIR}/lib/RotaryEncoder")
include_directories("${PROJECT_SOURCE_DIR}/test/sil/application")
include_directories("${PROJECT_SOURCE_DIR}/test/sil/mocks")
include_directories("${PROJECT_SOURCE_DIR}/test/sil/argtable3/src")

add_definitions(-DARGTABLE_HEADER_PATH="argtable3.h")

list(APPEND MOCK_SOURCES
  "mocks/Arduino.cpp"
)

file(GLOB ARGTABLE3_SOURCES "${PROJECT_SOURCE_DIR}/test/sil/argtable3/src/*.c")

add_executable(ApplicationSilTest
    application/MenuTest.cpp
    
    ${MOCK_SOURCES}
    ${ARGTABLE3_SOURCES}
    ${RR32CAN_SOURCES}
    ${applicationSources}
     )
target_link_libraries(ApplicationSilTest gtest_main gmock)
add_test(NAME gtest_ApplicationSil_test COMMAND ApplicationSilTest)
