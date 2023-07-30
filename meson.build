project('gtk_app', 'c', 
  version : '0.1',
  default_options : ['warning_level=1', 'cpp_std=c++17'])

glib = dependency('glib-2.0', required: true)
gtk = dependency('gtk4', required: true)
adwaita = dependency('libadwaita-1', required: true)
json = dependency('json-c', required: true)
curl = dependency('libcurl', required: true)

sources = ['main.c']
gnome=import('gnome')
i18n = import('i18n')

gettext_package = meson.project_name()

subdir('src')
subdir('po')
subdir('data')

config_h = configuration_data()
#config_h.set_quoted('APP_ID', app_id)
config_h.set_quoted('DATA_DIR', './data')
config_h.set_quoted('LOCALE_DIR', './po')
config_h.set_quoted('GETTEXT_PACKAGE', gettext_package)

configure_file(
         output: 'config.h',
  configuration: config_h,
)

configuration_inc = include_directories('.')

executable('app', resources, schemas,
           include_directories : ['include', configuration_inc],
           sources : sources,
           dependencies : [gtk,adwaita,json,curl],
           )


