require 'mkmf'

if have_header('linux/inotify.h') &&
		( have_header('asm-generic/ioctls.h') || have_header('asm-x86_64/ioctls.h') || have_header('asm-i386/ioctls.h') )
	create_makefile 'inotify'
end
