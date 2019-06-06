#include "locallibrary.h"

#include <stdexcept>

using namespace rrl;

LocalLibrary::LocalLibrary(std::string const &name)
    : Library(LinkageKind::Local, GetCurrentProcess(), name)
{}

LocalLibrary::LocalSymbol LocalLibrary::operator[](std::string const &symbol) const {
    if (auto it = symbols_.find(symbol); it != symbols_.end())
        return it->second;
    else
        throw std::out_of_range("symbol `" + symbol + "` not found");
}
