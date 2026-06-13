#pragma once

#include "Request.h"
#include <string>

bool parseNavCommandLine(const std::string& line, RequestWire& out);
