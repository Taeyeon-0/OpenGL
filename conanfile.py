from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    default_options = {
        "qt/*:shared": True,
        "qt/*:qttools": True
    }

    def requirements(self):
        self.requires("qt/5.15.19")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generator = "Ninja"
        tc.cache_variables["CMAKE_PREFIX_PATH"] = self.generators_folder
        tc.generate()