
#include <iostream>
#include "map-joining.hpp"

using std::cout;
using std::get_if;
using std::holds_alternative;

FullMappingSet joinMappings(PhysicalLayout const& phys, KeyboardLayout const& layout) {
  FullMappingSet res = { { } };
  
  auto map = [&](TypedKey const& tk, LayoutKey const& lk) {
    if (holds_alternative<NullLayoutKey>(lk)) {
      res.mappings[tk] = { };
    }
    else if (auto c = get_if<CharLayoutKey>(&lk)) {
      auto found = phys.keys.find(c->c);
      if (found != phys.keys.end()) {
        res.mappings[tk] = found->second;
      }
      else {
        cout << "No physical mapping for char " << c->c << "(" << (int)c->c << ")" << "\n"; cout.flush();
        res.mappings[tk] = { };
      }
    }
    else if (auto c = get_if<CodeLayoutKey>(&lk)) {
      res.mappings[tk] = PhysRevKey { c->code, false };
    }
  };
  
  auto addRow = [&](int codeStart, vector<LayoutKey> const& row, bool hasShift, bool hasAltGR) {
    for (size_t i=0; i<row.size(); i++) {
      map({ (int)(codeStart + i), hasShift, hasAltGR }, row[i]);
    }
  };

  map({ phys.tildeCode, false, false }, layout.tilde);
  map({ phys.tildeCode, true, false }, layout.tildeShift);
  map({ phys.tildeCode, false, true }, layout.tildeAltGr);
  
  map({ phys.leftWinCode, false, false }, layout.leftWin);
  map({ phys.leftWinCode, true, false }, layout.leftWin);
  map({ phys.leftWinCode, false, true }, layout.leftWin);
  
  addRow(phys.k1Code, layout.k1Row, false, false);
  addRow(phys.qCode, layout.qRow, false, false);
  addRow(phys.aCode, layout.aRow, false, false);
  addRow(phys.zCode, layout.zRow, false, false);
  
  addRow(phys.k1Code, layout.k1RowShift, true, false);
  addRow(phys.qCode, layout.qRowShift, true, false);
  addRow(phys.aCode, layout.aRowShift, true, false);
  addRow(phys.zCode, layout.zRowShift, true, false);
  
  addRow(phys.k1Code, layout.k1RowAltGr, false, true);
  addRow(phys.qCode, layout.qRowAltGr, false, true);
  addRow(phys.aCode, layout.aRowAltGr, false, true);
  addRow(phys.zCode, layout.zRowAltGr, false, true);
  
  return res;
}

