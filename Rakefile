require 'rake/extensiontask'
require 'rspec/core/rake_task'
require 'yard'
require 'rubygems/package_task'

GEM_SPEC = Gem::Specification.new do |s|
  s.name = 'inotify'
  s.version = File.read('ext/inotify/inotify.c')[/#define VERSION "(.*)"/,1]
  s.authors = ['Brian Faga']
  s.email = 'brian@accidentaldevelopment.com'
  s.platform = 'linux'
  s.required_ruby_version = '>= 1.9.2'
  s.files = FileList['ext/inotify/*']
  s.add_development_dependency('rspec')
  s.add_development_dependency('rake-compiler')
  s.add_development_dependency('yard')
  s.extensions << 'ext/inotify/extconf.rb'
  s.summary = 'Ruby binding for inotify kernel module'
  s.description = <<DESC
Rather simple Ruby bindings for Linux's inotify subsystem.
DESC
end

Gem::PackageTask.new(GEM_SPEC) do |pkg|
  pkg.need_tar = false
  pkg.need_zip = false
end

# Compile native code
Rake::ExtensionTask.new('inotify')

YARD::Rake::YardocTask.new do |yard|
  yard.options = ['--no-private']
end

desc 'Run specs'
RSpec::Core::RakeTask.new(:specs => :compile) do |r|
  r.rspec_opts = '-f d' if ENV['DOC']
  r.pattern = 'spec/**/*_spec.rb'
end

task :default => :specs
