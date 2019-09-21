require 'mkmf'

dir_config('vterm')
have_header('vterm.h')
have_library('vterm', 'vterm_new')
create_makefile('vterm')
$CFLAGS = `pkg-config --cflags gtk+-3.0`
$LDFLAGS = `pkg-config --libs gtk+-3.0`
