#pragma once

namespace mmse {
 
class {{Class}} {
public:
    {{Class}}();
    ~{{Class}}();

    {{Class}}({{Class}} const&) = delete;             // Copy construct
    {{Class}}({{Class}}&&) = delete;                  // Move construct
    {{Class}}& operator=({{Class}} const&) = delete;  // Copy assign
    {{Class}}& operator=({{Class}} &&) = delete;      // Move assign    
private:
};

} // namespace