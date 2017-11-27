# Create the main library

libuf_sources = [
    'map.c',
]

libuf_include_directories = [
    config_h_dir,
    include_directories('.'),
]

if get_option('with-static') == true
    libuf = static_library('uf',
        sources: libuf_sources,
        c_args: am_cflags,
        include_directories: libuf_include_directories,
    )
else
    libuf = shared_library('uf',
        sources: libuf_sources,
        version: abi_version,
        c_args: am_cflags,
        include_directories: libuf_include_directories,
    )
endif

# Allow other components to link here
link_libuf = declare_dependency(
    link_with: libuf,
    include_directories: [
        include_directories('.'),
    ],
)
