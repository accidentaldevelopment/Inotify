require 'spec_helper'

describe Inotify do
	before{ mkdir 'sandbox' }
  after { rm_rf 'sandbox' }

	context 'OPEN' do
		before{ @open = Inotify.new }

		context 'Directory events' do
	    before do
	      @open.add_watcher('sandbox', Inotify::OPEN)
	      Dir.open('sandbox').close
	    end
	
	    it 'should return one event when the dir is opened' do
	      events = @open.read
	      events.length.should == 1
	      event = events.first
	      event.wd.should == 1
	      event.cookie.should == 0
	      event.path.should be_empty
	      event.types.should == [:OPEN, :ISDIR]
				event.should be_dir
	    end
	
	    it 'should return two events if something inside the dir is opened' do
	      mkdir('sandbox/dir1')
	      Dir.open('sandbox/dir1').close
	      events = @open.read
	      events.should have(2).items
	      event = events.last
	      event.path.should == 'dir1'
	      event.types.should == [:OPEN, :ISDIR]
	    end
	  end
		
		context 'File Events' do
			before do
				touch 'sandbox/file1'
	      @open.add_watcher('sandbox/file1', Inotify::OPEN)
			end

			it 'should return one event when a file is opened' do
				open('sandbox/file1').close
				events = @open.read
				events.should have(1).item
				event = events.first
				event.path.should be_empty
				event.types.should == [:OPEN]
				event.should_not be_dir
			end
		end
	end
end
