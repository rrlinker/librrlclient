#pragma once

#include "library.h"

namespace rrl {

    class RemoteLibrary : public Library {
    public:
        using Library::Library;
        using Library::operator=;
    };

}
