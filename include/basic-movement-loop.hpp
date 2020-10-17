
#pragma once

#include "devinput-model.hpp"

#include <map>
#include <list>
#include <set>

using std::map;
using std::list;
using std::set;

struct Action {
  list<int> keys;
};

struct BasicMovementLoop {
  BasicMovementLoop();
  
  list<DevInputEvent> handle(DevInputEvent const& in);
  
  private:
    map<int, Action> movementMappings;
    bool movementDown;
    set<int> nativePressedKeys;
    set<int> ordinaryPressedKeys;
    set<int> magicPressedKeys;
};

