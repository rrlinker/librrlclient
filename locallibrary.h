#pragma once

#include "library.h"

namespace rrl {

    class LocalLibrary : public Library {
    public:
        LocalLibrary(std::string const &name);
        LocalLibrary(LocalLibrary const &other) = delete;
        LocalLibrary(LocalLibrary &&other) = default;

        using Library::operator=;
    };

}
