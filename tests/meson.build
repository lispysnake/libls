# Contains definitions for all of our tests

required_tests = [
    'list',
    'map',
]

# Just need libls, self contained.
test_dependencies = [
    link_libls,
    dep_check,
]

# Similar to the test meson I created for clr-boot-mnager tests/meson.build
foreach test : required_tests
    t = executable(
        'test-@0@'.format(test),
        sources: [
            'check-@0@.c'.format(test),
        ],
        c_args: am_cflags,
        dependencies: test_dependencies,
        install: false,
    )
    test(test, t)
endforeach
