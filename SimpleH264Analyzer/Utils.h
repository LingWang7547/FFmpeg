#pragma once
#include "Global.h"

// Get bool value from bit position..
int Get_bit_at_position(UINT8 *buf, UINT32 &bytePosition, UINT8 &bitPosition);

// Parse bit stream using unsigned-Expo-Columb coding
int Get_uev_code_num(UINT8 *buf, UINT32 &bytePosition, UINT8 &bitPosition);

// Parse bit stream using signed-Expo-Columb coding
int Get_sev_code_num(UINT8 *buf, UINT32 &bytePosition, UINT8 &bitPosition);

// Parse bit stream as unsigned int bits
int Get_uint_code_num(UINT8 *buf, UINT32 &bytePosition, UINT8 &bitPosition, UINT8 length);

