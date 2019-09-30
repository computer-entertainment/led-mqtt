#pragma once

#pragma pack(push,1)
struct ledState {
    uint8_t animationId;
    uint8_t brightness;
    uint16_t animationSize;
    uint16_t speed;
    uint32_t color;
    float colorRotation;
    uint8_t decay;
    uint8_t fadeId;
    uint16_t fadeSpeed;
    uint8_t colorRotationEffect;
};

#pragma pack(pop)
