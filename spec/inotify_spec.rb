require 'spec_helper'
require 'fileutils'
include FileUtils

describe Inotify do
  it 'should include the Inotify::Constants module' do
    Inotify.should include(Inotify::Constants)
  end

  context '::buffer_size' do
    it 'returns the current buffer_size' do
      Inotify.buffer_size.should == 10 * Inotify::MIN_BUFFER
    end
  end

  context '::buffer_size=' do
    it 'sets the buffer size' do
      Inotify.buffer_size = 5 * Inotify::MIN_BUFFER
      Inotify.buffer_size.should == 5 * Inotify::MIN_BUFFER
    end

    it 'raises an ArgumentError for values that are too low' do
      expect do
        Inotify.buffer_size = Inotify::MIN_BUFFER - 1
      end.to raise_error(ArgumentError, /larger than MIN_BUFFER \(#{Inotify::MIN_BUFFER}\)/)
    end
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
      @inotify.add_watcher('/etc/hosts', Inotify::OPEN | Inotify::ONESHOT)
      @inotify.watches.should have(1).item # sanity check
      open('/etc/hosts'){} # OPEN hosts file
      @inotify.read while @inotify.ready? # make sure I read all events
      @inotify.watches.should be_empty
    end
  end
end
