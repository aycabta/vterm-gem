lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'vterm/version'

Gem::Specification.new do |spec|
  spec.name          = 'vterm'
  spec.version       = VTerm::VERSION
  spec.authors       = ['aycabta']
  spec.email         = ['aycabta@gmail.com']

  spec.summary       = %q{A wrapper library of libvterm}
  spec.description   = %q{A wrapper library of libvterm}
  spec.homepage      = 'https://github.com/aycabta/vterm-gem'
  spec.extensions    = %w[ext/vterm/extconf.rb]

  spec.files         = Dir['README.md', 'ext/**/*', 'lib/**/*.rb']
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']

  spec.add_development_dependency 'bundler'
  spec.add_development_dependency 'rake'
  spec.add_development_dependency 'rake-compiler'
  spec.add_development_dependency 'test-unit'
end
