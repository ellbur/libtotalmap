
#pragma once

#include <string>
#include "devinput-model.hpp"

using std::string;

void runDevInputLoop(string const& inDevPath, string const& outDevName, bool printDiagnostics, DevInputHandler handler);

