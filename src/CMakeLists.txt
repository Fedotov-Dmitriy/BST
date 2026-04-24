add_library(bst STATIC
    bst.c
)

target_include_directories(bst
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(bst
    PRIVATE
        -Wall
        -Wextra
        -pedantic
)