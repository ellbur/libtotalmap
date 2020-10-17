
// vim: expandtab
// vim: shiftwidth=2

#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "devinput-interaction.hpp"
#include "basic-movement-loop.hpp"

#include "my-keyboard-layout.hpp"
#include "my-physical-keyboard.hpp"
#include "map-joining.hpp"
#include "remapping-handler.hpp"

using std::string;
using std::cerr;
using boost::program_options::options_description;
using boost::program_options::positional_options_description;
using boost::program_options::command_line_parser;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::bool_switch;
using boost::program_options::value;

int main(int argc, char **argv) {
  options_description optsDesc("Options");
  optsDesc.add_options()
    ("help", "Display usage")
    ("trace", bool_switch(), "Print JSON info describing behavior")
    ("kbd-file", value<string>(), "Keyboard device under /dev/input")
    ;

  positional_options_description posOpts;
  posOpts.add("kbd-file", 1);

  auto parsed = command_line_parser(argc, argv)
    .options(optsDesc)
    .positional(posOpts)
    .run();

  variables_map optionsMap;
  store(parsed, optionsMap);

  bool trace = optionsMap["trace"].as<bool>();

  if (optionsMap.count("help") || optionsMap.count("kbd-file") == 0) {
    char const* programName = argc >= 1 ? argv[0] : "example-keyboard-layout-1";
    fprintf(stderr, "Usage: %s [OPTIONS] <kbd-file>\n", programName);
    fprintf(stderr, "\n");
    fprintf(stderr, "<kbd-file> is a keyboard device under /dev/input (e.g., /dev/input/by-path/platform-i8042-serio-0-event-kbd)\n");
    fprintf(stderr, "\n");
    fflush(stderr);
    cerr << optsDesc;
    return 0;
  }

  string keyboardFilePath = optionsMap["kbd-file"].as<string>();

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

  RemappingHander remapping(full,
    { leftShift, rightShift },
    { rightAlt, capsLock, leftWin },
    { leftAlt, leftControl, rightControl }
  );
  BasicMovementLoop movement;

  runDevInputLoop(keyboardFilePath, "send_b", trace, [&](DevInputEvent const& ev) {
    auto r1 = movement.handle(ev);

    list<DevInputEvent> r2;
    for (auto ev2 : r1) {
      auto rr2 = remapping.handle(ev2);
      for (auto ev3 : rr2) {
        r2.push_back(ev3);
      }
    }

    return r2;
  });

  return 0;
}

