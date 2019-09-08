project(
    'libls',
    ['c'],
    version: '0.0.0',
    license: [
        'ZLib',
    ],
    default_options: [
        'c_std=c11',
        'prefix=/usr',
        'sysconfdir=/etc',
        'localstatedir=/var',
    ],
)

# Define our library ABI version at a global level.
abi_version_major = '0'
abi_version_minor = '0'
abi_version_micro = '0'
abi_version = '@0@.@1@.@2@'.format(abi_version_major,
                                   abi_version_minor,
                                   abi_version_micro)


am_cflags = [
    '-fstack-protector',
    '-Wall',
    '-pedantic',
    '-Wstrict-prototypes',
    '-Wundef',
    '-fno-common',
    '-Werror-implicit-function-declaration',
    '-Wformat',
    '-Wformat-security',
    '-Werror=format-security',
    '-Wconversion',
    '-Wunused-variable',
    '-Wunreachable-code',
    '-W',
]

# Get configuration bits together
path_prefix = get_option('prefix')
path_sysconfdir = join_paths(path_prefix, get_option('sysconfdir'))

# Sort out config.h now
cdata = configuration_data()

# General options..
cdata.set_quoted('PACKAGE_NAME', meson.project_name())
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())

# Write config.h now
config_h = configure_file(
     configuration: cdata,
     output: 'config.h',
)
config_h_dir = include_directories('.')

with_tests = get_option('with-tests')

# Now go build the source
subdir('src')

# Only tested with check 0.12.0. Yes, irony.
if with_tests == true
    dep_check = dependency('check', version: '>= 0.12.0')
    subdir('tests')
endif

report = [
    '    Build configuration:',
    '    ====================',
    '',
    '    prefix:                                 @0@'.format(path_prefix),
    '    sysconfdir:                             @0@'.format(path_sysconfdir),
    '    enable tests:                           @0@'.format(with_tests),
]

if meson.is_subproject() == false
    # Output some stuff to validate the build config
    message('\n\n\n' + '\n'.join(report) + '\n\n')
endif
