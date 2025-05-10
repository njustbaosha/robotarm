#include "Kinematic_solution.h"
static double M_PI = 3.14159265358979323846;

float theta1,theta2;
float R1 = 150;
float R2 = 165;

float rad_to_deg(float rad)
{
    return rad * 180 / M_PI;
}

void acclulate(float x, float y, float z,float *theta1, float *theta2, float *theta3)
{
	  const float A = 150.0f;
    const float B = 165.0f;
	  float C = sqrtf(x * x + y * y+ z * z);
    float D = (A * A - B * B - x * x - y * y- z*z) / (2.0f * B);
    float phi = atan2f(z, sqrtf(x*x+y*y));\
	  // 检查 |D/C| 是否 ≤ 1，确保 acosf 有意义
    if (fabsf(D / C) <= 1.0f) {
        *theta1 = phi + acosf(D / C);
        // 计算 θ2
        float cos_theta2 = (sqrtf(x*x+y*y) + B * cosf(*theta1)) / A;
        float sin_theta2 = (z + B * sinf(*theta1)) / A;
        *theta2 = atan2f(sin_theta2, cos_theta2);
    } else {
        // 处理无解情况，可根据实际需求调整
        *theta1 = 0.0f;
        *theta2 = 0.0f;
    }
	*theta1 = rad_to_deg(*theta1);
    *theta2 = rad_to_deg(*theta2);
    // 计算 θ3
    float angle3 = atan2f(y, x);
    *theta3 = rad_to_deg(angle3);

    // 处理角度范围
    if (*theta1 < 0) {
        *theta1 += 360;
    }
    if (*theta2 < 0) {
        *theta2 += 360;
    }
    if (*theta3 < 0) {
        *theta3 += 360;
    }
//    float R = sqrt(x*x + y*y);
//    float angle3 = rad_to_deg(atan2(y,x));
//    float angle1 = atan2(z, R);
//    float S = x*x+y*y + z * z;
//    float C1 = (-4725.0f - S) / 330.0f;
//    float arccos_val = fabsf(C1) / sqrtf(S);

//    if (arccos_val > 1.0f) {
//        // 处理数值异常（如超出反余弦定义域）
//        return ;
//    }
//    float temp_angle = acosf(arccos_val);
//    float theta1_1 = angle1 + temp_angle;
//    float theta1_2 = angle1 - temp_angle;

//    // 弧度转角度
//    theta1_1 *= (180.0f / M_PI);
//    theta1_2 *= (180.0f / M_PI);

//    // 筛选符合 80°~180° 的 theta1
//    if (theta1_1 >= 80.0f && theta1_1 <= 180.0f) {
//        *theta1 = theta1_1;
//        angle1 = theta1_1;
//    } else if (theta1_2 >= 80.0f && theta1_2 <= 180.0f) {
//        *theta1 = theta1_2;
//        angle1 = theta1_2;
//    } else {
//        // theta1 超出范围，错误处理

//        return ;
//    }
//    float angle2 = rad_to_deg(atan2(((z+165*sinf(angle1*M_PI/180.0f))/150.0f),((R+165*cosf(angle1*M_PI/180.0f))/150.0f)));
//    *theta2 = angle2;
//    *theta3 = angle3;
}



