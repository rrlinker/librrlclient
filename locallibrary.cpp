#include "locallibrary.h"

using namespace rrl;

LocalLibrary::LocalLibrary(std::string const &name)
    : Library(GetCurrentProcess(), name)
{}
