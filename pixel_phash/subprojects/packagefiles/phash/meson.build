project('pHash', 'cpp',
    version: '1.0.0',
    license: 'GPL-3.0-or-later',
    default_options: [
        'cpp_std=c++14',
        'warning_level=1',
        'default_library=shared',
    ],
    meson_version: '>=0.56.0',
)

cxx = meson.get_compiler('cpp')
host_sys = host_machine.system()

with_audio = get_option('with-audio-hash')
with_video = get_option('with-video-hash')
with_examples = get_option('build-examples')

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------

# Image deps are always required (HAVE_IMAGE_HASH is unconditional).
png_dep    = dependency('libpng')
zlib_dep   = dependency('zlib')
jpeg_dep   = dependency('libjpeg', required: false)
if not jpeg_dep.found()
    jpeg_dep = cxx.find_library('jpeg')
endif
tiff_dep   = dependency('libtiff-4', required: false)
if not tiff_dep.found()
    tiff_dep = cxx.find_library('tiff')
endif

threads_dep = dependency('threads')

base_deps = [png_dep, zlib_dep, jpeg_dep, tiff_dep, threads_dep]

# libmpg123 is an optional upstream toggle; when audio hashing is on, the
# audio sources #include <mpg123.h> unconditionally so it becomes required.
mpg123_dep = dependency('libmpg123', required: with_audio)
have_mpg123 = mpg123_dep.found()
if have_mpg123
    base_deps += mpg123_dep
endif

audio_deps = []
if with_audio
    audio_deps = [
        dependency('sndfile'),
        dependency('samplerate'),
        dependency('vorbis'),
        dependency('vorbisenc'),
        dependency('ogg'),
    ]
endif

video_deps = []
if with_video
    video_deps = [
        dependency('libavcodec'),
        dependency('libavformat'),
        dependency('libswscale'),
        dependency('libavutil'),
    ]
endif

# -----------------------------------------------------------------------------
# Generated pHash.h
# -----------------------------------------------------------------------------

conf = configuration_data()
conf.set('HAVE_IMAGE_HASH',    true)
conf.set('HAVE_AUDIO_HASH',    with_audio)
conf.set('HAVE_VIDEO_HASH',    with_video)
conf.set('HAVE_LIBMPG123',     have_mpg123)
conf.set('HAVE_PTHREAD',       threads_dep.found())
conf.set('HAVE_SYS_SYSCTL_H',  cxx.has_header('sys/sysctl.h'))
conf.set_quoted('PACKAGE_STRING', meson.project_name())

phash_h = configure_file(
    input: 'src/pHash.h.in',
    output: 'pHash.h',
    configuration: conf,
)

# -----------------------------------------------------------------------------
# Include paths
# -----------------------------------------------------------------------------

# CImg is header-only and shipped in-tree.
cimg_inc = include_directories('third-party/CImg')

# Public include dir (where the generated pHash.h lands and where the original
# headers live for internal/example use).
public_inc = include_directories('src', '.')

# -----------------------------------------------------------------------------
# Library sources
# -----------------------------------------------------------------------------

lib_sources = [phash_h, 'src/pHash.cpp']

if with_audio
    lib_sources += ['src/audiophash.cpp', 'src/ph_fft.cpp']
endif
if with_video
    lib_sources += ['src/cimgffmpeg.cpp']
endif

extra_cpp_args = []
extra_inc = []
if host_sys == 'windows'
    lib_sources += [
        'phash-win32/src/mman.cpp',
        'phash-win32/src/dirent.c',
    ]
    extra_cpp_args += ['-D_EXPORTING']
    extra_inc += [include_directories('phash-win32/src')]
endif

phash_lib = library('pHash',
    lib_sources,
    version: meson.project_version(),
    include_directories: [public_inc, cimg_inc] + extra_inc,
    dependencies: base_deps + audio_deps + video_deps,
    cpp_args: extra_cpp_args,
    install: true,
)

# -----------------------------------------------------------------------------
# Public headers and pkg-config
# -----------------------------------------------------------------------------

install_headers(phash_h)

pkg = import('pkgconfig')
pkg.generate(phash_lib,
    name: 'pHash',
    description: 'Perceptual hashing library',
    version: meson.project_version(),
    libraries: ['-lm'],
)

# -----------------------------------------------------------------------------
# Dependency object for subproject consumers
# -----------------------------------------------------------------------------

phash_dep = declare_dependency(
    link_with: phash_lib,
    include_directories: [public_inc, cimg_inc],
    dependencies: base_deps + audio_deps + video_deps,
)

meson.override_dependency('phash', phash_dep)
meson.override_dependency('pHash', phash_dep)

# -----------------------------------------------------------------------------
# Examples
# -----------------------------------------------------------------------------

if with_examples
    examples = {
        'TestDCT':    'examples/test_imagephash.cpp',
        'TestRadish': 'examples/test_radish.cpp',
        'TestMH':     'examples/test_mhimagehash.cpp',
    }
    if with_audio
        examples += {'TestAudio': 'examples/test_audiophash.cpp'}
    endif
    if with_video
        examples += {'TestVideoHash': 'examples/test_dctvideohash.cpp'}
    endif

    foreach name, src : examples
        executable(name, src,
            dependencies: phash_dep,
            install: true,
        )
    endforeach
endif
