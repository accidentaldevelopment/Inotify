#include "ruby.h"
#include "ruby/io.h"
#include <linux/inotify.h>
#include <asm/ioctls.h>

#define VERSION "0.1.0"

// Max size for read(2) calls.  There's probably a better way to do this
#define BUF_SIZE (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
// I can never remember the proper name for the method, so here's a macro!
#define CSTR2STR(cstr) rb_tainted_str_new2(cstr)

// class Notify < IO
static VALUE rb_cInotify;
// module Notify::Constants
static VALUE rb_mInotifyConstants;
// class Notify::Event < Object
static VALUE rb_cEvent;

/* 
 * Instatiates a new Inotify object
 * @return [Inotify] A new Inotify instance
 */
static VALUE rb_inotify_init(VALUE self) {
  int fd = inotify_init();
  if(fd == -1)
    rb_sys_fail("inotify_init");
  rb_iv_set(self, "@watches", rb_ary_new());
  VALUE fd_array = rb_ary_new3(1, INT2FIX(fd));
  VALUE rb_fd = INT2FIX(fd);
  rb_call_super(1, &rb_fd);
  return self;
}

/*
 * Adds a new file or directory to be watched for events
 * @param [String] path The path to watch.  Can be a file or directory
 * @param [Fixnum] mask The mask of events watch for.  Multiple events can be ORed together.  Inotify::Constants contains all the events available.
 * @return [Fixnum] The watch descriptor integer of the newly added descriptor
 */
static VALUE rb_inotify_add(VALUE self, VALUE path, VALUE mask) {
  int wd;
  rb_io_t *fptr;
  GetOpenFile(self, fptr);
  wd = inotify_add_watch(fptr->fd, StringValueCStr(path), NUM2UINT(mask));
  if(wd == -1)
    rb_sys_fail("inotify_add_watch");
  rb_ary_push(rb_iv_get(self, "@watches"), INT2FIX(wd));
  return INT2FIX(wd);
}

/*
 * Removes the specified watch descriptor.
 * @param [Fixnum] wd The number of the watch descriptor to remove
 */
static VALUE rb_inotify_rm(VALUE self, VALUE wd) {
  rb_io_t *fptr;
  GetOpenFile(self, fptr);
  if( inotify_rm_watch(fptr->fd,FIX2INT(wd)) )
    rb_sys_fail("inotify_rm_watch");
  rb_ary_delete(rb_iv_get(self,"@watches"), wd);
  return Qnil;
}

/*
 * This function takes the event mask and loops through the constants in Inotify::Constants
 * Returns a symbol for the first event found.  This won't work.  Multiple events can be in one Event
 * @param  [uint32_t] mask The event mask
 * @return [Symbol] A symbol that matches the name of the event that was masked, not including ALL_EVENTS
 */
static VALUE rb_event_detect_types(uint32_t mask) {
  VALUE constants = rb_const_list(rb_mod_const_at(rb_mInotifyConstants,0));
  int i;
  ID id;
  long const_val;
  VALUE types = rb_ary_new();
  for(i=0; i<RARRAY_LEN(constants); i++) {
    id = SYM2ID(RARRAY_PTR(constants)[i]);
    const_val = NUM2LONG(rb_const_get(rb_mInotifyConstants,id));
    if( const_val == IN_ALL_EVENTS ) continue;
    if( mask & const_val )
      rb_ary_push(types,ID2SYM(id));
  }
  return types;
}

/*
 * Creates and returns a new Inotify::Event object based on the supplied struct inotify_event
 */
static VALUE rb_event_setup(struct inotify_event *ev) {
  VALUE notify_event = rb_class_new_instance(0, NULL, rb_cEvent);
  rb_iv_set(notify_event, "@wd", INT2FIX(ev->wd));
  rb_iv_set(notify_event, "@mask", UINT2NUM(ev->mask));
  rb_iv_set(notify_event, "@cookie", UINT2NUM(ev->cookie));
  rb_iv_set(notify_event, "@types", rb_event_detect_types(ev->mask));
  if( ev->len > 0 )
    rb_iv_set(notify_event, "@path", CSTR2STR(ev->name));
  else
    rb_iv_set(notify_event, "@path", rb_str_new("",0));
  return notify_event;
}

/*
 * Returns true if there's an event in the queue to be read.  False otherwise
 * @return [boolean] A boolean indicating if anything is available to be read
 */
static VALUE rb_inotify_ready(VALUE self) {
  rb_io_t *fptr;
  GetOpenFile(self, fptr);
  int bytes_available;
  if( ioctl(fptr->fd, FIONREAD, &bytes_available) )
    rb_sys_fail("ioctl");
  return (bytes_available == 0 ? Qfalse : Qtrue); //INT2NUM(bytes_available));
}

/*
 * Reads in all available Inotify::Events and returns and array of them.
 * Blocks until something is available.
 * @return [Array<Inotify::Event>] An array of Inotify::Events
 */
