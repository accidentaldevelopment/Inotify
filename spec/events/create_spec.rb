require 'spec_helper'

describe Inotify do
  before{ mkdir 'sandbox' }
  after { rm_rf 'sandbox' }

  context 'CREATE' do
    before{ @create = Inotify.new }

    context 'Directory events' do
      before do
        @create.add_watcher('sandbox', Inotify::CREATE)
      end

      it 'should fire a non-directory event when a subfile is created' do
        touch 'sandbox/file1'
        events = @create.read
        events.should have(1).item
        event = events.first
        event.path.should == 'file1'
        event.should_not be_dir
        event.types.should == [:CREATE]
      end

      it 'should fire a directory event when a subdirectory is created' do
        mkdir 'sandbox/dir1'
        events = @create.read
        events.should have(1).item
        event = events.first
        event.path.should == 'dir1'
        event.should be_dir
        event.types.should == [:CREATE, :ISDIR]
      end
    end
    
    context 'File Events' do
      it 'should raise a ENOENT if the CREATE mask is added for a file' do
        lambda{ @create.add_watcher('sandbox/file1', Inotify::CREATE) }.should raise_error(Errno::ENOENT)
      end
    end
  end
end
