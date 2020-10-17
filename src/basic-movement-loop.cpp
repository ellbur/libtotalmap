
#include "basic-movement-loop.hpp"
#include <boost/range/adaptor/reversed.hpp>

using boost::adaptors::reverse;

const int leftControl = 29;
const int capsLock = 58;
const int myTabKey = 15;
const int backslash = 43;

BasicMovementLoop::BasicMovementLoop() :
  movementMappings({
    { 36, { { 105 } } },
    { 37, { { 108 } } },
    { 38, { { 106 } } },
    { 23, { { 103 } } },
    { 35, { { 102 } } },
    { 39, { { 107 } } },
    { 22, { { 104 } } },
    { 50, { { 109 } } },
    { 49, { { leftControl, 105 } } },
    { 51, { { leftControl, 106 } } },
  }),
  movementDown(false),
  nativePressedKeys(),
  ordinaryPressedKeys(),
  magicPressedKeys()
{
}

list<DevInputEvent> BasicMovementLoop::handle(DevInputEvent const& ev) {
  list<DevInputEvent> res = { };  

  auto press = [&](int code) {
    res.push_back({ code, pressed });
  };

  auto release = [&](int code) {
    res.push_back({ code, released });
  };

  auto ordinaryPress = [&](int code) {
    ordinaryPressedKeys.insert(code);
    press(code);
  };

  auto ordinaryRelease = [&](int code) {
    ordinaryPressedKeys.erase(code);
    if (! magicPressedKeys.count(code)) {
      release(code);
    }
  };

  auto magicPress = [&](int code) {
    magicPressedKeys.insert(code);
    press(code);
  };

  auto magicRelease = [&](int code) {
    magicPressedKeys.erase(code);
    if (! ordinaryPressedKeys.count(code)) {
      release(code);
    }
  };

  auto applyAction = [&](int native, Action const& action) {
    ordinaryRelease(native);
    for (int key : action.keys) {
      magicPress(key);
    }
  };

  auto swallowAction = [&](Action const& action) {
    for (int key : reverse(action.keys)) {
      magicRelease(key);
    }
  };

  int workingCode = ev.code;
  int workingValue = ev.value;

  if (workingValue == pressed) {
    nativePressedKeys.insert(workingCode);
  }
  else if (workingValue == released) {
    nativePressedKeys.erase(workingCode);
  }

  if (workingCode==myTabKey) {
    if (workingValue==pressed) {
      for (int key : nativePressedKeys) {
        if (movementMappings.count(key)) {
          applyAction(key, movementMappings[key]);
        }
      }

      release(capsLock);
      movementDown = true;
    }
    else if (workingValue==released) {
      for (int key : nativePressedKeys) {
        if (movementMappings.count(key)) {
          swallowAction(movementMappings[key]);
        }
      }

      movementDown = false;
    }
  }
  else if (movementDown) {
    if (workingValue == pressed) {
      if (movementMappings.count(workingCode)) {
        Action action = movementMappings[workingCode];
        for (int key : action.keys) {
          magicPress(key);
        }
      }
      else {
        ordinaryPress(workingCode);
      }
    }
    else if (workingValue == released) {
      if (movementMappings.count(workingCode)) {
        Action action = movementMappings[workingCode];
        for (int key : reverse(action.keys)) {
          magicRelease(key);
        }
      }
      else {
        ordinaryRelease(workingCode);
      }
    }
  }
  else {
    int codeToSend = workingCode;
    if (workingCode == backslash) {
      codeToSend = myTabKey;
    }

    if (workingValue == pressed) {
      ordinaryPress(codeToSend);
    }
    else if (workingValue == released) {
      ordinaryRelease(codeToSend);
    }
  }

  return res;
}

