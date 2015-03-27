from distutils.core import setup, Extension
from Cython.Build import cythonize


setup(  name = "pyfof",
        version="0.1",
        description="Friends-of-friends cluster finding in python",
        author="Simon Gibbons",
        author_email="sljg2@ast.cam.ac.uk",
        url="https://github.com/simongibbons/pyfof",
        keywords=['clustering', 'friends-of-friends'],

        ext_modules = cythonize(Extension(
            "pyfof",
            sources=["src/pyfof.pyx", "src/fof.cc"],
            extra_compile_args=["-std=c++11"],
            language="c++"
        ))
    )
