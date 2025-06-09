#include <Arduino.h>
#include <stdio.h>
#include "config.h"
#include "sine_lut.h"
#include "esp_system.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

static uint32_t mf = 111;
static float ma = 1.0f;
static float freq = 100.0f;
static int sample_offset = 1;

static mcpwm_config_t timerConf = {.frequency = mf * freq * 2, .cmpr_a = 50.0, .cmpr_b = 50.0, .duty_mode = MCPWM_DUTY_MODE_0, .counter_mode = MCPWM_UP_DOWN_COUNTER};

static mcpwm_pin_config_t pinConfig = {.mcpwm0a_out_num = FASE_R_HIGH_PIN,
                                       .mcpwm0b_out_num = FASE_R_LOW_PIN,
                                       .mcpwm1a_out_num = FASE_S_HIGH_PIN,
                                       .mcpwm1b_out_num = FASE_S_LOW_PIN,
                                       .mcpwm2a_out_num = FASE_T_HIGH_PIN,
                                       .mcpwm2b_out_num = FASE_T_LOW_PIN};

static mcpwm_sync_config_t sync0_conf = {.sync_sig = MCPWM_SELECT_NO_INPUT, .timer_val = 0, .count_direction = MCPWM_TIMER_DIRECTION_UP};
static mcpwm_sync_config_t sync1_conf = {.sync_sig = MCPWM_SELECT_TIMER0_SYNC, .timer_val = 0, .count_direction = MCPWM_TIMER_DIRECTION_UP};
static mcpwm_sync_config_t sync2_conf = {.sync_sig = MCPWM_SELECT_TIMER0_SYNC, .timer_val = 0, .count_direction = MCPWM_TIMER_DIRECTION_UP};

static void IRAM_ATTR mcpwm_callback(void *);

void init_mcpwm()
{
    mcpwm_set_pin(MCPWM_UNIT_0, &pinConfig);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &timerConf);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &timerConf);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_2, &timerConf);

    mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 5, 5);
    mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 5, 5);
    mcpwm_deadtime_enable(MCPWM_UNIT_0, MCPWM_TIMER_2, MCPWM_ACTIVE_HIGH_COMPLIMENT_MODE, 5, 5);
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_0, &sync0_conf);
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_1, &sync1_conf);
    mcpwm_sync_configure(MCPWM_UNIT_0, MCPWM_TIMER_2, &sync2_conf);
    mcpwm_set_timer_sync_output(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_SWSYNC_SOURCE_TEZ);
    sample_offset = (int)round(((float)LUT_SIZE) / (float)mf);
    mcpwm_isr_register(MCPWM_UNIT_0, mcpwm_callback, NULL, ESP_INTR_FLAG_IRAM, NULL);

    MCPWM0.int_ena.timer0_tez_int_ena = 1;
}

void start_gen()
{
    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_1);
    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_2);
    return;
}
void stop_gen()
{
    mcpwm_stop(MCPWM_UNIT_0, MCPWM_TIMER_0);
    return;
}

void set_ma(float _ma)
{
    ma = _ma;
}

float get_ma()
{
    return ma;
}

void set_mf(int _mf)
{
    mf = _mf;
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, mf * freq * 2);
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_1, mf * freq * 2);
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_2, mf * freq * 2);
    sample_offset = (int)round(((float)LUT_SIZE) / (float)mf);
}

int get_mf()
{
    return mf;
}

float get_freq() { return freq; }

void set_freq(float _freq)
{
    freq = _freq;
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_0, mf * freq * 2);
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_1, mf * freq * 2);
    mcpwm_set_frequency(MCPWM_UNIT_0, MCPWM_TIMER_2, mf * freq * 2);
    sample_offset = (int)round(((float)LUT_SIZE) / (float)mf);
}

static void IRAM_ATTR mcpwm_callback(void *)
{
    static int counterR = 0;
    static int counterS = (LUT_SIZE - 1) / 3;
    static int counterT = (LUT_SIZE - 1) * 2 / 3;

    if (MCPWM0.int_st.timer0_tez_int_st) // Counter reached zero
    {
        MCPWM0.int_clr.timer0_tez_int_clr = 1; // Clear interrupt

        // Precompute sample values
        float lutR = sine_lut[counterR]/50.0f; // Scale to 0-1 range
        float lutS = sine_lut[counterS]/50.0f;
        float lutT = sine_lut[counterT]/50.0f;

        // Compute duty cycles
        float dutyR = 75.0f;
        float dutyS = fmaxf((lutS * ma) + 50.0f, 0.0f);
        float dutyT = fmaxf((lutT * ma) + 50.0f, 0.0f);

        // Update PWM duties
        const mcpwm_timer_t timers[] = {MCPWM_TIMER_0, MCPWM_TIMER_1, MCPWM_TIMER_2};
        const float duties[] = {dutyR, dutyS, dutyT};

        for (int i = 0; i < 3; ++i) {
            mcpwm_set_duty(MCPWM_UNIT_0, timers[i], MCPWM_GEN_A, duties[i]);
            mcpwm_set_duty(MCPWM_UNIT_0, timers[i], MCPWM_GEN_B, duties[i]);
        }

        // Advance counters with wraparound
        counterR = (counterR + sample_offset) % LUT_SIZE;
        counterS = (counterS + sample_offset) % LUT_SIZE;
        counterT = (counterT + sample_offset) % LUT_SIZE;
    }
}