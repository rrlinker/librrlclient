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
    
        Courier &courier_;
    };
}
