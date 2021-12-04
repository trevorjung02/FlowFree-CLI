from conans import ConanFile, tools
import os


class MinisatConan(ConanFile):
    python_requires = "conan_base/[~=2.0]@cybercalc/master"
    python_requires_extend = "conan_base.CybercalcConanBase"

    name = "minisat"
    url = "https://demeeslx0105/cybercalc/external-deps/minisat"
    homepage = url
    author = "Niklas Sorensson <niklasso@gmail.com>"
    description = "A minimalistic and high-performance SAT solver"
    license = "MIT"
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    no_copy_source = True
    options = {
        'shared': [False, True],
        'lto': [False, True],
    }
    default_options = {
        'shared': False,
        'lto': False,
    }
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
    }

    def source(self):
        tools.replace_in_file("CMakeLists.txt", "project(MiniSat VERSION 2.2 LANGUAGES CXX)",
                              '''project(MiniSat VERSION 2.2 LANGUAGES CXX)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()''')

    def build(self):
        cmake = self.CMake()
        cmake.configure()
        cmake.build()
        if self.should_test:
            try:
                cpu_count = len(os.sched_getaffinity(0))
            except AttributeError:
                cpu_count = os.cpu_count() or 1
            print(cmake.definitions)
            cmd = 'ctest -j {}'.format(cpu_count)
            if cmake.is_multi_configuration:
                cmd += ' -C {}'.format(cmake.build_config.split()[-1])
            self.run(cmd)

    def package(self):
        cmake = self.CMake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs += ['minisat']
