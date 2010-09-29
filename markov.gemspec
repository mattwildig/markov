
Gem::Specification.new do |s|
  s.name = "Markov"
  s.version = '0.0.1'
  s.summary = "Simple Markov Chain generator"
  s.require_paths << 'ext'
  s.files = Dir.glob("{lib,ext}/**/*.{c,h,rb}")
  s.extensions << 'ext/extconf.rb'
end