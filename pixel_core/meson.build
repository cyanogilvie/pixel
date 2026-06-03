project('pixel', 'c',
  version:          '4.1',
  default_options:  ['c_std=c2y,c23,c2x', 'warning_level=3', 'werror=true'],
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

tea_want_shims  = ['tip445']
tea_want_defer  = true

deps = [cc.find_library('m')]
inc = [include_directories('generic')]

# Stubs generation: reads generic/pixel.decls, emits pixelStubInit.c and
# pixelDecls.h into the build dir.
tea_genstubs = files('generic/pixel.decls', 'generic/pixelStubLib.c')

subdir('teabase/init')

pkg_sources = files(
  'generic/main.c',
  'generic/2d.c',
  'generic/bezierstuff.c',
  'generic/primitives.c',
  'generic/rle.c',
  'generic/shaders.c',
  'generic/tcl_pmap.c',
  'generic/pmapf.c',
  'generic/misc.c',
  'generic/scale_pmap.c',
  'generic/lanczos.c',
  'generic/perceptual_scaling.c',
)

conf.set_quoted('PIXEL_LIBRARY', meson.project_name())
conf.set_quoted('PIXEL_STUBLIB', meson.project_name() + 'stub')
conf.set_quoted('PIXEL_HEADER',  'pixel.h')

teaconf_files = files(
  'library/utils.tcl',
  'generic/pixel.h.in',
)

subdir('teabase')

# Headers transitively included by pixel.h that consumers (pixel_jpeg, etc.)
# need in the same directory so their `#include <pixel.h>` resolves.
install_headers(
  'generic/2d_types.h',
  'generic/pmapf.h',
  'generic/perceptual_scaling.h',
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
