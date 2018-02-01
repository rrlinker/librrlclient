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

        void resolve_external_symbol(Linker &linker, Library &library, msg::ResolveExternalSymbol const &message);
        void reserve_memory_space(Linker &linker, Library &library, msg::ReserveMemorySpace const &message);
        void accept_exported_symbol(Linker &linker, Library &library, msg::ExportSymbol const &message);
        void commit_memory(Linker &linker, Library &library, msg::CommitMemory const &message);
        void execute(Linker &linker, Library &library, msg::Execute const &message);
    
        Courier &courier_;
    };
}
