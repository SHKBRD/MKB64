#include "script/userScript.h"
#include "scene/sceneManager.h"

#include "debug.h"
#include <vi/swapChain.h>
#include "scene/components/camera.h"
#include "scene/components/collBody.h"
#include "collision/attach.h"
#include <libdragon.h>

#include "systems/World.h"

namespace {
  const float MAX_TILT_ANGLE = 23.0;
  const float TILT_EASING = 0.2;
  const float CAM_PLAYER_BEHIND = 48.0;
  const float CAM_PLAYER_ABOVE = 16.0;
  const fm_vec3_t CAM_PLAYER_FOCUS_OFFSET = {0.0,8.0,0.0};

  const float GRAVITY = 1 * 16;
  const float FRICTION = 0.97;
}

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
    fm_vec2_t stageTilt;

    fm_vec3_t velocity;

    Object* camera;

    //Coll::Attach meshAttach;

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

  void init_player(Object& obj, Data *data) {
    data->playerNumber = get_player_number(obj, data);
    data->prevStageTilt = fm_vec2_t{0.0, 0.0};
    data->stageTilt = fm_vec2_t{0.0, 0.0};
    data->velocity = fm_vec3_t{0.0, 0.0, 0.0};
  }

  void update_player_movement(Object& obj, Data *data, float deltaTime) {
    fm_vec2_t tilt = User::world.stageTilt[data->playerNumber];
    fm_vec3_t downForce = {
      sinf(tilt.x), 
      GRAVITY * cosf(tilt.x) * cosf(tilt.y), 
      sinf(tilt.y)
    };
    
    // xz velocity += tilt
    data->velocity *= FRICTION;
    data->velocity += downForce * fm_vec3_t{1.0, 0.0, 1.0};
    obj.pos += data->velocity * deltaTime * 60;
  }

  void update_stage_tilt(Object& obj, Data *data, float deltaTime) {
    data->prevStageTilt = User::world.stageTilt[data->playerNumber];
    joypad_inputs_t input = joypad_get_inputs((joypad_port_t)data->playerNumber);
    fm_vec2_t normInpVec{-input.stick_x/128.0f, input.stick_y/128.0f};
    fm_vec2_t tiltDiff = (normInpVec * FM_DEG2RAD(MAX_TILT_ANGLE) - data->prevStageTilt);
    User::world.stageTilt[data->playerNumber] += tiltDiff * TILT_EASING;
  }

  void update_cameras(Object& obj, Data *data, float deltaTime)
  {
    Object* player = obj.getScene().getObjectById(User::world.playerIDs[data->playerNumber]);
    Camera* camera = &obj.getScene().getObjectById(User::world.cameraIDs[data->playerNumber])->getComponent<Comp::Camera>()->camera;
    fm_vec3_t newCamPos;
    fm_vec3_t newCamRot;

    fm_vec3_t playerRot = player->rot * fm_vec3_t{0.0, 0.0, 1.0}; // rot * forward
    
    // lateral movement away from player
    newCamPos = player->pos-(playerRot*CAM_PLAYER_BEHIND*fm_vec3_t{1.0, 0.0, 1.0});
    
    // height
    newCamPos += fm_vec3_t{0.0, CAM_PLAYER_ABOVE, 0.0};
    
    fm_vec3_t camLookAt = {player->pos + CAM_PLAYER_FOCUS_OFFSET};

    camera->setLookAt(newCamPos, camLookAt);
    
  }

  void update_player_body(Coll::BCS bcs, Object& obj, Data *data) {
    //bcs.center -= data->meshAttach.update(bcs.center);
    bcs.velocity = data->velocity;
    //obj.pos += bcs.velocity;
  }

  void init(Object& obj, Data *data)
  {
    // initialization, this is called once when the object spawns
    init_player(obj, data);
  }

  void destroy(Object& obj, Data *data)
  {
    // clean-up, this is called when the object gets deleted
  }

  void update(Object& obj, Data *data, float deltaTime)
  {
    // this is called once every frame, put your main logic here
    auto coll = obj.getComponent<Comp::CollBody>();
    Coll::BCS &bcs = coll->bcs;
    update_stage_tilt(obj, data, deltaTime);

    update_player_movement(obj, data, deltaTime);
    update_player_body(bcs, obj, data);

    update_cameras(obj, data, deltaTime);
  }

  void draw(Object& obj, Data *data, float deltaTime)
  {
    // this is called once every frame, and for every active camera.
    // Put your drawing code here
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
      data->velocity *= -1;
      return;
    }
  }
}
