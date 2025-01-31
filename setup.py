from setuptools import setup, Extension
from pybind11.setup_helpers import Pybind11Extension, build_ext
import sys

# Define common compiler flags
DBG = False
if DBG:
    extra_compile_args = ['-O3', '-pg', '-fprofile-arcs', '-ftest-coverage']
    extra_link_args = ['-pg', '-fprofile-arcs', '-ftest-coverage']
else:
    extra_compile_args = ['-O3']
    extra_link_args = []

ext_modules = [
    Pybind11Extension(
        "myersdiff_anti",
        ["myersdiff2.cpp"],
        cxx_std=14,
        extra_compile_args=extra_compile_args,
        extra_link_args=extra_link_args,
    ),
]

setup(
    name="myersdiff_anti",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
    version="0.1.0",  # Add version number
    author="Your Name",  # Add author information
    description="A simple myers diff implementation with cpp",  # Add description
    python_requires=">=3.6",  # Add Python version requirement
    setup_requires=["pybind11>=2.6.0", "wheel"],  # Add setup_requires

    install_requires=["pybind11>=2.6.0", "wheel"],  # Add dependencies

)

