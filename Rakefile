require 'bundler/gem_tasks'

require 'rake/extensiontask'
Rake::ExtensionTask.new('vterm')

require 'rake/testtask'
Rake::TestTask.new(:test) do |t|
  t.libs << 'lib'
  t.test_files = FileList['test/test_*.rb']
end

task :default => [:compile, :test]
