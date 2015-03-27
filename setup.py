from distutils.core import setup, Extension
from Cython.Build import cythonize


setup( ext_modules = cythonize(Extension(
            "pyfof",
            sources=["src/pyfof.pyx", "src/fof.cc"], 
            extra_compile_args=["-std=c++11", "-O3"],
            language="c++"
    )))
