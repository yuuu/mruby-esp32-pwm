class PWM
  attr_accessor :pin, :pulse_width_ticks, :period_ticks

  def initialize(pin = nil, **kwargs)
    self.pin = pin
    missing_argument_error(:pin) if self.pin.nil?

    self.period_ticks = Rational(1_000_000, (kwargs[:freq] || kwargs[:frequency] || 1)).to_i
    self.pulse_width_ticks = (self.period_ticks * Rational((kwargs[:duty] || 0), 100)).to_i

    __initialize(self.pin).__update(self.period_ticks, self.pulse_width_ticks)
  end

  def frequency(freq)
    self.period_ticks = Rational(1_000_000, freq).to_i if freq != 0

    __update(self.period_ticks, self.pulse_width_ticks)
  end

  def period_us(micro_sec)
    self.period_ticks = micro_sec

    __update(self.period_ticks, self.pulse_width_ticks)
  end

  def duty(percent)
    self.pulse_width_ticks = (self.period_ticks * Rational(percent, 100)).to_i

    __update(self.period_ticks, self.pulse_width_ticks)
  end

  def pulse_width_us(micro_sec)
    self.pulse_width_ticks = micro_sec

    __update(self.period_ticks, self.pulse_width_ticks)
  end

  def missing_argument_error(name)
    ArgumentError.new("must pass #{key}")
  end
end