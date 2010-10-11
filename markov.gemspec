
Gem::Specification.new do |s|
  s.name = "markov"
  s.version = '0.0.1'
  s.summary = "Simple Markov Chain generator"
  s.files = Dir.glob("{lib,ext}/**/*.{c,h,rb}")
  s.extensions << 'ext/extconf.rb'
  s.required_ruby_version = '~> 1.8.7'
end