#pragma once
#include "script/userScript.h"

namespace P64::DeltaLerp {
    float delta_lerp(float from, float to, float factor, float delta, float time=1.0f) {
        return fm_lerp(from, to, 1 - powf(factor, delta/time));
    }

    float delta_lerp_angle(float from, float to, float factor, float delta, float time=1.0f) {
        return fm_lerp_angle(from, to, 1 - powf(factor, delta/time));
    }
}