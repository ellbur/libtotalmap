
#pragma once

#include <list>

using std::list;
#include <functional>

enum DevInputValue {
  pressed = 1,
  released = 0,
  repeated = 2,
};

struct DevInputEvent {
  int code;
  DevInputValue value;
};

typedef std::function<list<DevInputEvent>(DevInputEvent const&)> DevInputHandler;

