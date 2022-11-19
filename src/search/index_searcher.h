#pragma once

namespace mmse {
 
class IndexSearcher {
public:
    IndexSearcher();
    ~IndexSearcher();

    IndexSearcher(IndexSearcher const&) = delete;             // Copy construct
    IndexSearcher(IndexSearcher&&) = delete;                  // Move construct
    IndexSearcher& operator=(IndexSearcher const&) = delete;  // Copy assign
    IndexSearcher& operator=(IndexSearcher &&) = delete;      // Move assign    
private:
};

} // namespace