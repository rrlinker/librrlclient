#include "linker.h"

using namespace rrl;

uint64_t Linker::resolve_internal_symbol(std::string const &library, std::string const &symbol) const {
    if (auto it = libraries_.find(library); it != libraries_.end())
        return it->second.get_symbol_address(symbol);
    return 0;
}
