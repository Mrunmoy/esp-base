file(GLOB_RECURSE srcs
    "*.c"
    "*.cpp"
    )

set(include_dirs
    "."
    )

idf_component_register(SRCS "${srcs}"
    INCLUDE_DIRS "${include_dirs}"
    )