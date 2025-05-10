/**
 * @file servo.c
 * @brief 基于STM32 HAL库的舵机控制库
 * @author doubao
 * @date 2025-05-10
 */

#include "servo.h"

// 舵机配置参数表
static ServoConfig_t gServoConfig[SERVO_MAX_CHANNELS];

/**
 * @brief 初始化舵机控制库
 * @return 初始化状态，0表示成功
 */
uint8_t Servo_Init(void)
{
    // 初始化所有舵机配置
    for (uint8_t i = 0; i < SERVO_MAX_CHANNELS; i++) {
        gServoConfig[i].initialized = 0;
    }
    
    return 0;
}

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
                     uint16_t minAngle, uint16_t maxAngle)
{
    if (channel >= SERVO_MAX_CHANNELS) {
        return 1; // 通道号超出范围
    }
    
    // 保存配置参数
    gServoConfig[channel].htim = htim;
    gServoConfig[channel].channel = timChannel;
    gServoConfig[channel].minPulse = minPulse;
    gServoConfig[channel].maxPulse = maxPulse;
    gServoConfig[channel].minAngle = minAngle;
    gServoConfig[channel].maxAngle = maxAngle;
    gServoConfig[channel].initialized = 1;
    
    HAL_TIM_PWM_Start(gServoConfig[channel].htim,gServoConfig[channel].channel);

    return 0;
    
}

/**
 * @brief 获取舵机当前角度
 * @param channel 舵机通道号
 * @param angle 角度存储指针
 * @return 获取状态，0表示成功
 */
uint8_t Servo_GetAngle(uint8_t channel, float* angle)
{
    if (channel >= SERVO_MAX_CHANNELS || !gServoConfig[channel].initialized) {
        return 1; // 通道未初始化
    }
    
    ServoConfig_t* config = &gServoConfig[channel];
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(config->htim);
    uint32_t pulse = 0;
    
    // 获取当前PWM占空比
    switch(config->channel) {
        case TIM_CHANNEL_1:
            pulse = __HAL_TIM_GET_COMPARE(config->htim, TIM_CHANNEL_1);
            break;
        case TIM_CHANNEL_2:
            pulse = __HAL_TIM_GET_COMPARE(config->htim, TIM_CHANNEL_2);
            break;
        case TIM_CHANNEL_3:
            pulse = __HAL_TIM_GET_COMPARE(config->htim, TIM_CHANNEL_3);
            break;
        case TIM_CHANNEL_4:
            pulse = __HAL_TIM_GET_COMPARE(config->htim, TIM_CHANNEL_4);
            break;
        default:
            return 2; // 无效通道
    }
    
    // 计算脉冲宽度(微秒)
    float pulseWidth = (float)pulse * 20000 / period;
    
    // 脉冲宽度到角度的线性映射
    *angle = config->minAngle + 
            ((pulseWidth - config->minPulse) * (config->maxAngle - config->minAngle)) / 
            (config->maxPulse - config->minPulse);
    return 0;
}    

/**
 * @brief 设置舵机角度
 * @param channel 舵机通道号
 * @param angle 目标角度(度)
 * @return 设置状态，0表示成功
 */
uint8_t Servo_SetAngle(uint8_t channel, float angle)
{
    if (channel >= SERVO_MAX_CHANNELS || !gServoConfig[channel].initialized) {
        return 1; // 通道未初始化
    }
    
    ServoConfig_t* config = &gServoConfig[channel];
    
    // 角度限制
    if (angle < config->minAngle) angle = config->minAngle;
    if (angle > config->maxAngle) angle = config->maxAngle;
    
    // 角度到脉冲宽度的线性映射
    float pulseWidth = config->minPulse + 
                      ((angle - config->minAngle) * (config->maxPulse - config->minPulse)) / 
                      (config->maxAngle - config->minAngle);
    
    // 计算PWM占空比值
    uint32_t period = __HAL_TIM_GET_AUTORELOAD(config->htim);
    uint32_t pulse = (uint32_t)((pulseWidth * period) / 20000); // 20000us = 20ms

    
    // 设置PWM占空比
    switch(config->channel) {
        case TIM_CHANNEL_1:
            __HAL_TIM_SET_COMPARE(config->htim, TIM_CHANNEL_1, pulse);
            break;
        case TIM_CHANNEL_2:
            __HAL_TIM_SET_COMPARE(config->htim, TIM_CHANNEL_2, pulse);
            break;
        case TIM_CHANNEL_3:
            __HAL_TIM_SET_COMPARE(config->htim, TIM_CHANNEL_3, pulse);
            break;
        case TIM_CHANNEL_4:
            __HAL_TIM_SET_COMPARE(config->htim, TIM_CHANNEL_4, pulse);
            break;
        default:
            return 2; // 无效通道
    }
    
    return 0;
}

