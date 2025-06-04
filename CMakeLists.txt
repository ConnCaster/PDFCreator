cmake_minimum_required(VERSION 3.31)
project(PDFCreatorRunner)

set(CMAKE_CXX_STANDARD 17)

add_executable(PDFCreatorRunner main.cpp)
target_link_libraries(PDFCreatorRunner pdfcreator)
target_include_directories(PDFCreatorRunner
        PRIVATE
        libs/libpdfcreator/include

        3rd_party/libharu/include
        3rd_party/
)

add_subdirectory(libs/libpdfcreator)
