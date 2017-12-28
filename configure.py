#!/usr/bin/env python
# pylint: disable=C0413
# pylint: disable=C0303
"""
Build sketches
"""

from __future__ import print_function

import os
import sys

def c_file(name):
    return name + ".c"

def cpp_file(name):
    return name + ".cpp"

def dep_file(name):
    return '3rdparty/'+ name + ".c"

def cpp_dep_file(name):
    return '3rdparty/'+ name + ".cpp"

def obj_file(name):
    return '$builddir/'+ name + ".o"

def exe_file(name):
    return '$builddir/' + name

def app_exe_file(appname, filename):
    return '$builddir/' + appname + '.app/Contents/MacOS/' + filename

def dylib_file(name):
    return '$builddir/' + name + '.dylib'

def app_bundle(name):
    return '$builddir/' + name + '.app'

def extend_cflags(additional_flags):
    return CFLAGS + ' ' + additional_flags

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
if PLATFORM == 'osx':
    N.comment('build sketches for %s' % PLATFORM)
    N.newline()

    N.variable('builddir', 'build_osx')
    N.variable('cc', 'clang')
    N.variable('cpp', 'clang++')
    N.variable('ld', '$cpp')
    N.newline()

    CPPFLAGS = " ".join([
        # Common Compiler Flags
        '-g',
        '-Wall',
        '-fcolor-diagnostics',
        # Include paths
        '-I3rdparty',
        '-I3rdparty/pcg-c-0.94/include'
        # Warnings
        '-Wno-missing-braces'
    ])

    CFLAGS = " ".join([
        '-std=c99',
    ])

    CXXFLAGS = " ".join([
        '-std=c++11',
    ])

    N.variable('cppflags', CPPFLAGS)
    N.variable('cflags', CFLAGS)
    N.variable('cxxflags', CXXFLAGS)
    N.variable('ldflags', '')
    N.newline()

    N.rule('compile',
        '$cc $cppflags $cflags -MMD -MF $out.d -c -o $out $in $ldflags',
        depfile='$out.d',
        deps='gcc')
    N.newline()

    N.rule('compile_cpp',
        '$cpp $cppflags $cxxflags -MMD -MF $out.d -c -o $out $in $ldflags',
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
        'mkdir -p $out/Contents/{MacOS,Resources,Frameworks}',
        'cp misc/Info.plist $out/Contents/Info.plist',
        "sed -i -e 's/{APP}/$name/g' $out/Contents/Info.plist",
        "rsync -aq 3rdparty/lib/SDL2.framework $out/Contents/Frameworks/"
    ]

    N.rule('app_bundle', ' && '.join(BUNDLE_COMMANDS))
    N.newline()

    N.rule('copy_file', 'cp $in $out')
    N.newline()

    # Libraries
    N.comment('nanovg')
    N.build(obj_file('nanovg'), 'compile', dep_file('nanovg'))
    N.newline()

    N.comment('gl3w')
    N.build(obj_file('gl3w'), 'compile', dep_file('gl3w'))
    N.newline()

    N.comment('imgui')
    N.build(obj_file('imgui'), 'compile_cpp', cpp_dep_file('imgui'))
    N.newline()

    N.comment('imgui_draw')
    N.build(obj_file('imgui_draw'), 'compile_cpp', cpp_dep_file('imgui_draw'))
    N.newline()

    N.comment('imgui_demo')
    N.build(obj_file('imgui_demo'), 'compile_cpp', cpp_dep_file('imgui_demo'))
    N.newline()

    N.comment('imgui_impl_sdl_gl3')
    N.build(obj_file('imgui_impl_sdl_gl3'), 'compile_cpp', cpp_dep_file('imgui_impl_sdl_gl3'))
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

    N.comment('test_steve_tools')
    N.build(obj_file('test_steve_tools'), 'compile', c_file('test_steve_tools'))
    N.build(exe_file('test_steve_tools'), 'link_exe', obj_file('test_steve_tools'))
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
    N.build(app_bundle('sdl2_example'), 'app_bundle', variables={'name': 'sdl2_example'})
    N.build(obj_file('sdl2_example'), 'compile', c_file('sdl2_example'))
    N.build(exe_file('sdl2_example'), 'link_sdl2', [obj_file('sdl2_example'), obj_file('nanovg'), obj_file('gl3w')])
    N.build(app_exe_file('sdl2_example', 'sdl2_example'), 'copy_file', exe_file('sdl2_example'))

    # Imgui c++ example
    N.comment('imgui_example')
    N.build(app_bundle('imgui_example'), 'app_bundle', variables={'name': 'imgui_example'})
    N.build(obj_file('imgui_example_lib'), 'compile_cpp', cpp_file('imgui_example_lib'))
    N.build(dylib_file('libimguiexample'), 'link_dylib', obj_file('imgui_example_lib'))
    N.build(obj_file('imgui_example'), 'compile_cpp', cpp_file('imgui_example'))
    N.build(exe_file('imgui_example'), 'link_sdl2', [
        obj_file('imgui_example'),
        obj_file('gl3w'),
        obj_file('imgui'),
        obj_file('imgui_draw'),
        obj_file('imgui_demo'),
        obj_file('imgui_impl_sdl_gl3')])
    N.build(app_exe_file('imgui_example', 'libimguiexample.dylib'), 'copy_file', exe_file('libimguiexample.dylib'))
    N.build(app_exe_file('imgui_example', 'imgui_example'), 'copy_file', exe_file('imgui_example'))

    # Try pcg
    N.comment('dumb')
    N.build(app_bundle('dumb'), 'app_bundle', variables={'name': 'dumb'})
    N.build(obj_file('dumb_platform'), 'compile_cpp', cpp_file('dumb_platform'))
    N.build(obj_file('dumb'), 'compile_cpp', cpp_file('dumb'))
    N.build(exe_file('dumb'), 'link_sdl2', [
        obj_file('dumb_platform'),
        obj_file('dumb'),
        obj_file('gl3w'),
        obj_file('imgui'),
        obj_file('imgui_draw'),
        obj_file('imgui_demo'),
        obj_file('imgui_impl_sdl_gl3')
        ], variables={
        'ldflags': '-L3rdparty/lib -lpcg_random'
    })
    N.build(app_exe_file('dumb', 'dumb'), 'copy_file', exe_file('dumb'))

