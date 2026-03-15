#include "script/userScript.h"
#include "scene/sceneManager.h"

#include "debug.h"
#include <vi/swapChain.h>
#include "scene/components/camera.h"
#include "scene/components/collBody.h"
#include "collision/attach.h"


#include "systems/World.h"
#include "systems/PlayerConst.h"
#include "systems/DeltaLerp.h"
#include "systems/ExtraMath.h"
#include <libdragon.h>

namespace P64::Script::CC8B68CB9A118F18
{
  P64_DATA(
    // Put your arguments and runtime values bound to an object here.
    // If you need them to show up in the editor, add a [[P64::Name("...")]] attribute.
    //
    // Types that can be set in the editor:
    // - uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t
    // - float
    // - AssetRef<sprite_t>
    // - ObjectRef
    //
    // Other types can be used but are not exposed in the editor.

    uint8_t playerNumber;

    fm_vec2_t prevStageTilt;
    //fm_vec2_t stageTilt;

    fm_vec2_t inp1;
    fm_vec2_t inp2;

    //fm_vec3_t velocity;

    Camera* camera;
    Coll::BCS *bcs;
    fm_vec3_t oldCamAimTarget;
    fm_vec3_t camAimTarget;

    fm_vec3_t ballRotSpeed;

  );

  // The following functions are called by the engine at different points in the object's lifecycle.
  // If you don't need a specific function you can remove it.

  uint8_t get_player_number(Object& obj, Data *data) {
    for (uint8_t i=0; i<4; ++i) {
      if (User::world.playerIDs[i] == obj.id) {
        return i;
      }
    }
    return 4;
    //assertf(false, "no players!");
  }

  void update_player_movement(Object& obj, Data *data, float deltaTime) {
    fm_vec2_t tilt = User::world.stageTilt[data->playerNumber];
    fm_vec3_t downForce = {
      sinf(tilt.x), 
      -cosf(tilt.x) * cosf(tilt.y), 
      sinf(tilt.y)
    };

    downForce *= Player::GRAVITY;
    
    // xz velocity += tilt
    data->bcs->velocity *= Player::FRICTION;
    data->bcs->velocity += downForce;
    //obj.pos += data->velocity * deltaTime * 60;
  }

  fm_vec2_t adjust_input_to_cam(fm_vec2_t input, Camera* cam)
{
    fm_vec3_t camForward = cam->getViewDir();

    // keep movement on XZ plane
    camForward.y = 0.0f;

    if (fm_vec3_len(&camForward) > 0.001f)
        fm_vec3_norm(&camForward, &camForward);

    fm_vec3_t camRight = {
        camForward.z,
        0.0f,
        -camForward.x
    };

    fm_vec3_t moveDir = camForward * input.y + camRight * input.x;

    return {moveDir.x, moveDir.z};
}

  void update_stage_tilt(Object& obj, Data *data, float deltaTime) {
    data->prevStageTilt = User::world.stageTilt[data->playerNumber];
    joypad_inputs_t input = joypad_get_inputs((joypad_port_t)data->playerNumber);
    fm_vec2_t normInpVec{-input.stick_x/128.0f, input.stick_y/128.0f};

    fm_vec2_t camAdjustedInput;
    if (fm_vec2_len2(&normInpVec) != 0) {
      camAdjustedInput = adjust_input_to_cam(normInpVec, data->camera);
    } else {
      camAdjustedInput = normInpVec;
    }
    
    data->inp1 = normInpVec;
    data->inp2 = camAdjustedInput;
    

    fm_vec2_t tiltDiff = (camAdjustedInput * FM_DEG2RAD(Player::MAX_TILT_ANGLE) - data->prevStageTilt);
    User::world.stageTilt[data->playerNumber] += tiltDiff * Player::STAGE_TILT_EASING;
  }

  void smooth_camera_turning(Object& obj, Data *data, float deltaTime) {
    fm_vec2_t oldTarget2D = {data->oldCamAimTarget.x, data->oldCamAimTarget.z};
    fm_vec2_t newTarget2D = {data->camAimTarget.x, data->camAimTarget.z};

    float angleDiff = ExtraMath::vec2_angle_norm_diff(&oldTarget2D, &newTarget2D);

    float maxStep = FM_DEG2RAD(Player::PLAYER_CAMERA_TURN_SPEED_DEG_PER_SEC) * deltaTime;

    float step = angleDiff;
    if (angleDiff > maxStep)
        step = maxStep;

    // determine rotation direction. libdragon also only has 3d cross products, so here's another custom one :P
    float cross = ExtraMath::vec2_cross(&oldTarget2D, &newTarget2D);
    if (cross < 0.0f)
        step = -step;

    fm_vec2_t smoothTurnedTarget =
        ExtraMath::vec2_rotate_by_angle(&oldTarget2D, step);

    fm_vec3_t lerpedLookAngle{smoothTurnedTarget.x, 0.0f, smoothTurnedTarget.y};

    data->camAimTarget = lerpedLookAngle;
  }

