#pragma once
#include <libdragon.h>
#include "script/userScript.h"

namespace P64::User {
    struct World {
        uint16_t playerIDs[4]{0,0,0,0};
        uint16_t cameraIDs[4]{0,0,0,0};
        fm_vec2_t stageTilt[4]{};
        uint8_t playerCount{1};
    };
    extern World world;
}