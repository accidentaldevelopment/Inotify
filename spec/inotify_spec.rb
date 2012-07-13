require 'spec_helper'
require 'fileutils'
include FileUtils

describe Inotify do
  it 'should include the Inotify::Constants module' do
    Inotify.should include(Inotify::Constants)
  end
  
  context 'Inotify objects' do
    before(:each){ @inotify = Inotify.new }
    
    it 'should add watchers with #add_watcher' do
      @inotify.add_watcher(Dir.getwd, Inotify::CREATE)
      @inotify.watches.should == [1]
    end
    
    it 'should remove watchers with #rm_watcher' do
      @inotify.add_watcher(Dir.getwd, Inotify::CREATE)
      @inotify.rm_watcher(1)
      @inotify.watches.should == []
    end
    
    # This is failing in rhel5.  I believe it's due to a difference in the way 
    # ONESHOT and IGNORED are handled in particular versions of the kernel
    it 'removes ONESHOT watchers after an event fires' do
      @inotify.add_watcher('/etc/hosts', Inotify::ACCESS | Inotify::ONESHOT)
      @inotify.watches.should have(1).item # sanity check
      File.read('/etc/hosts') # ACCESS hosts file
      @inotify.read while @inotify.ready? # make sure I read all events
      @inotify.watches.should be_empty
    end
  end
end
