#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

#include "library.h"

namespace rrl {

    class Linker {
    public:
        virtual uintptr_t resolve_symbol(std::string const &library, std::string const &symbol) const = 0;
        virtual uintptr_t reserve_memory(uintptr_t address, size_t size) const = 0;

        virtual std::byte* get_buffer(uintptr_t address, size_t size) const = 0;
        virtual void commit_buffer(std::byte *buffer, uintptr_t address, size_t size, uint32_t protection) const = 0;

    protected:
        uintptr_t resolve_internal_symbol(std::string const &library, std::string const &symbol) const;

        std::unordered_map<std::string, Library&> libraries_;
    };

}

