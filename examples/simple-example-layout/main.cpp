
// vim: expandtab
// vim: shiftwidth=2

#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "devinput-interaction.hpp"
#include "map-joining.hpp"
#include "remapping-handler.hpp"
#include "standard-physical-keyboards.hpp"

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

  // A simple layout that just swaps A and S when not shifted
  KeyboardLayout layout;
  layout.aRow = {
      CharLayoutKey { 's' },
      CharLayoutKey { 'a' },
  };
  
  FullMappingSet full = joinMappings(
    ANSIWithWin().layout, // Physical layout
    layout // Logical layout
  );

  const int leftShift = 42;
  const int rightShift = 54;
  const int leftAlt = 56;
  const int rightAlt = 100;
  const int leftWin = 125;
  const int leftControl = 29;
  const int rightControl = 97;

  RemappingHander remapping(full,
    { leftShift, rightShift }, // Shit keys
    { }, // AltGr keys (not using any)
    { leftAlt, rightAlt, leftControl, rightControl, leftWin } // Other modifier keys
  );

  runDevInputLoop(keyboardFilePath, "simple_example_layout", trace, [&](DevInputEvent const& ev) {
    return remapping.handle(ev);
  });

  return 0;
}

