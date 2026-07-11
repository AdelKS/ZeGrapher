qt6 = import('qt6')

# the referenced .qrc file contains directly the compiled .qm names
# the .ts counterparts are expected to live alongside this meson.build file
compiled_translations = qt6.compile_translations(
  qresource: 'translations.qrc'
)
