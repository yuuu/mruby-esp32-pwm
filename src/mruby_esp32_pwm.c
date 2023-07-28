#include <mruby.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#include <mruby/string.h>
#include <mruby/array.h>
#include <mruby/presym.h>

#include <stdio.h>
#include <string.h>

#if ESP_PLATFORM
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#else
#define ESP_LOGI(tag, fmt, ...)  printf(fmt, __VA_ARGS__)
#endif

#define TAG ("mruby-esp32-pwm")

typedef struct pwm_t {
  mrb_int pulse_width_ticks;
  mrb_int pin;
  mrb_int period_ticks;
#if ESP_PLATFORM
  mcpwm_timer_handle_t *timer;
  mcpwm_cmpr_handle_t *comparator;
  mcpwm_oper_handle_t *operator;
  mcpwm_gen_handle_t *generator;
#endif
  mrb_state *mrb;
} pwm_t;

static void mrb_pwm_free(mrb_state *mrb, void *p);

static const struct mrb_data_type mrb_pwm = {
  "mrb_mruby_esp32_pwm", mrb_pwm_free
};

static void
mrb_pwm_new_mcpwm(mrb_state *mrb, pwm_t *pwm) {
#if ESP_PLATFORM
  if((pwm->period_ticks <= 0) || (pwm->pulse_width_ticks <= 0)) {
    return;
  }

  pwm->timer = mrb_malloc(mrb, sizeof(mcpwm_timer_handle_t));
  mcpwm_timer_config_t timer_config = {
    .group_id = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
    .resolution_hz = 1000000,
    .period_ticks = pwm->period_ticks,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
  };
  ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, pwm->timer));

  pwm->operator = mrb_malloc(mrb, sizeof(mcpwm_oper_handle_t));
  mcpwm_operator_config_t operator_config = {
    .group_id = 0,
  };
  ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, pwm->operator));
  ESP_ERROR_CHECK(mcpwm_operator_connect_timer(*pwm->operator, *pwm->timer));

  pwm->comparator = mrb_malloc(mrb, sizeof(mcpwm_cmpr_handle_t));
  ESP_LOGI(TAG, "!(%p)", pwm->comparator);

  mcpwm_comparator_config_t comparator_config = {
    .flags.update_cmp_on_tez = true,
  };
  ESP_ERROR_CHECK(mcpwm_new_comparator(*pwm->operator, &comparator_config, pwm->comparator));

  pwm->generator = mrb_malloc(mrb, sizeof(mcpwm_gen_handle_t));
  mcpwm_generator_config_t generator_config = {
    .gen_gpio_num = pwm->pin,
  };
  ESP_ERROR_CHECK(mcpwm_new_generator(*pwm->operator, &generator_config, pwm->generator));
  ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(*pwm->comparator, pwm->pulse_width_ticks));

  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(*pwm->generator, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(*pwm->generator, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, *pwm->comparator, MCPWM_GEN_ACTION_LOW)));

  ESP_ERROR_CHECK(mcpwm_timer_enable(*pwm->timer));
  ESP_ERROR_CHECK(mcpwm_timer_start_stop(*pwm->timer, MCPWM_TIMER_START_NO_STOP));
#endif
}

static void
mrb_pwm_del_mcpwm(mrb_state *mrb, pwm_t *pwm) {
#if ESP_PLATFORM
  if(pwm->generator != NULL) {
    ESP_ERROR_CHECK(mcpwm_del_generator(*pwm->generator));
    mrb_free(mrb, pwm->generator);
    pwm->generator = NULL;
  }
  if(pwm->comparator != NULL) {
    ESP_ERROR_CHECK(mcpwm_del_comparator(*pwm->comparator));
    mrb_free(mrb, pwm->comparator);
    pwm->comparator = NULL;
  }
  if(pwm->operator != NULL) {
    ESP_ERROR_CHECK(mcpwm_del_operator(*pwm->operator));
    mrb_free(mrb, pwm->operator);
    pwm->operator = NULL;
  }
  if(pwm->timer != NULL) {
    ESP_ERROR_CHECK(mcpwm_timer_start_stop(*pwm->timer, MCPWM_TIMER_STOP_EMPTY));
    ESP_ERROR_CHECK(mcpwm_timer_disable(*pwm->timer));
    ESP_ERROR_CHECK(mcpwm_del_timer(*pwm->timer));
    mrb_free(mrb, pwm->timer);
    pwm->timer = NULL;
  }
#endif
}

static void
mrb_pwm_free(mrb_state *mrb, void *p) {
  pwm_t *pwm = (pwm_t *)p;

#if ESP_PLATFORM
  mrb_pwm_del_mcpwm(mrb, pwm);
#endif

  mrb_free(mrb, p);
}

static mrb_value
mrb_pwm_init(mrb_state *mrb, mrb_value self) {
  pwm_t *pwm = mrb_malloc(mrb, sizeof(pwm_t));
  mrb_get_args(mrb, "i", &pwm->pin);
  ESP_LOGI(TAG, "__initialize(pin: %d)", pwm->pin);

  pwm->period_ticks = 0;
  pwm->pulse_width_ticks = 0;

#if ESP_PLATFORM
  pwm->timer = NULL;
  pwm->comparator = NULL;
  pwm->operator = NULL;
  pwm->generator = NULL;

  mrb_pwm_new_mcpwm(mrb, pwm);
#endif

  mrb_data_init(self, pwm, &mrb_pwm);

  return self;
}

static mrb_value
mrb_pwm_update(mrb_state *mrb, mrb_value self) {
  pwm_t *pwm = (pwm_t *) DATA_PTR(self);
  mrb_get_args(mrb, "ii", &pwm->period_ticks, &pwm->pulse_width_ticks);
  ESP_LOGI(TAG, "__update(%d, %d)", pwm->period_ticks, pwm->pulse_width_ticks);

#if ESP_PLATFORM
  mrb_pwm_del_mcpwm(mrb, pwm);
  mrb_pwm_new_mcpwm(mrb, pwm);
#endif

  return self;
}

void
mrb_mruby_esp32_pwm_gem_init(mrb_state* mrb) {
  struct RClass *client_class = mrb_define_class(mrb, "PWM", mrb->object_class);

  mrb_define_method(mrb, client_class, "__initialize", mrb_pwm_init, MRB_ARGS_REQ(1));
  mrb_define_method(mrb, client_class, "__update", mrb_pwm_update, MRB_ARGS_REQ(2));
}

void
mrb_mruby_esp32_pwm_gem_final(mrb_state* mrb) {
}
