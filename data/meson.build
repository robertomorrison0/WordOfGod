
desktop_file_in_config = configuration_data()
desktop_file_in_config.set('icon', app_id)
desktop_file_in = configure_file(
          input: 'org.robertomorrison.wordofgod.desktop.in',
         output: '@0@.desktop.in'.format(app_id),
  configuration: desktop_file_in_config,
)



desktop_file = i18n.merge_file(
        input: desktop_file_in,
       output: '@0@.desktop'.format(app_id),
         type: 'desktop',
       po_dir: '../po',
      install: true,
  install_dir: join_paths(get_option('datadir'), 'applications')
)

desktop_utils = find_program('desktop-file-validate', required: true)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils, args: [desktop_file.full_path()])
endif

appstream_file = i18n.merge_file(
        input: 'org.robertomorrison.wordofgod.appdata.xml.in',
       output: 'org.robertomorrison.wordofgod.appdata.xml',
       po_dir: '../po',
      install: true,
  install_dir: join_paths(get_option('datadir'), 'appdata')
)

appstream_util = find_program('appstream-util', required: false)
if appstream_util.found()
  test('Validate appstream file', appstream_util, args: ['validate', appstream_file])
endif

install_data('org.robertomorrison.wordofgod.gschema.xml',
  install_dir: join_paths(get_option('datadir'), 'glib-2.0/schemas')
)

compile_schemas = find_program('glib-compile-schemas', required: false)
if compile_schemas.found()
  test('Validate schema file',
       compile_schemas,
       args: ['--strict', '--dry-run', meson.current_source_dir()])
endif

subdir('icons')
