project('gtk_bible', 'c', 
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

glib = dependency('glib-2.0', required: true)
gtk = dependency('gtk4', required: true)
adwaita = dependency('libadwaita-1', required: true)
json = dependency('json-c', required: true)
curl = dependency('libcurl', required: true)

sources = ['main.c']
subdir('src')
i18n = import('i18n')
subdir('po')

gnome=import('gnome')



subdir('data')


executable('app', resources, schemas,
           include_directories : ['include'],
           sources : sources,
           dependencies : [gtk,adwaita,json,curl],
           )

