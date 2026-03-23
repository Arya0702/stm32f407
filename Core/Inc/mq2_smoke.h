#ifndef MQ2_SMOKE_H
#define MQ2_SMOKE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * MQ2 烟雾/可燃气体半定量解算（ADC → Rs → 经验 ppm）
 *
 * 电路模型：Vout = Vcc * RL / (Rs + RL)，故 Rs = RL * (Vcc / Vout - 1)。
 * ppm 使用常见对数拟合（近似烟雾/LPG 混合响应），仅作趋势参考；真实浓度需标定。
 *
 * 在洁净空气中测得 adc_clean 后调用 MQ2_CalibrateR0_FromCleanAirAdc(adc_clean)，
 * 内部按 Rs/R0≈9.8（手册典型洁净空气比）反推 R0。
 */

float MQ2_AdcToSmokePpm(uint32_t adc_raw);
void MQ2_CalibrateR0_FromCleanAirAdc(uint32_t adc_in_clean_air);

#ifdef __cplusplus
}
#endif

#endif /* MQ2_SMOKE_H */
