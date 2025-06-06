add_library(pdfcreator STATIC src/pdfcreator.cpp
        tests/google_tests.cpp)

find_library(LIBHARU
        NAMES libhpdf hpdf
        PATHS ${PROJECT_SOURCE_DIR}/3rd_party/libharu/lib
        NO_DEFAULT_PATH
        REQUIRED
)
target_link_libraries(pdfcreator ${LIBHARU})

list(APPEND PDF_Creator_HEADERS
        ${PROJECT_SOURCE_DIR}/3rd_party/libharu/include
        ${PROJECT_SOURCE_DIR}/3rd_party/

        include/
)

target_include_directories(pdfcreator
        PRIVATE
       ${PDF_Creator_HEADERS}
)


include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(tests_google tests/google_tests.cpp src/pdfcreator.cpp)
target_include_directories(tests_google PUBLIC  ${PDF_Creator_HEADERS})
target_link_libraries(tests_google gtest gtest_main)
target_compile_definitions(tests_google PUBLIC BUILD_GOOGLE_TEST)
enable_testing()

add_test(NAME PDF_Creator_Tests COMMAND tests_google)