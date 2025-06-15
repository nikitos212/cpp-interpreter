#pragma once
#include <vector>
#include <string>

inline thread_local std::vector<std::string> call_stack;

struct CallStackGuard {
    CallStackGuard(std::string fn) { call_stack.push_back(std::move(fn)); }
    ~CallStackGuard() { call_stack.pop_back(); }
};