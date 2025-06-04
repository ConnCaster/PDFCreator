add_library(pdfcreator STATIC src/pdfcreator.cpp)

find_library(LIBHARU
        NAMES libhpdf hpdf
        PATHS ${PROJECT_SOURCE_DIR}/3rd_party/libharu/lib
        NO_DEFAULT_PATH
        REQUIRED
)
target_link_libraries(pdfcreator ${LIBHARU})

target_include_directories(pdfcreator
        PRIVATE
        ${PROJECT_SOURCE_DIR}/3rd_party/libharu/include
        ${PROJECT_SOURCE_DIR}/3rd_party/

        include/
)
