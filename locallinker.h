#pragma once

#include "linker.h"

namespace rrl {

    class LocalLinker : public Linker {
    public:
        virtual uintptr_t resolve_symbol(std::string const &library, std::string const &symbol) const override;
        virtual uintptr_t reserve_memory(uintptr_t address, size_t size) const override;

        virtual std::byte* get_buffer(uintptr_t address, size_t size) const override;
        virtual void commit_buffer(std::byte *buffer, uintptr_t address, size_t size, uint32_t protection) const override;
    };

}

