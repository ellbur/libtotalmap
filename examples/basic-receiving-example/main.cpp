
// http://stackoverflow.com/questions/27581500/hook-into-linux-key-event-handling/27693340#27693340
// vim: shiftwidth=2

#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <thread>
#include <chrono>

using std::string;
using std::cerr;
using boost::program_options::options_description;
using boost::program_options::positional_options_description;
using boost::program_options::command_line_parser;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::bool_switch;
using boost::program_options::value;

#define die(str, args...) do { \
  perror(str); \
  exit(1); \
} while(0)

int main(int argc, char **argv) {
  options_description optsDesc("Options");
  optsDesc.add_options()
    ("help", "Display usage")
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
  
  int fdo, fdi;
  struct input_event ev;
  
  fdi = open(keyboardFilePath.c_str(), O_RDONLY);
  if (fdi < 0) {
    die((string("Failed to open input device path ") + keyboardFilePath).c_str());
  }
  
  while (true) {
    if (read(fdi, &ev, sizeof(struct input_event)) < 0) {
      die("error: read");
    }
    
    printf("%d %d %d\n", ev.type, ev.code, ev.value);
  }
  
  return 0;
}

