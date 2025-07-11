from setuptools import setup, Extension
from pathlib import Path
import sys

# Simple setuptools build using Python C API

mbase_include = Path(__file__).resolve().parent.parent / 'include'

ext = Extension(
    'mbasepy._core',
    sources=['mbasepy/_core.cpp'],
    include_dirs=[str(mbase_include)],
    language='c++',
    extra_compile_args=(
        ['/std:c++17'] if sys.platform == 'win32' else ['-std=c++17']
    )
)

setup(
    name='mbasepy',
    version='0.1.0',
    packages=['mbasepy'],
    ext_modules=[ext],
)
