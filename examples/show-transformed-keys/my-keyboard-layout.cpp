
#include "my-keyboard-layout.hpp"

#include <string>
using std::string;

const string my_top_row = "17531902468`";
const string my_q_row = ";,.pyfgcrl~@";
const string my_a_row = "aoeuidhtns-";
const string my_z_row = "'qjkxbmwvz";

const string my_top_row_shift = "17531902468`";
const string my_q_row_shift = ":<>PYFGCRL?^";
const string my_a_row_shift = "AOEUIDHTNS@";
const string my_z_row_shift = "\"QJKXBMWVZ";

const string my_top_row_altgr = "";
const string my_q_row_altgr = " {}% \\*][|  ";
const string my_a_row_altgr = "   = &)(/_$";
const string my_z_row_altgr = "     !+#  ";

MyKeyboardLayout::MyKeyboardLayout() :
  layout()
{
  auto addToRow = [&](vector<LayoutKey> &row, string const& chars) {
    for (size_t i=0; i<chars.size(); i++) {
      char c = chars[i];
      if (c == ' ') {
        row.push_back(NullLayoutKey { });
      }
      else {
        row.push_back(CharLayoutKey { c });
      }
    }
  };
  
  addToRow(layout.k1Row, my_top_row);
  addToRow(layout.qRow, my_q_row);
  addToRow(layout.aRow, my_a_row);
  addToRow(layout.zRow, my_z_row);
  
  addToRow(layout.k1RowShift, my_top_row_shift);
  addToRow(layout.qRowShift, my_q_row_shift);
  addToRow(layout.aRowShift, my_a_row_shift);
  addToRow(layout.zRowShift, my_z_row_shift);
  
  addToRow(layout.k1RowAltGr, my_top_row_altgr);
  addToRow(layout.qRowAltGr, my_q_row_altgr);
  addToRow(layout.aRowAltGr, my_a_row_altgr);
  addToRow(layout.zRowAltGr, my_z_row_altgr);
  
  layout.tilde = CodeLayoutKey { 125 };
  layout.tildeShift = CodeLayoutKey { 125 };
  layout.tildeAltGr = CodeLayoutKey { 125 };
  
  layout.leftWin = CodeLayoutKey { 100 };
  
  layout.qRowAltGr[0] = CodeLayoutKey { 1 };
}

