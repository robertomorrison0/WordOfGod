project('biblebolls', 'c', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

gtk = dependency('gtk4', required: true)
jsoncpp = dependency('jsoncpp', required: true)
adwaita = dependency('libadwaita-1', required: true)
libcurl = dependency('libcurl', required: true)

src = ['resources/resources.c']
incdir = include_directories('include')

executable('biblebolls',
           'main.cpp', 
           include_directories : incdir,
           sources : src,
           dependencies : [gtk,jsoncpp,adwaita,libcurl],
           install : true)
