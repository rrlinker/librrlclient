#include "librarian.h"

using namespace rrl;

Librarian::Librarian(Courier &courier)
    : courier_(courier)
{
}

void Librarian::link(Linker &linker, Library &library) {
    request_library(library);
    perform_linkage(linker, library);
}

void Librarian::request_library(Library const &library) {
    msg::LinkLibrary msg_link_lib;
    strncpy(msg_link_lib.name, library.name.c_str(), sizeof(msg_link_lib.name));
    courier_.send(msg_link_lib);

    auto response = courier_.receive();
    if (response.type() != MessageType::OK)
        throw UnexpectedResponse(std::move(response), MessageType::OK);
}

void Librarian::perform_linkage(Linker &linker, Library &library) {
    msg::Any message;
    while (true) {
        message = courier_.receive();
        switch (message.type()) {
        }
    }

    // resolve external symbol
    std::string symlib, symbol;
    uintptr_t addr = linker.resolve_symbol(symlib, symbol);
    if (!addr) {

    }

    // ...
    uintptr_t address;
    size_t size;
    address = linker.reserve_memory(address, size);

    std::byte *buf = linker.get_buffer(address, size);
    int protection;
    linker.commit_buffer(buf, address, size, protection);
}

void Librarian::unlink(Linker &linker, Library const &library) {
}
