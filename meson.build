project('Julia Set', 'c')

gtkdep = dependency('gtk4')
mdep = meson.get_compiler('c').find_library('m', required : false)

executable('julia', 'main.c', 'colours.c', c_args : ['-Wall'], dependencies : [gtkdep, mdep])
