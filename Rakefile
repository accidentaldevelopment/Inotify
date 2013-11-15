require 'rake/extensiontask'
require 'rspec/core/rake_task'
require 'yard'
require 'bundler/gem_tasks'

# Add the doc directory to clobbered dirs
CLOBBER << ['doc']

# Compile native code
Rake::ExtensionTask.new('inotify')

YARD::Rake::YardocTask.new do |yard|
  yard.options = ['--no-private']
end

RSpec::Core::RakeTask.new(:specs => :compile) do |r|
  r.rspec_opts = '-f d' if ENV['DOC']
  r.pattern = 'spec/**/*_spec.rb'
end

task :default => :specs
