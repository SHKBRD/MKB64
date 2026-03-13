#include "script/userScript.h"
#include "scene/sceneManager.h"
#include "../p64/assetTable.h"

namespace P64::Script::C2A3F0709DB1C5E7
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

    

    uint8_t playerCount;

    [[P64::Name("Player Container")]]
    ObjectRef playerContainer;
    uint16_t playerIDs[4]{0, 0, 0, 0};

    [[P64::Name("Camera Container")]]
    ObjectRef cameraContainer;
    uint16_t cameraIDs[4]{0, 0, 0, 0};

  );

  // The following functions are called by the engine at different points in the object's lifecycle.
  // If you don't need a specific function you can remove it.

  void add_players(uint8_t playerCount, Object& obj, Data *data) 
  {
    data->playerCount = playerCount;
    for (uint8_t i=0; i<playerCount; ++i) {
      Scene& sc = obj.getScene();
      //uint16_t newPlayerId = sc.addObject("ball/ball.pf"_asset, obj.pos);
    }
  }

  void init(Object& obj, Data *data)
  {
    // initialization, this is called once when the object spawns
    add_players(1, obj, data);
    
  }

  void destroy(Object& obj, Data *data)
  {
    // clean-up, this is called when the object gets deleted
  }

  void update(Object& obj, Data *data, float deltaTime)
  {
    // this is called once every frame, put your main logic here
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
  }
}
