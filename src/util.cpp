#include "util.h"
#include <string>
#include <vector>

namespace mmse {

int split_string(
        const std::string& str,
        std::vector<std::string>& splited_strs,
        const char separator,
        bool skip_sub_string/* = true*/,
        const std::string& avoid_nesting_mode/* = ""*/, 
        bool ignore_empty/* = false*/) {
    splited_strs.clear();
    // 分割点
    std::vector<size_t> split_point_list;
    // 检查嵌套模式
    if (avoid_nesting_mode.empty() == false && avoid_nesting_mode.length() != 2) {
        return -1;
    }
    // 嵌套深度
    int depth = 0;
    for (size_t idx = 0; idx < str.length(); ++idx) {
        if (skip_sub_string == true) {
            if (str[idx] == '"' && (idx == 0 || str[idx - 1] != '\\')) {
                while (++idx < str.length()) {
                    if (str[idx] == '"') {
                        break;
                    }
                    if (str[idx] == '\\') {
                        ++idx;
                    }
                }
                continue;
            }
        }
        auto& c = str[idx];
        if (avoid_nesting_mode.empty() == false && c == avoid_nesting_mode.back()) {
            if (avoid_nesting_mode.front() == avoid_nesting_mode.back()) {
                depth = 1 - depth;
                continue;
            }
            --depth;
        }
        if (depth == 0 && c == separator) {
            split_point_list.emplace_back(idx);
        }
        if (avoid_nesting_mode.empty() == false && c == avoid_nesting_mode.front()) {
            ++depth;
        }
        if (depth < 0) {
            return -1;
        }
    }
    split_point_list.emplace_back(str.length());
    splited_strs.reserve(split_point_list.size() - 1);
    // split
    size_t begin_idx = 0;
    for (size_t part_idx = 0; part_idx < split_point_list.size(); ++part_idx) {
        if (!ignore_empty || split_point_list[part_idx] - begin_idx > 0) {
            splited_strs.emplace_back(str, begin_idx, split_point_list[part_idx] - begin_idx);
        }
        begin_idx = split_point_list[part_idx] + 1;
    }
    return 0;
}

} // namespace