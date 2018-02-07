#pragma once

#include "linker.h"

namespace rrl {

    class LocalLinker : public Linker {
    public:
        using Linker::Linker;

        virtual uint64_t resolve_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) override;
        virtual void add_export(Library &library, std::string const &symbol, uint64_t address) override;
    };

}
