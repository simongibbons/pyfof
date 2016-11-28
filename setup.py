import os
import re

from distutils.core import setup, Extension
from distutils.command.build_ext import build_ext as _build_ext

import numpy

try:
    from Cython.Build import cythonize
    USE_CYTHON = True
except ImportError:
    USE_CYTHON = False

ext = '.pyx' if USE_CYTHON else '.cpp'

def find_boost(hint=None, verbose=True):
    """ Find the boost include directory,
    returns ``None`` if not found.

    This is based upon the version in https://github.com/dfm/transit/
    Copyright (c) 2014 Daniel Foreman-Mackey
    which is licenced under the MIT licence.
    """

    search_dirs = [] if hint is None else hint

    try:
        search_dirs.append( os.path.join( os.environ["BOOST_DIR"], "include") )
    except KeyError:
        pass

    search_dirs += [
        "/usr/local/include",
        "/usr/local/homebrew/include",
        "/opt/local/include",
        "/data/sljg2/software/boost/include"
    ]


    for d in search_dirs:
        path = os.path.join(d, "boost", "geometry", "index", "rtree.hpp")
        if os.path.exists(path):
            vf = os.path.join(d, "boost", "version.hpp")
            src = open(vf, "r").read()
            v = re.findall("#define BOOST_LIB_VERSION \"(.+)\"", src)
            if not len(v):
                continue
            v = v[0]
            if verbose:
                print("Found Boost version {0} in: {1}".format(v, d))
            return d
    return None

class build_ext(_build_ext):
    def build_extension(self, ext):
        dirs = ext.include_dirs + self.compiler.include_dirs

        boost_include = find_boost(hint=dirs)
        if boost_include is None:
            raise RuntimeError("Boost not found, see the README for possible solutions")

        ext.include_dirs.append(boost_include)

        _build_ext.build_extension(self, ext)


extensions = [Extension(
                "pyfof",
                sources=["pyfof/pyfof"+ext, "pyfof/fof.cc"],
                extra_compile_args=["-std=c++11"],
                include_dirs=[numpy.get_include()],
                language="c++"
            )]

if USE_CYTHON:
    extensions = cythonize(extensions)

setup(  name = "pyfof",
        version="0.2-dev",
        description="Friends-of-friends cluster finding in python",
        author="Simon Gibbons",
        author_email="sljg2@ast.cam.ac.uk",
        url="https://github.com/simongibbons/pyfof",
        license="MIT",
        keywords=['clustering', 'friends-of-friends'],
        install_requires=[
            'numpy',
        ]

        cmdclass={'build_ext' : build_ext},
        ext_modules = extensions
    )
