#include "harness.hpp"

int main() {
    for (const auto& [name, fn] : th::registry()) {
        const int before = th::failures();
        fn();
        std::cout << (th::failures() == before ? "PASS " : "FAIL ") << name << '\n';
    }
    std::cout << (th::failures() == 0 ? "all tests passed\n"
                                      : "FAILURES: " + std::to_string(th::failures()) + "\n");
    return th::failures() == 0 ? 0 : 1;
}
