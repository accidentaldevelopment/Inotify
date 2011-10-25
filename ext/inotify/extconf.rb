require 'mkmf'

if have_header('linux/inotify.h') && have_header('asm/ioctls.h')
  create_makefile 'inotify'
end
