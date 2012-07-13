require 'mkmf'

# This header is required.  Without it, there's nothing to do
if have_header('sys/inotify.h')
  have_struct_member('struct rb_io_t', 'fd', 'ruby/io.h')
  $defs << '-DUSE_SYMBOL_AS_CONSTANT_NAME' if Process.constants[0].class == Symbol
  
  create_header     # generate extconf.h
  create_makefile 'inotify'
end
