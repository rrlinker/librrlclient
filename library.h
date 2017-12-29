#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace rrl {

    class Library {
    public:
        explicit Library(std::string const &name);
        Library(Library const &other) = delete;
        Library(Library &&other) = default;

        Library& operator=(Library const &other) = delete;
        Library& operator=(Library &&other) = default;

        uintptr_t get_symbol_address(std::string const &symbol);
        uintptr_t operator[](std::string const &symbol);

        std::string const name;
    protected:

        std::unordered_map<std::string, uintptr_t> symbols_;
    };

}
