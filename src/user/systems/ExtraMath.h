#pragma once
#include "script/userScript.h"

namespace P64::ExtraMath {
    float vec2_angle_norm_diff(fm_vec2_t* vec1, fm_vec2_t* vec2) 
    {
        float c = fm_vec2_dot(vec1, vec2);
        c = fmaxf(-1.0f, fminf(1.0f, c));
        return acosf(c);
    }

    fm_vec2_t vec2_rotate_by_angle(fm_vec2_t* vec, float rotateDegrees) {
        float radians = FM_DEG2RAD(rotateDegrees);
        float sin = sinf(radians);
        float cos = cosf(radians);

        return {
            vec->x * cos - vec->y * sin,
            vec->x * sin + vec->y * cos
        };
    }

    float vec2_cross(fm_vec2_t* vec1, fm_vec2_t* vec2) {
        return vec1->x * vec2->y - vec1->y * vec2->x;
    }
}