#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace rrl {

    class Library {
    public:
        struct Symbol {
            uintptr_t address;

            template<typename T, typename ...Args, typename Fn = T(Args...)>
            std::invoke_result_t<Fn> operator()(Args... args) const {
                return *reinterpret_cast<Fn*>(args...);
            }
        };

        explicit Library(std::string const &name);
        Library(Library const &other) = delete;
        Library(Library &&other) = default;

        Library& operator=(Library const &other) = delete;
        Library& operator=(Library &&other) = default;

        uintptr_t get_symbol_address(std::string const &symbol);
        Symbol const& operator[](std::string const &symbol);

        std::string const name;
    protected:

        std::unordered_map<std::string, Symbol> symbols_;
    };

}
