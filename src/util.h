#pragma once

#include <string>
#include <vector>

namespace unise {
 
int split_string(
        const std::string& str,
        std::vector<std::string>& splited_strs,
        const char separator,
        bool skip_sub_string = true,
        const std::string& avoid_nesting_mode = "", 
        bool ignore_empty = false);

} // namespace