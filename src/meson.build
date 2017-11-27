# Create the main library

libuf_sources = [
    'map.c',
]

libuf = library('uf',
    sources: libuf_sources,
    version: abi_version,
    c_args: am_cflags,
    include_directories: [
        config_h_dir,
        include_directories('.'),
    ],
)

# Allow other components to link here
link_libuf = declare_dependency(
    link_with: libuf,
    include_directories: [
        include_directories('.'),
    ],
)


test_bin = executable(
    'demo',
    sources: ['main.c'],
    dependencies: [link_libuf],
    install: false,
)
