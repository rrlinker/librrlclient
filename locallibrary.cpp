#include "locallibrary.h"

using namespace rrl;

LocalLibrary::LocalLibrary(std::string const &name)
    : Library(GetCurrentProcess(), name)
{}

LocalLibrary::LocalSymbol const &LocalLibrary::operator[](std::string const & symbol) const {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return it->second;
    else
        throw std::logic_error("symbol not found");
}
