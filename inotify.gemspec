require File.expand_path('../lib/inotify/version', __FILE__)

Gem::Specification.new do |s|
  s.name = 'inotify'
  s.version = Inotify::VERSION
  s.authors = ['Brian Faga']
  s.email = 'brian@accidentaldevelopment.com'

  s.summary = 'Ruby binding for inotify kernel module'
  s.description = <<DESC
Rather simple Ruby bindings for Linux's inotify subsystem.
DESC

  s.required_ruby_version = '>= 1.9.2'

  s.files         = `git ls-files`.split($/)
  s.executables   = s.files.grep(%r{^bin/}) { |f| File.basename(f) }
  s.test_files    = s.files.grep(%r{^(test|spec|features)/})
  s.require_paths = ["lib"]
  s.extensions << 'ext/inotify/extconf.rb'
  
  s.add_development_dependency('bundler')
  s.add_development_dependency('rake')
  s.add_development_dependency('rspec')
  s.add_development_dependency('rake-compiler')
  s.add_development_dependency('yard')
end
