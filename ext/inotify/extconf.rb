require 'mkmf'

# This header is required.  Without it, there's nothing to do
if have_header('sys/inotify.h')
  have_type 'const' # check for const.  If it's there we won't use #define for 
                    # what should be constants
  create_header     # generate extconf.h
  
  create_makefile 'inotify'
end
