#include "script/globalScript.h"
#include "script/userScript.h"
#include <libdragon.h>
#include <vi/swapChain.h>

#include "scene/sceneManager.h"
#include "World.h"

namespace P64::User {
    constinit World world{};
}

namespace P64::GlobalScript::C4F4D286D6CBAAAA
{
  void onGameInit()
  {
    User::world = {};
  }

  void onScenePreLoad()
  {
    
  }

  void onScenePostLoad()
  {
    
  }

  void onScenePostUnload()
  {
    
  }

  void onSceneUpdate()
  {
    
  }

  void onScenePreDraw()
  {
    
  }

  void onScenePostDraw3D()
  {
    
  }

  void onSceneDraw2D()
  {
    
  }
}
