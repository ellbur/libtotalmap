
// vim: shiftwidth=2

#include "remapping-handler.hpp"

#include <algorithm>

using std::pair;

RemappingHander::RemappingHander(FullMappingSet mappings, set<int> shiftKeys, set<int> altGrKeys, set<int> otherModifierKeys) :
  mappings(mappings),
  shiftKeys(shiftKeys),
  altGrKeys(altGrKeys),
	otherModifierKeys(otherModifierKeys),
  inPressedKeys(),
  outPressedKeys(),
  outPressedModifiers()
{
}
  
list<DevInputEvent> RemappingHander::handle(DevInputEvent const& ev) {
  list<DevInputEvent> res = { };

  bool shiftDown = false;
  bool altGrDown = false;
  for (auto p : inPressedKeys) {
    if (shiftKeys.count(p.first)) {
      shiftDown = true;
    }

    if (altGrKeys.count(p.first)) {
      altGrDown = true;
    }
  }

  const int outShiftCode = 42;

  if (ev.value == pressed) {
    optional<PhysRevKey> rev;

    if (shiftKeys.count(ev.code) || altGrKeys.count(ev.code)) {
      rev = { };
    }
    else {
      TypedKey tk { ev.code, shiftDown, altGrDown };
      auto found = mappings.mappings.find(tk);
      if (found == mappings.mappings.end()) {
        rev = { PhysRevKey { ev.code, shiftDown } };
      }
      else {
        rev = found->second;
      }
    }

    inPressedKeys[ev.code] = rev;
  }
  else if (ev.value == released) {
    inPressedKeys.erase(ev.code);
  }

  set<int> newOutPressedKeys;
  set<int> newOutPressedModifiers;
  bool needsShift = false;
  bool needsNoShift = false;

  for (pair<int, optional<PhysRevKey>> p : inPressedKeys) {
    optional<PhysRevKey> rev = p.second;
    if (rev) {
      int k = rev->keyCode;
      if (shiftKeys.count(k) || altGrKeys.count(k) || otherModifierKeys.count(k)) {
        newOutPressedModifiers.insert(k);
      }
      else {
        newOutPressedKeys.insert(k);
        needsShift = needsShift || rev->needsShift;
        needsNoShift = needsNoShift || !rev->needsShift;
      }
    }
  }

  shiftDown = false;
  for (auto p : inPressedKeys) {
    if (shiftKeys.count(p.first)) {
      shiftDown = true;
    }
  }

  if (newOutPressedModifiers.size() > 0 && shiftDown) {
    // If the user is pressing a modifier, Shift might be important to the shortcut
    // they are trying to type, so always preserve it
    newOutPressedModifiers.insert(outShiftCode);
  }
  else {
    // Otherwise, if we are typing a regular key, determine shift in such a way as to produce the desired symbol (e.g., Shift is needed to type % but not 5)
    if (needsShift && needsNoShift) {
      // We have a conflict. Type nothing!
      newOutPressedKeys.clear();
    }
    else {
      if (needsShift) {
        newOutPressedModifiers.insert(outShiftCode);
      }
      else if (shiftDown && !needsNoShift) {
        // Default to preserving shift. This allows the user to, e.g.,
        // type a single Shift for software that recognizes it.
        newOutPressedModifiers.insert(outShiftCode);
      }
    }
  }

  auto diff = [&res](set<int> const& a, set<int> const& b) {
    set<int> added;
    for (int c : b) {
      if (!a.count(c)) {
        added.insert(c);
      }
    }

    set<int> removed;
    for (int c : a) {
      if (!b.count(c)) {
        removed.insert(c);
      }
    }

    for (int c : removed) {
      res.push_back(DevInputEvent { c, released });
    }

    for (int c : added) {
      res.push_back(DevInputEvent { c, pressed });
    }
  };

  diff(outPressedModifiers, newOutPressedModifiers);
  outPressedModifiers = newOutPressedModifiers;

  diff(outPressedKeys, newOutPressedKeys);
  outPressedKeys = newOutPressedKeys;

  return res;
}

