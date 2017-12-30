#pragma once

#include <courier.h>
#include "linker.h"
#include "library.h"

namespace rrl {

    class Librarian {
    public:
        Librarian(Courier &courier);

        void link(Linker &linker, Library &library);
        void unlink(Linker &linker, Library const &library);

    private:
        void request_library(Library const &library);
        void perform_linkage(Linker &linker, Library &library);

        void resolve_external_symbols(Linker &linker, Library &library, msg::ResolveExternalSymbols const &message);
        void reserve_memory_spaces(Linker &linker, Library &library, msg::ReserveMemorySpaces const &message);
        void commit_memory(Linker &linker, Library &library, msg::CommitMemory const &message);
        void execute(Linker &linker, Library &library, msg::Execute const &message);
    
        Courier &courier_;
    };
}
