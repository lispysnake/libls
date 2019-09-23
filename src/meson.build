# Create the main library

libls_sources = [
    'array.c',
    'list.c',
    'map.c',
]

libls_include_directories = [
    config_h_dir,
    include_directories('.'),
]

if get_option('with-static') == true
    libls = static_library('ls',
        sources: libls_sources,
        c_args: am_cflags,
        include_directories: libls_include_directories,
    )
else
    libls = shared_library('ls',
        sources: libls_sources,
        version: abi_version,
        c_args: am_cflags,
        include_directories: libls_include_directories,
    )
endif

# Allow other components to link here
link_libls = declare_dependency(
    link_with: libls,
    include_directories: [
        include_directories('.'),
    ],
)
