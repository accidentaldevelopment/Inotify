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
  end
end
