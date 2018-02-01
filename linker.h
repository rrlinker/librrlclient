#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

#include "library.h"

#include <Windows.h>

namespace rrl {

    class Linker {
    public:
        virtual uint64_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) = 0;
        virtual uint64_t reserve_memory(uint64_t address, size_t size) const = 0;
        virtual void commit_memory(uint64_t buffer, std::vector<std::byte> const &memory, uint32_t protection) const = 0;
        virtual void create_thread(Library &library, uint64_t address) const = 0;

    protected:
        uint64_t resolve_internal_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const;
        virtual HMODULE get_module_handle(Library &library, std::string const &module) = 0;

        std::unordered_map<std::string, Library&> libraries_;
        
        std::unordered_map<std::string, HMODULE> module_handles_;
    };

}

