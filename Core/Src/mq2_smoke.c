#include "mq2_smoke.h"

#include <math.h>

/* STM32 ADC：12 位右对齐，参考电压约 3.3V */
#ifndef MQ2_ADC_MAX
#define MQ2_ADC_MAX 4095.0f
#endif
#ifndef MQ2_ADC_VREF_V
#define MQ2_ADC_VREF_V 3.3f
#endif

/* MQ2 模块典型：负载电阻 5kΩ，加热/分压供电 5V（以模块丝印为准，常见为 5k） */
#ifndef MQ2_RL_OHM
#define MQ2_RL_OHM 5000.0f
#endif
#ifndef MQ2_VCC_V
#define MQ2_VCC_V 5.0f
#endif

/*
 * 若 AO 经电阻分压再进 MCU（例如 5V→3.3V），V_ao = V_adc * MQ2_AO_VOLTAGE_SCALE
 * 无分压则为 1.0f
 */
#ifndef MQ2_AO_VOLTAGE_SCALE
#define MQ2_AO_VOLTAGE_SCALE 1.0f
#endif

/* 洁净空气下 Rs/R0 典型值（手册曲线），用于由一次洁净采样求 R0 */
#ifndef MQ2_CLEAN_AIR_RS_DIV_R0
#define MQ2_CLEAN_AIR_RS_DIV_R0 9.8f
#endif

/*
 * 经验拟合：ppm ≈ A * (Rs/R0)^B（B<0，气体浓度升高时 Rs 下降、比值减小、ppm 升高）
 * 系数来自常见 MQ2 开源拟合（半定量）。
 */
#ifndef MQ2_CURVE_A
#define MQ2_CURVE_A 613.9f
#endif
#ifndef MQ2_CURVE_B
#define MQ2_CURVE_B (-2.074f)
#endif

static float g_r0_ohm = 10000.0f;

void MQ2_CalibrateR0_FromCleanAirAdc(uint32_t adc_in_clean_air)
{
  if (adc_in_clean_air == 0U) {
    return;
  }
  float v_adc = ((float)adc_in_clean_air / MQ2_ADC_MAX) * MQ2_ADC_VREF_V;
  float v_ao = v_adc * MQ2_AO_VOLTAGE_SCALE;
  if (v_ao < 0.01f || v_ao >= (MQ2_VCC_V * 0.99f)) {
    return;
  }
  float rs = MQ2_RL_OHM * ((MQ2_VCC_V / v_ao) - 1.0f);
  if (rs < 1.0f) {
    return;
  }
  g_r0_ohm = rs / MQ2_CLEAN_AIR_RS_DIV_R0;
  if (g_r0_ohm < 100.0f) {
    g_r0_ohm = 100.0f;
  }
}

float MQ2_AdcToSmokePpm(uint32_t adc_raw)
{
  if (adc_raw == 0U) {
    return 0.0f;
  }
  float v_adc = ((float)adc_raw / MQ2_ADC_MAX) * MQ2_ADC_VREF_V;
  float v_ao = v_adc * MQ2_AO_VOLTAGE_SCALE;

  if (v_ao < 0.01f) {
    return 0.0f;
  }
  if (v_ao >= (MQ2_VCC_V * 0.995f)) {
    v_ao = MQ2_VCC_V * 0.995f;
  }

  float rs = MQ2_RL_OHM * ((MQ2_VCC_V / v_ao) - 1.0f);
  if (rs < 1.0f || g_r0_ohm < 1.0f) {
    return 0.0f;
  }

  float ratio = rs / g_r0_ohm;
  if (ratio < 0.01f) {
    ratio = 0.01f;
  }
  if (ratio > 100.0f) {
    ratio = 100.0f;
  }

  float ppm = MQ2_CURVE_A * powf(ratio, MQ2_CURVE_B);
  if (ppm < 0.0f) {
    ppm = 0.0f;
  }
  if (ppm > 20000.0f) {
    ppm = 20000.0f;
  }
  return ppm;
}
