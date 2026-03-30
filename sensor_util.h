/**
 * @file    sensor_utils.h
 * @brief   Sensor utility functions — Railway Track Health Monitor
 * @author  Sameeksha R
 *
 * Provides ADC-to-g conversion and RMS computation
 * for the 3-axis accelerometer on TI CC3200.
 */
 
#ifndef SENSOR_UTILS_H
#define SENSOR_UTILS_H
 
#include <math.h>
 
/* ── ADC config (CC3200: 12-bit ADC, 1.8V reference) ───────────────── */
#define ADC_RESOLUTION     4096.0f    /* 12-bit */
#define ADC_VREF           1.8f       /* Volts  */
#define ACCEL_SENSITIVITY  0.300f     /* V/g — adjust per datasheet */
#define ACCEL_ZERO_G_V     0.9f       /* Voltage at 0g (mid-supply) */
 
/**
 * @brief  Convert raw ADC reading to acceleration in g.
 * @param  raw  12-bit ADC value (0–4095)
 * @return Acceleration in g
 */
inline float adc_to_g(int raw)
{
    float voltage = ((float)raw / ADC_RESOLUTION) * ADC_VREF;
    return (voltage - ACCEL_ZERO_G_V) / ACCEL_SENSITIVITY;
}
 
/**
 * @brief  Compute RMS magnitude of 3-axis acceleration vector.
 * @param  ax, ay, az  Acceleration components in g
 * @return RMS value in g
 */
inline float compute_rms(float ax, float ay, float az)
{
    return sqrtf((ax * ax + ay * ay + az * az) / 3.0f);
}
 
/**
 * @brief  Apply simple exponential moving average filter.
 * @param  prev    Previous filtered value
 * @param  current Raw new value
 * @param  alpha   Filter coefficient (0–1, lower = more smoothing)
 * @return Filtered value
 */
inline float ema_filter(float prev, float current, float alpha)
{
    return alpha * current + (1.0f - alpha) * prev;
}
 
#endif /* SENSOR_UTILS_H */
