#pragma once
#include <stdint.h>

typedef struct
{
    uint8_t Height;
    uint8_t CharSpace;
    uint8_t UpperCaseBottomSpc;
    uint16_t Offset[95][2];
    const uint8_t *Data;
}
tFont;
