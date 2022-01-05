#pragma once

#include <functional>

namespace Utils {

class ExecuteOnDestruction
{
public:
    ExecuteOnDestruction() noexcept : destructionCode([](){}) {}
    ExecuteOnDestruction(std::function<void()> code) : destructionCode(std::move(code)) {}
    ~ExecuteOnDestruction() { if (destructionCode) destructionCode(); }

    void reset(std::function<void()> code) { destructionCode = std::move(code); }

private:
    std::function<void()> destructionCode;
};

} // Utils
