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
        if (message.type() == MessageType::OK)
            break;
        switch (message.type()) {
        default:
            throw UnexpectedResponse(std::move(message), MessageType::Unknown);
        case MessageType::ResolveExternalSymbols:
            resolve_external_symbols(linker, library, message.cast<msg::ResolveExternalSymbols>());
            break;
        case MessageType::ReserveMemorySpaces:
            reserve_memory_spaces(linker, library, message.cast<msg::ReserveMemorySpaces>());
            break;
        case MessageType::CommitMemory:
            commit_memory(linker, library, message.cast<msg::CommitMemory>());
            break;
        case MessageType::Execute:
            execute(linker, library, message.cast<msg::Execute>());
            break;
        }
    }
}

void Librarian::resolve_external_symbols(Linker &linker, Library &library, msg::ResolveExternalSymbols const &message) {
    msg::ResolvedSymbols msg_resolved;
    msg_resolved.body().reserve(message.body().size());
    for (auto const& [lib, sym] : message.body()) {
        msg_resolved.body().emplace_back(linker.resolve_symbol(lib, sym));
    }
    courier_.send(msg_resolved);
}

void Librarian::reserve_memory_spaces(Linker &linker, Library &library, msg::ReserveMemorySpaces const &message) {
    msg::ReservedMemory msg_reserved;
    msg_reserved.body().reserve(message.body().size());
    for (auto [addr, size] : message.body()) {
        msg_reserved.body().emplace_back(linker.reserve_memory(addr, size));
    }
    courier_.send(msg_reserved);
}

void Librarian::commit_memory(Linker &linker, Library &library, msg::CommitMemory const &message) {
    linker.commit_memory(message.address, message.memory, message.protection);
    msg::OK msg_ok;
    courier_.send(msg_ok);
}

void Librarian::execute(Linker &linker, Library &library, msg::Execute const &message) {
    linker.create_thread(library, message.value);
}

void Librarian::unlink(Linker &linker, Library const &library) {
}
