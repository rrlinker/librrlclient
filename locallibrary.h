#pragma once

#include "library.h"

namespace rrl {

    class LocalLibrary : public Library {
    public:
        using Library::Library;
        using Library::operator=;
    };

}

