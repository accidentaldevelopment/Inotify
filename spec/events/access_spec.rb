require 'spec_helper'

describe Inotify do
  before{ mkdir 'sandbox'; mkdir 'sandbox/dir1' }
  after { rm_rf 'sandbox' }

  context 'ACCESS' do
    before do
      @access = Inotify.new
      open('sandbox/file1','w'){|f| f.puts 'data!' }
    end

    context 'Directory events' do
      before{ @access.add_watcher('sandbox', Inotify::ACCESS) }

      it 'should fire an event when a file within the directory is accessed' do
        File.read('sandbox/file1')
        events = @access.read
        events.should have(1).item
        event = events.first
        event.path.should == 'file1'
        event.should_not be_dir
        event.types.should == [:ACCESS]
      end

    end
    
    context 'File Events' do
      before{ @access.add_watcher('sandbox/file1', Inotify::ACCESS) }

      it 'should fire an event if the file is accessed' do
        File.read('sandbox/file1')
        events = @access.read
        events.should have(1).item
        event = events.first
        event.path.should be_empty
        event.should_not be_dir
        event.types.should == [:ACCESS]
      end
    end
  end
end
