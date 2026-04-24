project('Pixel', 'c',
  version:          '4.0',
  default_options:  ['c_std=c17', 'warning_level=2'],
  meson_version:    '>=1.3.0',
)

cc = meson.get_compiler('c')
fs = import('fs')
add_project_arguments('-D_DEFAULT_SOURCE', language: 'c')

if get_option('debug')
  add_project_arguments(cc.get_supported_arguments(['-ggdb3']), language: 'c')
endif

add_project_arguments('-Wno-missing-field-initializers', language: 'c')

conf = configuration_data()
conf.set10('TESTMODE', get_option('testmode'))
conf.set10('DEBUG',    get_option('debugmode'))

tea_want_shims = ['tip445']

deps = [cc.find_library('m')]
inc = [include_directories('generic')]

subdir('teabase/init')

# Stubs generation: reads generic/Pixel.decls, emits PixelStubInit.c and
# pixelDecls.h into the build dir.
deps += declare_dependency(
  sources: custom_target('genstubs',
    output:  ['pixelStubInit.c', 'pixelDecls.h'],
    command: [
      build_tclsh,
      meson.project_source_root() / 'teabase/tools/genStubs.tcl',
      meson.project_build_root(),
      meson.project_source_root() / 'generic/pixel.decls',
    ],
    install:     true,
    install_dir: [false, pkg_dir],
  ),
)

pkg_sources = files(
  'generic/main.c',
  'generic/2d.c',
  'generic/bezierstuff.c',
  'generic/primitives.c',
  'generic/rle.c',
  'generic/shaders.c',
  'generic/tcl_pmap.c',
  'generic/pmapf.c',
  'generic/tcl_utils.c',
  'generic/misc.c',
  'generic/scale_pmap.c',
  'generic/lanczos.c',
  'generic/perceptual_scaling.c',
)

conf.set_quoted('PIXEL_LIBRARY', meson.project_name())
conf.set_quoted('PIXEL_STUBLIB', meson.project_name() + 'stub')
conf.set_quoted('PIXEL_HEADER',  'pixel.h')

teaconf_files = [files('library/utils.tcl')]

subdir('teabase')

configure_file(
  configuration:  teaconf,
  input:          'generic/pixel.h.in',
  output:         '@BASENAME@',
  install_dir:    pkg_dir,
)

stubs_lib = static_library(meson.project_name() + 'stub',
  'generic/PixelStubLib.c',
  dependencies: deps,
  include_directories: inc,
  install:     true,
  install_dir: pkg_dir,
)

pc_conf = configuration_data()
pc_conf.set('PACKAGE_NAME',     meson.project_name())
pc_conf.set('PACKAGE_VERSION',  meson.project_version())
pc_conf.set('PKG_DIR_BASENAME', meson.project_name() + '-' + meson.project_version())
pc_conf.set('STUBLIB',          meson.project_name() + 'stub')

configure_file(
  input:          'pixel.pc.in',
  output:         'pixel.pc',
  configuration:  pc_conf,
  install_dir:    tcl_dep.get_variable('libdir') / 'pkgconfig',
)

# vim: foldmethod=marker foldmarker=<<<,>>> ts=2 shiftwidth=2 expandtab
