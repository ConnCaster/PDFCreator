add_library(pdfcreator STATIC src/pdfcreator.cpp)

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