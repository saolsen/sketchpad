#!/usr/bin/env python
# pylint: disable=C0413
# pylint: disable=C0303
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

CFLAGS = " ".join([
    # Common Compiler Flags
    '-g',
    '-std=c99',
    '-Wall',
    '-fcolor-diagnostics',
    # Include paths
    '-I3rdparty',
    # Warnings
    '-Wno-missing-braces'
])

N.variable('cflags', CFLAGS)
N.variable('ldflags', '')
N.newline()

N.rule('compile',
       '$cc $cflags -MMD -MF $out.d -c -o $out $in $ldflags',
       depfile='$out.d',
       deps='gcc')
N.newline()

N.rule('link_exe', '$ld -o $out $in $ldflags')
N.newline()

N.rule('link_dylib', '$ld -o $out $in $ldflags -dynamiclib -undefined dynamic_lookup')
N.newline()

N.rule('link_sdl2', '$ld -o $out $in $ldflags -F 3rdparty/lib -framework SDL2 -framework OpenGL -rpath @executable_path/../Frameworks')
N.newline()

BUNDLE_COMMANDS = [
    'mkdir -p $builddir/$out/Contents/{MacOS,Resources,Frameworks}',
    'cp misc/Info.plist $builddir/$out/Contents/Info.plist',
    "sed -i -e 's/{APP}/$name/g' $builddir/$out/Contents/Info.plist",
    "rsync -aq 3rdparty/lib/SDL2.framework $builddir/$out/Contents/Frameworks/"
]

N.rule('app_bundle', ' && '.join(BUNDLE_COMMANDS))
N.newline()

N.rule('copy_file', 'cp $in $out')
N.newline()

def c_file(name):
    return name + ".c"

def dep_file(name):
    return '3rdparty/'+ name + ".c"

def obj_file(name):
    return '$builddir/'+ name + ".o"

def exe_file(name):
    return '$builddir/' + name

def app_exe_file(appname, filename):
    return '$builddir/' + appname + '.app/Contents/MacOS/' + filename

def dylib_file(name):
    return '$builddir/' + name + ".dylib"

def extend_cflags(additional_flags):
    return CFLAGS + ' ' + additional_flags

# Libraries
N.comment('nanovg')
N.build(obj_file('nanovg'), 'compile', dep_file('nanovg'))
N.newline()

N.comment('glad')
N.build(obj_file('glad'), 'compile', dep_file('glad'), variables={'cflags': extend_cflags('-Wno-unused-function')})
N.newline()

# Typical sketches, simple little programs
N.comment('hello_world')
N.build(obj_file('hello_world'), 'compile', c_file('hello_world'))
N.build(exe_file('hello_world'), 'link_exe', obj_file('hello_world'))
N.newline()

N.comment('raytracer')
N.build(obj_file('raytracer'), 'compile', c_file('raytracer'))
N.build(exe_file('raytracer'), 'link_exe', obj_file('raytracer'))
N.newline()

# Reloading but not in an app bundle.
N.comment('test_reload')
N.build(obj_file('test_reload'), 'compile', c_file('test_reload'))
N.build(exe_file('test_reload'), 'link_exe', obj_file('test_reload'))
N.build(obj_file('lib_test_reload'), 'compile', c_file('lib_test_reload'))
N.build(dylib_file('libtestreload'), 'link_dylib', obj_file('lib_test_reload'))
N.newline()

# Simple sdl2 example without reloading.
N.comment('sdl2_example')
N.build('sdl2_example.app', 'app_bundle', variables={'name': 'sdl2_example'})
N.build(obj_file('sdl2_example'), 'compile', c_file('sdl2_example'))
N.build(exe_file('sdl2_example'), 'link_sdl2', [obj_file('sdl2_example'), obj_file('nanovg'), obj_file('glad')])
N.build(app_exe_file('sdl2_example', 'sdl2_example'), 'copy_file', exe_file('sdl2_example'))

N.close()
