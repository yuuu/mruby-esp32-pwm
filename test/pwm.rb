assert('PWM.new') do
  pwm = PWM.new(1)
  assert_equal(1, pwm.pin)
  assert_equal(0, pwm.pulse_width_ticks)
  assert_equal(1_000_000, pwm.period_ticks)

  pwm = PWM.new(1, freq: 50, duty: 50)
  assert_equal(pwm.pin, 1)
  assert_equal(pwm.pulse_width_ticks, 10_000)
  assert_equal(pwm.period_ticks, 20_000)
end

assert('PWM#freq') do
  pwm = PWM.new(1)
  pwm.frequency(50)
  assert_equal(pwm.pulse_width_ticks, 0)
  assert_equal(pwm.period_ticks, 20_000)
end

assert('PWM#period_us') do
  pwm = PWM.new(1)
  pwm.period_us(5000)
  assert_equal(pwm.pulse_width_ticks, 0)
  assert_equal(pwm.period_ticks, 5_000)
  pwm.period_us(2000)
  assert_equal(pwm.pulse_width_ticks, 0)
  assert_equal(pwm.period_ticks, 2_000)
end

assert('PWM#duty') do
  pwm = PWM.new(1)
  pwm.duty(20)
  assert_equal(pwm.pulse_width_ticks, 200_000)
  assert_equal(pwm.period_ticks, 1_000_000)

  pwm = PWM.new(1, freq: 20)
  pwm.duty(20)
  assert_equal(pwm.pin, 1)
  assert_equal(pwm.pulse_width_ticks, 10_000)
  assert_equal(pwm.period_ticks, 50_000)
end

assert('PWM#pulse_width_us') do
  pwm = PWM.new(1, freq: 100)
  pwm.pulse_width_us(1)
  assert_equal(pwm.pulse_width_ticks, 1)
  assert_equal(pwm.period_ticks, 10_000)

  pwm = PWM.new(1, freq: 50)
  pwm.pulse_width_us(20)
  assert_equal(pwm.pulse_width_ticks, 20)
  assert_equal(pwm.period_ticks, 20_000)
end
