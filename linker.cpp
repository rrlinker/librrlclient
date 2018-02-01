#include "linker.h"

using namespace rrl;

uint64_t Linker::resolve_internal_symbol(Library &library, std::string const &symbol_library, std::string const &symbol_name) const {
    if (auto it = libraries_.find(symbol_library); it != libraries_.end()) {
        auto const &export_library = it->second;
        library.add_library_dependency(export_library);
        return export_library.get_symbol_address(symbol_name);
    }
    return 0;
}
