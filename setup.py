from distutils.core import setup, Extension
import numpy

try:
    from Cython.Build import cythonize
    USE_CYTHON = True
except ImportError:
    USE_CYTHON = False

ext = '.pyx' if USE_CYTHON else '.cpp'

extensions = [Extension(
                "pyfof",
                sources=["src/pyfof"+ext, "src/fof.cc"],
                extra_compile_args=["-std=c++11"],
                include_dirs=[numpy.get_include()],
                language="c++"
            )]

if USE_CYTHON:
    extensions = cythonize(extensions)

setup(  name = "pyfof",
        version="0.1",
        description="Friends-of-friends cluster finding in python",
        author="Simon Gibbons",
        author_email="sljg2@ast.cam.ac.uk",
        url="https://github.com/simongibbons/pyfof",
        download_url = 'https://github.com/simongibbons/pyfof/tarball/0.1',
        keywords=['clustering', 'friends-of-friends'],

        ext_modules = extensions
    )
