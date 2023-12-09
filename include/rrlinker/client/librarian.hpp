#pragma once

#include <rrlinker/com/courier.hpp>
#include <rrlinker/client/linker.hpp>
#include <rrlinker/client/library.hpp>

namespace rrl {

    class Librarian {
    public:
        Librarian(Courier &courier);

        void link(Linker &linker, Library &library);
        void unlink(Linker &linker, Library &library);

    private:
        void request_library(Library const &library);
        void perform_linkage(Linker &linker, Library &library);

        void resolve_external_symbol(Linker &linker, Library &library, msg::ResolveExternalSymbol const &data);
        void reserve_memory_space(Linker &linker, Library &library, msg::ReserveMemorySpace const &data);
        void accept_exported_symbol(Linker &linker, Library &library, msg::ExportSymbol const &data);
        void commit_memory(Linker &linker, Library &library, msg::CommitMemory const &data);
        void execute(Linker &linker, Library &library, msg::Execute const &data);
    
        Courier &courier_;
    };
}
