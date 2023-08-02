bible_sources = [
  'main.c',
  'bible-application.c',
  'bible-window.c',
]

bible_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.2'),
]

bible_sources += gnome.compile_resources('bible-resources',
  'bible.gresource.xml',
  c_name: 'bible'
)

executable('bible', bible_sources,
  dependencies: bible_deps,
       install: true,
)
