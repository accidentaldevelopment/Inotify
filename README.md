Inotify
===

This is a wrapper for Linux's inotify kernel subsystem.  It's pretty simple and probably contains a good number of bugs and memory leaks right now.

There are a few of these out there, but most seem to be rather old or for 1.8.x only.

Installation
---

Installation is pretty simple:

	1. Clone the repo
	2. rake gem
	3. gem install pkg/inotify-<version>.gem

**Note:** This will only work on Linux.  The Inotify system doesn't exist on anything else.

This library has been tested on the following systems:
	* Red Hat 5 x86_64
	* Red Hat 6 x86_64
	* Fedora 15 x86_64
It should work on a 32bit system, but I don't have one to test on.

Usage
---

First things first, I recommend reading the inotify man page.  It's the best explanation of how the system works and what each mask bit does.  This library is a relatively thin layer on top of the C API, so it's pretty similar in how it works.

	# First up, create an instance of Inotify
	inotify = Inotify.new
	
	# Add a watch for a directory and a file
	etc_watcher = inotify.add_watcher('/etc', Inotify::CREATE)  # Watches for the creation of a file or subdirectory in /etc
	etc_hosts_watcher = inotify.add_watcher('/etc/hosts' Inotify::ACCESS | Inotify::OPEN) # Watches for /etc/hosts being access or opened
	
	# To read events, use the read() instance method
	events = inotify.read
	#=> [#<Inotify::Event @wd=1, @mask=256, @cookie=0, @types=[:CREATE], @path="blah">, 
	     #<Inotify::Event @wd=2, @mask=32, @cookie=0, @types=[:OPEN], @path="">]
	
	# Close the Inotify instance
	inotify.close

A couple things to notice about this:

1. You can create as many Inotify instances as you want.

2. An Inotify instance is nothing more than a special type of file descriptor, just like a socket is a special type of file descriptor.  With that in mind, it's subject to all the same operations, including *close()*.

3. The *add_watcher* method returns the watch descriptor created by the underlying system call.  This value corresponds to the @wd instance variable.  If you want to check which watch was triggered, or delete a watch, you'll need to know the descriptor number.

4. The second parameter to *add_watcher* can be a series of the constants ORed together, you're not limited to one even per watch descriptor.

5. *read()* is a blocking call.  The application will be unresponsive until there is something to read.

6. The *ready?* instance method will tell you if there are any events to be read:
		inotify.ready? #=> true or false

In order to prevent your application from blocking, you'll probably want to use something like *ready?* or *IO.select()*.

Things Left to Do
---

So many things before this should be considered "good".

1. A better API.  The current one works, but it would be nice to add some iterators and the like.
2. I might consider defining something like *read_event()* to return Event objects and leave *read()* as the default implementation from IO.
3. More (and better) documentation.  Everything has documentation, but the usefulness of the docs I've written remains to be determined.
4. Add more things to this list

Final Notes
---

If there's one thing to take from the previous section, it's that this is still in early development and the API could change.  I'll do my best to remember to update this file as soon as it does, but no guarantees.  It will get done eventually though.

I wrote this in a rather short period of time and with the exception of the 11 or so RSpec examples I've written so far (which all pass!) I haven't tested this a whole lot.  There are probably some bugs I haven't found yet, or a memory leak or several.
