#pragma once
#include "script/userScript.h"

namespace P64::Player {
    const float MAX_TILT_ANGLE = 23.0;
    const float STAGE_TILT_EASING = 0.2;
    const float PLAYER_CAMERA_TURN_SPEED_DEG_PER_SEC = 6000.0;
    const float CAM_PLAYER_BEHIND = 48.0;
    const float CAM_PLAYER_ABOVE = 16.0;
    const fm_vec3_t CAM_PLAYER_FOCUS_OFFSET = {0.0,8.0,0.0};

    const float GRAVITY = 0.5 * 16;
    const float FRICTION = 0.99;
}