  void update_cameras(Object& obj, Data *data, float deltaTime)
  {
    Object* player = obj.getScene().getObjectById(User::world.playerIDs[data->playerNumber]);
    fm_vec3_t newCamPos;
    
    data->oldCamAimTarget = data->camAimTarget;

    data->camAimTarget = data->bcs->velocity * fm_vec3_t{1.0, 0.0, 1.0};
    if (fm_vec3_len2(&data->camAimTarget) < 0.01) {
      data->camAimTarget = obj.pos - data->camera->getPos();
    }
    fm_vec3_norm(&data->camAimTarget, &data->camAimTarget);

    smooth_camera_turning(obj, data, deltaTime);

    fm_vec3_t playerNextPos = player->pos + data->bcs->velocity * deltaTime;

    // lateral movement away from player
    newCamPos = playerNextPos-(data->camAimTarget*Player::CAM_PLAYER_BEHIND*fm_vec3_t{1.0, 0.0, 1.0});
    
    // height
    newCamPos += fm_vec3_t{0.0, Player::CAM_PLAYER_ABOVE, 0.0};
    
    fm_vec3_t camLookAt = {playerNextPos + Player::CAM_PLAYER_FOCUS_OFFSET};

    data->camera->setLookAt(newCamPos, camLookAt);
    
  }

  void init_player(Object& obj, Data *data) {
    data->playerNumber = get_player_number(obj, data);
    data->prevStageTilt = fm_vec2_t{0.0, 0.0};
    data->bcs->velocity = fm_vec3_t{0.01, 0.0, 0.0};
    data->oldCamAimTarget = {1.0, 0.0, 0.0};
    data->camAimTarget = {1.0, 0.0, 0.0};
    data->ballRotSpeed = {0.0, 0.0, 0.0};
    data->camera = &obj.getScene().getObjectById(User::world.cameraIDs[data->playerNumber])->getComponent<Comp::Camera>()->camera;
    update_cameras(obj, data, 1/60.0f);
  }

  void update_player_body(Coll::BCS *bcs, Object& obj, Data *data) {
    //bcs.center -= data->meshAttach.update(bcs.center);
    //bcs->velocity = data->velocity;
    //obj.pos += bcs.velocity;
  }

  void update_ball_rolling(Object& obj, Data *data, float deltaTime) {
    bool onFloor = data->bcs->hitTriTypes & Coll::TriType::FLOOR;
    if (onFloor) {
      data->ballRotSpeed.x = -data->bcs->velocity.x;
      data->ballRotSpeed.z = data->bcs->velocity.z;
    }
    fm_vec3_t zAxis{0.0, 0.0, 1.0};
    fm_vec3_t xAxis{1.0, 0.0, 0.0};
    fm_quat_rotate(&obj.rot, &obj.rot, &zAxis, data->ballRotSpeed.x/500.0);
    fm_quat_rotate(&obj.rot, &obj.rot, &xAxis, data->ballRotSpeed.z/500.0);
  }

  void init(Object& obj, Data *data)
  {
    // initialization, this is called once when the object spawns
    auto coll = obj.getComponent<Comp::CollBody>();
    data->bcs = &coll->bcs;
    init_player(obj, data);
  }

  void destroy(Object& obj, Data *data)
  {
    // clean-up, this is called when the object gets deleted
  }

  void update(Object& obj, Data *data, float deltaTime)
  {
    // this is called once every frame, put your main logic here
    
    update_stage_tilt(obj, data, deltaTime);
    update_cameras(obj, data, deltaTime);
    update_player_movement(obj, data, deltaTime);
    update_ball_rolling(obj, data, deltaTime);
    //update_player_body(data->bcs, obj, data);

    
  }

  void draw(Object& obj, Data *data, float deltaTime)
  {
    // this is called once every frame, and for every active camera.
    // Put your drawing code here
    fm_vec3_t camPos = data->camera->getPos();
    rdpq_text_printf(nullptr, 1, 10, 200, "P: %.4f", camPos.x);
    rdpq_text_printf(nullptr, 1, 10, 210, "P: %.4f", camPos.y);
    rdpq_text_printf(nullptr, 1, 10, 220, "P: %.4f", camPos.z);
    rdpq_text_printf(nullptr, 1, 110, 200, "P: %.4f", obj.pos.x);
    rdpq_text_printf(nullptr, 1, 110, 210, "P: %.4f", obj.pos.y);
    rdpq_text_printf(nullptr, 1, 110, 220, "P: %.4f", obj.pos.z);

    rdpq_text_printf(nullptr, 1, 10, 160, "P: %.4f", data->inp1.x);
    rdpq_text_printf(nullptr, 1, 10, 170, "P: %.4f", data->inp1.y);
    rdpq_text_printf(nullptr, 1, 10, 180, "P: %.4f", data->inp2.x);
    rdpq_text_printf(nullptr, 1, 10, 190, "P: %.4f", data->inp2.y);
    
  }

  void onEvent(Object& obj, Data *data, const ObjectEvent &event)
  {
    // generic events an object can receive
    switch(event.type)
    {
      case EVENT_TYPE_ENABLE: // object got enabled
      break;
      case EVENT_TYPE_DISABLE: // object got disabled
      break;

      // you can check for your own custom types here too
    }
  }

  void onCollision(Object& obj, Data *data, const Coll::CollEvent& event)
  {
    // collision callbacks, only used if any collider is attached
    if(event.otherMesh)
    {
      //data->meshAttach.setReference(event.otherMesh);
      //data->velocity *= -1;
      return;
    }
  }
}
