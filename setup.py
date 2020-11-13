#!/usr/bin/env python3

from numpy.distutils.core import setup
from numpy.distutils.misc_util import Configuration


from __version__ import version


def configuration(parent_package='', top_path=None):
    config = Configuration('f2py', parent_package, top_path)
    config.add_subpackage('tests')
    config.add_data_dir('tests/src')
    config.add_data_files(
        'src/fortranobject.c',
        'src/fortranobject.h')
    return config


if __name__ == "__main__":

    config = configuration(top_path='')
    config = config.todict()

    config['download_url'] = "http://cens.ioc.ee/projects/f2py2e/2.x"\
                             "/F2PY-2-latest.tar.gz"
    config['classifiers'] = [
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: C',
        'Programming Language :: Fortran',
        'Programming Language :: Python',
        'Topic :: Scientific/Engineering',
        'Topic :: Software Development :: Code Generators',
    ]
    setup(version=version,
          description="F2PY - Fortran to Python Interface Generator",
          author="Pearu Peterson",
          author_email="pearu@cens.ioc.ee",
          maintainer="Pearu Peterson",
          maintainer_email="pearu@cens.ioc.ee",
          license="BSD",
          platforms="Unix, Windows (mingw|cygwin), Mac OSX",
          long_description="""\
The Fortran to Python Interface Generator, or F2PY for short, is a
command line tool (f2py) for generating Python C/API modules for
wrapping Fortran 77/90/95 subroutines, accessing common blocks from
Python, and calling Python functions from Fortran (call-backs).
Interfacing subroutines/data from Fortran 90/95 modules is supported.""",
          url="http://cens.ioc.ee/projects/f2py2e/",
          keywords=['Fortran', 'f2py'],
          **config)
