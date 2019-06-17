#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cstdint>

#include <Windows.h>

#include "linkage.hpp"

namespace rrl {

    class Linker;
    class LocalLinker;
    class RemoteLinker;

    class Library {
    public:
        friend class ::rrl::Linker;
        friend class ::rrl::LocalLinker;
        friend class ::rrl::RemoteLinker;

        struct Symbol {
            explicit Symbol(uintptr_t address = 0)
                : address(address)
            {}
            uintptr_t address;
        };

        Library(LinkageKind kind, HANDLE process, std::string const &name);
        Library(Library const &other) = delete;
        Library(Library &&other) = default;
        virtual ~Library();

        Library& operator=(Library const&) = delete;
        Library& operator=(Library&&) = default;

        std::unordered_map<LPVOID, SIZE_T> const& get_memory_spaces() const { return memory_spaces_; }
        std::unordered_map<std::string, Symbol> const& get_symbols() const { return symbols_; }
        std::unordered_map<std::string, HMODULE> const& get_module_dependencies() const { return module_dependencies_; }
        std::unordered_map<std::string, Library&> const& get_library_dependencies() const { return library_dependencies_; }
        std::unordered_map<std::string, Library&> const& get_dependent_libraries() const { return dependent_libraries_; }
        std::unordered_set<HANDLE> const& get_threads() const { return threads_; }

        inline LinkageKind kind() const { return linkage_kind_; }

        uintptr_t get_symbol_address(std::string const &symbol) const;

        void detach();

        std::string const name;
        HANDLE const process;

        static DWORD const UNLINK_THREAD_EXIT_CODE;

    protected:
        void unlink();

        void add_module_dependency(std::string const &module, HMODULE handle);
        void add_library_dependency(Library &library);
        void remove_library_dependency(Library &library);
        void add_dependent_library(Library &library);
        void remove_dependent_library(Library &library);

        void add_thread(HANDLE hThread);
        void add_memory_space(LPVOID address, SIZE_T size);

        void set_symbol_address(std::string, uintptr_t address);

        std::unordered_map<LPVOID, SIZE_T> memory_spaces_;
        std::unordered_map<std::string, Symbol> symbols_;
        std::unordered_map<std::string, HMODULE> module_dependencies_;
        std::unordered_map<std::string, Library&> library_dependencies_;
        std::unordered_map<std::string, Library&> dependent_libraries_;
        std::unordered_set<HANDLE> threads_;

    private:
        LinkageKind linkage_kind_;
        bool detached_;
    };

}
