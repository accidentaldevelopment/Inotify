require 'mkmf'

# These two headers are required.  Without them, there's nothing to do
if have_header('linux/inotify.h') && have_header('asm/ioctls.h')
  have_type 'const' # check for const.  If it's there we won't use #define for 
                    # what should be constants
  create_header     # generate extconf.h
  
  create_makefile 'inotify'
end
