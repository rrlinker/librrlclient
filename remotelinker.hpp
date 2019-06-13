#pragma once

#include "linker.hpp"

#include <Windows.h>

namespace rrl {

    class RemoteLinker : public Linker {
    public:
        explicit RemoteLinker(symbol_resolver resolver = nullptr);
        RemoteLinker(RemoteLinker const&) = delete;
        RemoteLinker(RemoteLinker&&) = default;

        using Linker::operator=;

        virtual uintptr_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) override;
        virtual void add_export(Library &library, std::string const &symbol, uintptr_t address) override;

        virtual void unlink(Library &library) override;

    protected:
        virtual HMODULE get_remote_module_handle(Library &library, std::string const &module);
        virtual void remote_load_module(HANDLE hProcess, std::string const &module);
        virtual void remote_free_module(HANDLE hProcess, HMODULE hModule);
        virtual HMODULE find_remote_module_handle(HANDLE hProcess, std::string const &module);

        std::unordered_map<std::string, HMODULE> remote_module_handles_;

    private:
        static DWORD const REMOTE_LOAD_LIBRARY_TIMEOUT;
        static DWORD const REMOTE_FREE_LIBRARY_TIMEOUT;
    };

}
