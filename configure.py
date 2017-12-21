#!/usr/bin/env python
"""
Build sketches
"""

from __future__ import print_function

import os
import sys

SOURCEDIR = os.path.dirname(os.path.realpath(__file__))
sys.path.insert(0, os.path.join(SOURCEDIR, 'misc'))

from ninja_syntax import Writer

SYS_PLATFORM = sys.platform
if SYS_PLATFORM.startswith('linux'):
    PLATFORM = 'linux'
elif SYS_PLATFORM.startswith('win'):
    PLATFORM = 'msvc'
elif SYS_PLATFORM.startswith('darwin'):
    PLATFORM = 'osx'

BUILD_FILENAME = 'build.ninja'
NINJA_WRITER = Writer(open(BUILD_FILENAME, 'w'))
N = NINJA_WRITER

# @TODO: Port this stuff
assert PLATFORM == 'osx'

N.comment('build sketches for %s' % PLATFORM)
N.newline()

N.variable('builddir', 'build')
N.variable('cc', 'clang')
N.variable('ld', '$cc')
N.newline()

CFLAGS = [
    # Common Compiler Flags
    '-g',
    '-std=c99',
    '-Wall',
    '-fcolor-diagnostics',
    # Include paths
    
    # Warnings
    '-Wno-missing-braces'
]

LDFLAGS = [

]

N.variable('cflags', " ".join(CFLAGS))
N.variable('ldflags', " ".join(LDFLAGS))
N.newline()

N.rule('compile',
       '$cc $cflags -MMD -MF $out.d -c -o $out $in',
       depfile='$out.d',
       deps='gcc')
N.newline()

N.rule('link', '$ld -o $out $in $ldflags')
N.newline()

N.comment("hello world")
N.build('$builddir/hello_world.o', 'compile', 'hello_world.c')
N.build('$builddir/hello_world', 'link', '$builddir/hello_world.o')

N.close()