elif PLATFORM == 'linux':
    N.comment('build sketches for %s' % PLATFORM)
    N.newline()

    N.variable('builddir', 'build_linux')
    N.variable('cc', 'clang')
    N.variable('cpp', 'clang++')
    N.variable('ld', '$cpp')
    N.newline()

    CPPFLAGS = " ".join([
        # Common Compiler Flags
        '-g',
        '-Wall',
        '-fcolor-diagnostics',
        # Include paths
        '-I3rdparty',
        # Warnings
        '-Wno-missing-braces'
    ])

    CFLAGS = " ".join([
        '-std=c99',
    ])

    N.variable('cppflags', CPPFLAGS)
    N.variable('cflags', CFLAGS)
    N.variable('ldflags', '')
    N.newline()

    N.rule('compile',
        '$cc $cppflags $cflags -MMD -MF $out.d -c -o $out $in $ldflags',
        depfile='$out.d',
        deps='gcc')
    N.newline()

    N.rule('link_exe', '$ld -o $out $in $ldflags')
    N.newline()
    
    N.comment('test_linux')
    N.build(obj_file('test_linux'), 'compile', c_file('test_linux'))
    N.build(exe_file('test_linux'), 'link_exe', obj_file('test_linux'))
    N.newline()

    N.comment('raytracer')
    N.build(obj_file('raytracer'), 'compile', c_file('raytracer'))
    N.build(exe_file('raytracer'), 'link_exe', obj_file('raytracer'))
    N.newline()

    N.comment('test_steve_tools')
    N.build(obj_file('test_steve_tools'), 'compile', c_file('test_steve_tools'))
    N.build(exe_file('test_steve_tools'), 'link_exe', obj_file('test_steve_tools'))
    N.newline()

N.close()
