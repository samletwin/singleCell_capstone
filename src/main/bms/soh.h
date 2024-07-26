#ifndef SOH_H
#define SOH_H

#include "custom_types.h"
#include <vector>

typedef struct {
    float32 OCV_f32;
    float32 internalResistance_f32;
} soh_result;

/* API Functions */
float soh_getSOH(float resistance_f32);
soh_result soh_LeastSquares(const std::vector<float32> &current_mA_vf32, const std::vector<float32> &voltage_mV_vf32);
soh_result soh_LeastSquares(const float32 current_mA_af32[], const float32 voltage_mV_af32[], uint16 numSamples_ui16);

#endif 