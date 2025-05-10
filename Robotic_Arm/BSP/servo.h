/**
 * @file servo.h
 * @brief 基于STM32 HAL库的舵机控制库头文件
 * @author doubao
 * @date 2025-05-10
 */

#ifndef __SERVO_H__
#define __SERVO_H__

#include "stm32f4xx_hal.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * 0号舵机为左舵机
 * 1号舵机为右舵机
 * 2号舵机为YAW舵机
 * 3号舵机为吸盘电机
 */
/** 舵机库配置参数 */
#define SERVO_MAX_CHANNELS 4    // 最大支持的舵机通道数
#define SERVO_STEP 10
	
/** 舵机配置结构体 */
typedef struct {
    TIM_HandleTypeDef* htim;    // 定时器句柄
    uint32_t channel;          // 定时器通道
    uint16_t minPulse;         // 最小脉冲宽度(微秒)
    uint16_t maxPulse;         // 最大脉冲宽度(微秒)
    uint16_t minAngle;         // 最小角度(度)
    uint16_t maxAngle;         // 最大角度(度)
    uint16_t initialized;          // 初始化状态
} ServoConfig_t;

/**
 * @brief 初始化舵机控制库
 * @return 初始化状态，0表示成功
 */
uint8_t Servo_Init(void);

/**
 * @brief 配置舵机控制通道
 * @param channel 舵机通道号
 * @param tim 定时器句柄指针
 * @param channel 定时器通道
 * @param minPulse 最小脉冲宽度(微秒)
 * @param maxPulse 最大脉冲宽度(微秒)
 * @param minAngle 最小角度(度)
 * @param maxAngle 最大角度(度)
 * @return 配置状态，0表示成功
 */
uint8_t Servo_Config(uint8_t channel, TIM_HandleTypeDef* htim, uint32_t timChannel, 
                     uint16_t minPulse, uint16_t maxPulse, 
                     uint16_t minAngle, uint16_t maxAngle);

/**
 * @brief 设置舵机角度
 * @param channel 舵机通道号
 * @param angle 目标角度(度)
 * @return 设置状态，0表示成功
 */
uint8_t Servo_SetAngle(uint8_t channel, float angle);

/**
 * @brief 获取舵机当前角度
 * @param channel 舵机通道号
 * @param angle 角度存储指针
 * @return 获取状态，0表示成功
 */
uint8_t Servo_GetAngle(uint8_t channel, float* angle);

#ifdef __cplusplus
}
#endif

#endif /* __SERVO_H__ */    