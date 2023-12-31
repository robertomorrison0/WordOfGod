project('wordofgod', 'c',
          version: '0.1.0',
    meson_version: '>= 0.62.0',
  default_options: [ 'warning_level=2', 'werror=false', 'c_std=gnu11', ],
)

gnome=import('gnome')
i18n = import('i18n')
cc = meson.get_compiler('c')


gettext_package = meson.project_name()
app_id = 'org.robertomorrison.wordofgod'
developers = 'robertomorrison'
developer_name = 'Roberto Morrison'


prefix = get_option('prefix')
datadir = prefix / get_option('datadir')
iconsdir = datadir / 'icons'



config_h = configuration_data()
config_h.set_quoted('PACKAGE_VERSION', meson.project_version())
config_h.set_quoted('GETTEXT_PACKAGE', 'wordofgod')
config_h.set_quoted('LOCALE_DIR', join_paths(get_option('prefix'), get_option('localedir')))
config_h.set_quoted('PACKAGE_ICON_NAME', app_id)
config_h.set_quoted('APP_ID', app_id)
config_h.set_quoted('DEVELOPERS', developers)
config_h.set_quoted('DEVELOPER_NAME', developer_name)
config_h.set_quoted('APP_NAME', 'Word of God')
configure_file(output: 'config.h', configuration: config_h)
add_project_arguments(['-I' + meson.project_build_root()], language: 'c')

# project_c_args = []
# test_c_args = [
#   '-Wcast-align',
#   '-Wdeclaration-after-statement',
#   '-Werror=address',
#   '-Werror=array-bounds',
#   '-Werror=empty-body',
#   '-Werror=implicit',
#   '-Werror=implicit-function-declaration',
#   '-Werror=incompatible-pointer-types',
#   '-Werror=init-self',
#   '-Werror=int-conversion',
#   '-Werror=int-to-pointer-cast',
#   '-Werror=main',
#   '-Werror=misleading-indentation',
#   '-Werror=missing-braces',
#   '-Werror=missing-include-dirs',
#   '-Werror=nonnull',
#   '-Werror=overflow',
#   '-Werror=parenthesis',
#   '-Werror=pointer-arith',
#   '-Werror=pointer-to-int-cast',
#   '-Werror=redundant-decls',
#   '-Werror=return-type',
#   '-Werror=sequence-point',
#   '-Werror=shadow',
#   '-Werror=strict-prototypes',
#   '-Werror=trigraphs',
#   '-Werror=undef',
#   '-Werror=write-strings',
#   '-Wformat-nonliteral',
#   '-Wignored-qualifiers',
#   '-Wimplicit-function-declaration',
#   '-Wlogical-op',
#   '-Wmissing-declarations',
#   '-Wmissing-format-attribute',
#   '-Wmissing-include-dirs',
#   '-Wmissing-noreturn',
#   '-Wnested-externs',
#   '-Wno-cast-function-type',
#   '-Wno-dangling-pointer',
#   '-Wno-missing-field-initializers',
#   '-Wno-sign-compare',
#   '-Wno-unused-parameter',
#   '-Wold-style-definition',
#   '-Wpointer-arith',
#   '-Wredundant-decls',
#   '-Wstrict-prototypes',
#   '-Wswitch-default',
#   '-Wswitch-enum',
#   '-Wundef',
#   '-Wuninitialized',
#   '-Wunused',
#   '-fno-strict-aliasing',
#   ['-Werror=format-security', '-Werror=format=2'],
# ]

# if get_option('buildtype') != 'plain'
#   test_c_args += '-fstack-protector-strong'
# endif
# foreach arg: test_c_args
#   if cc.has_multi_arguments(arg)
#     project_c_args += arg
#   endif
# endforeach
# add_project_arguments(project_c_args, language: 'c')

subdir('data')
subdir('src')
subdir('po')


configuration_inc = include_directories('.')

deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.2'),
  dependency('glib-2.0', required: true),
  dependency('gtk4', required: true),
  dependency('libadwaita-1', required: true),
  dependency('json-c', required: true),
  dependency('libcurl', required: true),
]

executable('wordofgod', sources,
	include_directories : ['include', '/usr/include', configuration_inc],
  	dependencies: deps,
       	install: true,
)

gnome.post_install(
     glib_compile_schemas: true,
    gtk_update_icon_cache: true,
  update_desktop_database: true,
)
