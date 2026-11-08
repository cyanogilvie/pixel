pandoc = find_program('pandoc', native: true, required: false)

if pandoc.found()
  doc_md = configure_file(
    input:          meson.project_name() + '.md.in',
    output:         '@BASENAME@',
    configuration:  teaconf,
  )

  doc_targets = []

  doc_targets += custom_target('manpage',
    input:    doc_md,
    output:   meson.project_name() + '.n',
    command:  [pandoc, '--standalone', '--from', 'markdown', '--to', 'man',
      '@INPUT@', '--output', '@OUTPUT@'],
    install:          not meson.is_subproject(),
    install_dir:      tcl_dep.get_variable('prefix') / 'share' / 'man' / 'mann',
    install_tag:      'man',
    build_by_default: true,
  )

  doc_targets += custom_target('readme',
    input:    doc_md,
    output:   'README.md',
    command:  [pandoc, '--from', 'markdown', '--to', 'gfm',
      '@INPUT@', '--output', '@OUTPUT@'],
    install_dir:      meson.project_source_root(),
    install_tag:      'source',
    build_by_default: false,
  )

  alias_target('doc', doc_targets)
endif
