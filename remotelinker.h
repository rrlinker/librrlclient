#pragma once

#include "linker.h"

#include <Windows.h>

namespace rrl {

    class RemoteLinker : public Linker {
    public:
        virtual uint64_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) override;
        virtual void add_export(Library &library, std::string const &symbol, uint64_t address) override;

    protected:
        virtual HMODULE get_module_handle(Library &library, std::string const &module) override;
        virtual void remote_load_module(HANDLE hProcess, std::string const &module);
        virtual HMODULE get_remote_module_handle(HANDLE hProcess, std::string const &module);

        std::unordered_map<std::string, HMODULE> remote_module_handles_;

    private:
        static DWORD const REMOTE_LOAD_LIBRARY_TIMEOUT;
    };

}

