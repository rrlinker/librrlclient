#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <functional>
#include <cstdint>

#include "library.h"

#include <Windows.h>

namespace rrl {

    class Linker {
    public:
        using symbol_resolver = std::function<uint64_t(std::string const &library, std::string const &symbol)>;

        Linker() = default;
        explicit Linker(symbol_resolver resolver);
        virtual ~Linker();

        virtual void set_unresolved_symbol_resolver(symbol_resolver resolver);
        virtual void remove_unresolved_symbol_resolver();

        virtual uint64_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) = 0;
        virtual void add_export(Library &library, std::string const &symbol, uint64_t address) = 0;
        virtual uint64_t reserve_memory(Library &library, uint64_t address, size_t size) const;
        virtual void commit_memory(Library &library, uint64_t buffer, std::vector<std::byte> const &memory, uint32_t protection) const;
        virtual void create_thread(Library &library, uint64_t address) const;

    protected:
        virtual uint64_t resolve_internal_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const;
        virtual uint64_t resolve_unresolved_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const;

        virtual HMODULE get_module_handle(Library &library, std::string const &module);

        std::unordered_map<std::string, Library&> libraries_;
        std::unordered_map<std::string, HMODULE> module_handles_;

        symbol_resolver symbol_resolver_;
    };

}

