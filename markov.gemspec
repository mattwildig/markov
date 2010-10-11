
Gem::Specification.new do |s|
  s.name = "markov"
  s.version = '0.0.2'
  s.author = 'Matt Wildig'
  s.email = 'matt@mattwildig.co.uk'
  s.homepage = 'http://github.com/mattwildig/markov'
  s.summary = "A Simple Markov Chain generator"
  s.files = Dir.glob("{lib,ext}/**/*.{c,h,rb}")
  s.extensions << 'ext/extconf.rb'
end