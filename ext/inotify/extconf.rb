require 'mkmf'

# This header is required.  Without it, there's nothing to do
if have_header('sys/inotify.h')
  create_header     # generate extconf.h
  create_makefile 'inotify'
end
