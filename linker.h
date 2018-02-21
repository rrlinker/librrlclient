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
        using symbol_resolver = std::function<uintptr_t(std::string const &library, std::string const &symbol)>;

        Linker() = default;
        explicit Linker(symbol_resolver resolver);
        virtual ~Linker();

        virtual void set_unresolved_symbol_resolver(symbol_resolver resolver);
        virtual void remove_unresolved_symbol_resolver();

        virtual uintptr_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) = 0;
        virtual void add_export(Library &library, std::string const &symbol, uintptr_t address) = 0;
        virtual uintptr_t reserve_memory(Library &library, uintptr_t address, size_t size) const;
        virtual void commit_memory(Library &library, uintptr_t buffer, std::vector<std::byte> const &memory, uint32_t protection) const;
        virtual void create_thread(Library &library, uintptr_t address) const;

        virtual void register_library(Library &library);
        virtual void unlink(Library &library);
        virtual void unregister_library(Library &library);

    protected:
        virtual uintptr_t resolve_internal_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const;
        virtual uintptr_t resolve_unresolved_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const;

        virtual void dependency_bind(Library &dependant, std::string const &dependency);

        virtual HMODULE get_module_handle(Library &library, std::string const &module);

        std::unordered_map<std::string, Library&> libraries_;
        std::unordered_map<std::string, HMODULE> module_handles_;

        symbol_resolver symbol_resolver_;
    };

}
