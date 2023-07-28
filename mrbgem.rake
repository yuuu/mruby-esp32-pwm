MRuby::Gem::Specification.new('mruby-esp32-pwm') do |spec|
  spec.license = 'MIT'
  spec.author  = 'yuuu'
  spec.summary = 'PWM class on ESP32'

  spec.add_dependency('mruby-rational', :core => 'mruby-rational')
end
