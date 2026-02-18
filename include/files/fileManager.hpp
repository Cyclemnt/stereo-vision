#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "files/paths.hpp"

/*
Static functions to read/write config/data files
*/

namespace Files {
    static bool test();
}

#endif