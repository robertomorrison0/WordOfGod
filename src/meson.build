wordofgod_sources = [
  'main.c',
  'app.c',
  'window.c',
  'preferences_window.c',
  'text_page.c',
  'color_selector.c',
  'theme_selector.c',
  'line_distance_selector.c',
  'bible_content.c',
  'scroll_button.c',
]

wordofgod_deps = [
  dependency('gtk4'),
  dependency('libcurl'),
  dependency('json-c'),
  dependency('libadwaita-1', version: '>= 1.2'),
]

wordofgod_sources += gnome.compile_resources('wordofgod-resources',
  'org.robertomorrison.wordofgod.gresource.xml',
  c_name: 'wordofgod'
)

executable('wordofgod',
  dependencies: wordofgod_deps,
  sources: [wordofgod_sources],
  include_directories : ['../include', '..'],
       install: true,
)

