
#include "my-keyboard-layout.hpp"
#include "my-physical-keyboard.hpp"
#include "map-joining.hpp"
#include "remapping-handler.hpp"

#include <iostream>

using std::pair;
using std::cout;
using std::string;

int main() {
  MyKeyboardLayout layout;
  MyPhysicalKeyboard phys;
  
  FullMappingSet full = joinMappings(phys.layout, layout.layout);
  
  const int leftShift = 42;
  const int rightShift = 54;
  const int leftAlt = 56;
  const int rightAlt = 100;
  const int capsLock = 58;
  const int leftWin = 125;
  const int leftControl = 29;
  const int rightControl = 97;

  RemappingHander handler(full,
    { leftShift, rightShift },
    { rightAlt, capsLock, leftWin },
    { leftAlt, leftControl, rightControl }
  );
  
  auto test = [&](int c, DevInputValue v) {
    auto res = handler.handle(DevInputEvent { c, v });
    for (auto d : res) {
      if (d.value == pressed) {
        cout << d.code << " ";
      }
    }
    cout << "| ";
    for (auto d : res) {
      if (d.value == released) {
        cout << d.code << " ";
      }
    }
    cout << "\n";
    cout.flush();
  };
  
  test(30, pressed);
  test(30, released);
  
  test(31, pressed);
  test(31, released);
  
  test(32, pressed);
  test(32, released);
  
  test(33, pressed);
  test(33, released);
  
  test(rightAlt, pressed);
  test(33, pressed);
  test(rightAlt, released);
  test(33, released);
  
  test(rightAlt, pressed);
  test(19, pressed);
  test(rightAlt, released);
  test(19, released);
  
  test(leftShift, pressed);
  test(40, pressed);
  test(leftShift, released);
  test(40, released);
}

