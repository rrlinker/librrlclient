#pragma once

#include "linker.h"

namespace rrl {

    class LocalLinker : public Linker {
    public:
        virtual uint64_t resolve_symbol(std::string const &library, std::string const &symbol) const override;
        virtual uint64_t reserve_memory(uint64_t address, size_t size) const override;
        virtual void commit_memory(uint64_t address, std::vector<std::byte> const &memory, uint32_t protection) const override;
        virtual void create_thread(Library &library, uint64_t address) const override;
    };

}

