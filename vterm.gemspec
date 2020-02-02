
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

  # Specify which files should be added to the gem when it is released.
  # The `git ls-files -z` loads the files in the RubyGem that have been added into git.
  spec.files         = Dir.chdir(File.expand_path('..', __FILE__)) do
    `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  end
  spec.bindir        = 'exe'
  spec.executables   = spec.files.grep(%r{^exe/}) { |f| File.basename(f) }
  spec.require_paths = ['lib']

  spec.add_development_dependency 'bundler'
  spec.add_development_dependency 'rake'
end
