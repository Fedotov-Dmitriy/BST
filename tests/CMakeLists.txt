add_executable(bst_tests
    tests.c
)

target_link_libraries(bst_tests
    PRIVATE
        bst
)

target_compile_options(bst_tests
    PRIVATE
        -Wall
        -Wextra
        -pedantic
)