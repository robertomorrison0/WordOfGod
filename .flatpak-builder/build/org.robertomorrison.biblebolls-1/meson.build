project('biblebolls', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

gtk = dependency('gtk4', required: true)
jsoncpp = dependency('jsoncpp', required: true)
adwaita = dependency('libadwaita-1', required: true)
libcurl = dependency('libcurl', required: true)

incdir = include_directories('include')


executable('biblebolls',
           'main.cpp',
           install : true,
           include_directories : incdir,
           dependencies : [gtk,jsoncpp,adwaita,libcurl])
