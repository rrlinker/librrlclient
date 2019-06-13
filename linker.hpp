#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <functional>
#include <cstdint>

#include <Windows.h>

#include "library.hpp"
#include "linkage.hpp"

namespace rrl {

    class Librarian;

    class Linker {
    public:
        friend class ::rrl::Librarian;

        using symbol_resolver = std::function<uintptr_t(std::string const &library, std::string const &symbol)>;

        Linker(LinkageKind kind, symbol_resolver resolver = nullptr);
        virtual ~Linker();

        Linker(Linker const&) = delete;
        Linker(Linker&&) = default;

        Linker& operator=(Linker const&) = delete;
        Linker& operator=(Linker&&) = default;

        void set_unresolved_symbol_resolver(symbol_resolver resolver);
        void remove_unresolved_symbol_resolver();

        inline LinkageKind kind() const { return linkage_kind_; }

    protected:
        virtual uintptr_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) = 0;
        virtual void add_export(Library &library, std::string const &symbol, uintptr_t address) = 0;
        virtual uintptr_t reserve_memory(Library &library, size_t size) const;
        virtual void commit_memory(Library &library, uintptr_t buffer, std::vector<std::byte> const &memory, uint32_t protection) const;
        virtual void create_thread(Library &library, uintptr_t address) const;

        void register_library(Library &library);
        void unregister_library(Library &library);

        virtual void unlink(Library &library);

        uintptr_t resolve_internal_symbol(std::string const &symbol_library, std::string const &symbol_name) const;
        uintptr_t resolve_unresolved_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const;

        void dependency_bind(Library &dependant, std::string const &dependency);

        HMODULE get_module_handle(Library &library, std::string const &module);

        std::unordered_map<std::string, Library&> libraries_;
        std::unordered_map<std::string, HMODULE> module_handles_;

        symbol_resolver symbol_resolver_;

    private:
        LinkageKind linkage_kind_;
    };

}
