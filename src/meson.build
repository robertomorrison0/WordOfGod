wordofgod_sources = [
  'main.c',
  'bible-application.c',
  'bible-window.c',
]

wordofgod_deps = [
  dependency('gtk4'),
  dependency('libadwaita-1', version: '>= 1.2'),
]

wordofgod_sources += gnome.compile_resources('wordofgod-resources',
  'wordofgod.gresource.xml',
  c_name: 'wordofgod'
)

executable('wordofgod', wordofgod_sources,
  dependencies: wordofgod_deps,
       install: true,
)
