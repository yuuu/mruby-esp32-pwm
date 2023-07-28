# mruby-esp32-pwm

PWM library for mruby-esp32.

## Installation

You need [esp-idf v5.0](https://docs.espressif.com/projects/esp-idf/en/release-v5.0/esp32/index.html) to use this mrbgems.

Add the line below to your `build_config.rb`:

```ruby
  conf.gem :github => 'yuuu/mruby-esp32-pwm'
```

## Examples

```ruby
pwm = PWM.new(1)  # GPIO pin = 1
pwm.frequency(50) # set frequency(Hz) of PWM
pwm.duty(10)      # set duty(%) of PWM
# => drive with frequency=50Hz(Period=20_000us), duby=10%(Pulse Width=2000us)

pwm = PWM.new(1, freq: 100, duty: 40)
# => drive with frequency=100Hz(Period=10_000us), duby=40%(Pulse Width=4000us)

pwm = PWM.new(1)
pwm.period_us(25_000)
pwm.pulse_width_us(5_000)
# => drive with frequency=40Hz(Period=25_000us), duby=20%(Pulse Width=5000us)
```

## LICENSE

MIT License.
