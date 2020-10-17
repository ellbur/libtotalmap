
#include "my-physical-keyboard.hpp"

#include <string>
#include <optional>
#include <map>

using std::string;
using std::optional;
using std::map;

const string phys_1_row = "1234567890-=";
const string phys_q_row = "qwertyuiop[]";
const string phys_a_row = "asdfghjkl;'";
const string phys_z_row = "zxcvbnm,./";

const string phys_1_row_shift = "!@#$%^&*()_+";
const string phys_q_row_shift = "QWERTYUIOP{}";
const string phys_a_row_shift = "ASDFGHJKL:\"";
const string phys_z_row_shift = "ZXCVBNM<>?";

constexpr int key_tilde = 41;
constexpr int key_1 = 2;
constexpr int key_q = 16;
constexpr int key_a = 30;
constexpr int key_z = 44;

MyPhysicalKeyboard::MyPhysicalKeyboard() :
  layout()
{
  layout.tildeCode = key_tilde;
  layout.k1Code = key_1;
  layout.qCode = key_q;
  layout.aCode = key_a;
  layout.zCode = key_z;
  layout.backslashCode = 43;
  layout.leftWinCode = 125;
  
  auto addRow = [&](int codeStart, string const& chars, bool shift) {
    for (size_t i=0; i<chars.length(); i++) {
      int code = codeStart + i;
      char c = chars[i];
      layout.keys[c] = PhysRevKey { code, shift };
    }
  };
  
  addRow(key_1, phys_1_row, false);
  addRow(key_q, phys_q_row, false);
  addRow(key_a, phys_a_row, false);
  addRow(key_z, phys_z_row, false);
  
  addRow(key_1, phys_1_row_shift, true);
  addRow(key_q, phys_q_row_shift, true);
  addRow(key_a, phys_a_row_shift, true);
  addRow(key_z, phys_z_row_shift, true);
  
  layout.keys['`'] = PhysRevKey { key_tilde, false };
  layout.keys['~'] = PhysRevKey { key_tilde, true };
  
  layout.keys['\\'] = PhysRevKey { 43, false };
  layout.keys['|'] = PhysRevKey { 43, true };
}

