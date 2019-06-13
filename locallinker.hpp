#pragma once

#include "linker.hpp"

namespace rrl {

    class LocalLinker : public Linker {
    public:
        explicit LocalLinker(symbol_resolver resolver = nullptr);
        LocalLinker(LocalLinker const&) = delete;
        LocalLinker(LocalLinker&&) = default;

        using Linker::operator=;

    protected:
        virtual uintptr_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) override;
        virtual void add_export(Library &library, std::string const &symbol, uintptr_t address) override;
    };

}