static VALUE rb_inotify_read(VALUE self) {
  rb_io_t *fptr;
  GetOpenFile(self, fptr);
  ssize_t num_read;
  char buf[BUF_SIZE];
  char *p;
  struct inotify_event *ev;
  VALUE events = rb_ary_new();

  if( (num_read = read(fptr->fd, buf, BUF_SIZE)) < 0 ) 
    rb_sys_fail("read");
  for(p=buf; p<buf + num_read; ) {
    ev = (struct inotify_event *)p;
    rb_ary_push(events,rb_event_setup(ev));
    p += sizeof(struct inotify_event) + ev->len;
  }
  return events;
}

/*
 * @return [Boolean] *true* if the event is a directory event.  *false* otherwise
 */
static VALUE rb_cEvent_is_dir(VALUE self) {
  uint32_t mask = NUM2UINT(rb_iv_get(self,"@mask"));
  if( mask & IN_ISDIR )
    return Qtrue;
  return Qfalse;
}

void Init_inotify() {
  rb_cInotify = rb_define_class("Inotify", rb_cIO);
  rb_define_const(rb_cInotify, "VERSION", rb_str_new2(VERSION));
  rb_define_method(rb_cInotify, "initialize", rb_inotify_init, 0);
  rb_define_method(rb_cInotify, "add_watcher", rb_inotify_add, 2);
  rb_define_method(rb_cInotify, "rm_watcher", rb_inotify_rm, 1);
  rb_define_method(rb_cInotify, "read", rb_inotify_read, 0);
  rb_define_method(rb_cInotify, "ready?", rb_inotify_ready, 0);
  rb_define_attr(rb_cInotify, "watches", 1,0);

  rb_mInotifyConstants = rb_define_module_under(rb_cInotify, "Constants");
  rb_define_const(rb_mInotifyConstants, "ACCESS",        LONG2NUM(IN_ACCESS));
  rb_define_const(rb_mInotifyConstants, "MODIFY",        LONG2NUM(IN_MODIFY));
  rb_define_const(rb_mInotifyConstants, "ATTRIB",        LONG2NUM(IN_ATTRIB));
  rb_define_const(rb_mInotifyConstants, "CLOSE_WRITE",   LONG2NUM(IN_CLOSE_WRITE));
  rb_define_const(rb_mInotifyConstants, "CLOSE_NOWRITE", LONG2NUM(IN_CLOSE_NOWRITE));
  rb_define_const(rb_mInotifyConstants, "OPEN",          LONG2NUM(IN_OPEN));
  rb_define_const(rb_mInotifyConstants, "MOVED_FROM",    LONG2NUM(IN_MOVED_FROM));
  rb_define_const(rb_mInotifyConstants, "MOVED_TO",      LONG2NUM(IN_MOVED_TO));
  rb_define_const(rb_mInotifyConstants, "CREATE",        LONG2NUM(IN_CREATE));
  rb_define_const(rb_mInotifyConstants, "DELETE",        LONG2NUM(IN_DELETE));
  rb_define_const(rb_mInotifyConstants, "DELETE_SELF",   LONG2NUM(IN_DELETE_SELF));
  rb_define_const(rb_mInotifyConstants, "MOVE_SELF",     LONG2NUM(IN_MOVE_SELF));
  rb_define_const(rb_mInotifyConstants, "UNMOUNT",       LONG2NUM(IN_UNMOUNT));
  rb_define_const(rb_mInotifyConstants, "Q_OVERFLOW",    LONG2NUM(IN_Q_OVERFLOW));
  rb_define_const(rb_mInotifyConstants, "IGNORED",       LONG2NUM(IN_IGNORED));
  rb_define_const(rb_mInotifyConstants, "CLOSE",         LONG2NUM(IN_CLOSE));
  rb_define_const(rb_mInotifyConstants, "MOVE",          LONG2NUM(IN_MOVE));
  rb_define_const(rb_mInotifyConstants, "ONLYDIR",       LONG2NUM(IN_ONLYDIR));
  rb_define_const(rb_mInotifyConstants, "DONT_FOLLOW",   LONG2NUM(IN_DONT_FOLLOW));
#ifdef IN_EXCL_UNLINK // Added in 2.6.36.  Not in rhel5 or 6
  rb_define_const(rb_mInotifyConstants, "EXCL_UNLINK",   LONG2NUM(IN_EXCL_UNLINK));
#endif
  rb_define_const(rb_mInotifyConstants, "MASK_ADD",      LONG2NUM(IN_MASK_ADD));
  rb_define_const(rb_mInotifyConstants, "ISDIR",         LONG2NUM(IN_ISDIR));
  rb_define_const(rb_mInotifyConstants, "ONESHOT",       LONG2NUM(IN_ONESHOT));
  rb_define_const(rb_mInotifyConstants, "ALL_EVENTS",    LONG2NUM(IN_ALL_EVENTS));
  rb_include_module(rb_cInotify, rb_mInotifyConstants);

  rb_cEvent = rb_define_class_under(rb_cInotify, "Event", rb_cObject);
  rb_define_method(rb_cEvent, "dir?", rb_cEvent_is_dir, 0);
  rb_define_attr(rb_cEvent, "wd", 1,0);
  rb_define_attr(rb_cEvent, "mask", 1,0);
  rb_define_attr(rb_cEvent, "cookie", 1,0);
  rb_define_attr(rb_cEvent, "types", 1,0);
  rb_define_attr(rb_cEvent, "path", 1,0);
}
