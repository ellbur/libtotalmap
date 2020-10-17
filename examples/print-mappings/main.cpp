
#include "my-keyboard-layout.hpp"
#include "my-physical-keyboard.hpp"
#include "map-joining.hpp"

#include <iostream>

using std::pair;
using std::cout;
using std::string;

string b2s(bool b) {
  if (b) {
    return "X";
  }
  else {
    return " ";
  }
}

int main() {
  MyKeyboardLayout layout;
  MyPhysicalKeyboard phys;
  
  FullMappingSet full = joinMappings(phys.layout, layout.layout);
  
  for (pair<TypedKey, optional<PhysRevKey>> m : full.mappings) {
    TypedKey f = m.first;
    optional<PhysRevKey> t = m.second;
    
    if (t) {
      cout << f.code << "[" << f.shift << "][" << f.altGR << "]" " -> " << t->keyCode << "[" << b2s(t->needsShift) << "]" << "\n";
    }
    else {
      cout << f.code << "[" << f.shift << "][" << f.altGR << "]" " -> ()\n";
    }
  }
}

