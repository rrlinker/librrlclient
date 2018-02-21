#include "librarian.h"
#include <bound_check.h>

using namespace rrl;

Librarian::Librarian(Courier &courier)
    : courier_(courier)
{
}

void Librarian::link(Linker &linker, Library &library) {
    request_library(library);
    perform_linkage(linker, library);
    linker.register_library(library);
}

void Librarian::request_library(Library const &library) {
    msg::LinkLibrary msg_link_lib;
    msg_link_lib.body() = library.name;
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
        case MessageType::ResolveExternalSymbol:
            resolve_external_symbol(linker, library, message.cast<msg::ResolveExternalSymbol>());
            break;
        case MessageType::ExportSymbol:
            accept_exported_symbol(linker, library, message.cast<msg::ExportSymbol>());
            break;
        case MessageType::ReserveMemorySpace:
            reserve_memory_space(linker, library, message.cast<msg::ReserveMemorySpace>());
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

void Librarian::resolve_external_symbol(Linker &linker, Library &library, msg::ResolveExternalSymbol const &message) {
    msg::ResolvedSymbol msg_resolved;
    msg_resolved.body().value = linker.resolve_symbol(library, message.body().library, message.body().symbol);
    courier_.send(msg_resolved);
}

void Librarian::accept_exported_symbol(Linker &linker, Library &library, msg::ExportSymbol const &message) {
    verify_pointer_bounds(message.body().address);
    linker.add_export(library, message.body().symbol, static_cast<uintptr_t>(message.body().address));
}

void Librarian::reserve_memory_space(Linker &linker, Library &library, msg::ReserveMemorySpace const &message) {
    msg::ReservedMemory msg_reserved;
    verify_pointer_bounds(message.body().first);
    verify_size_bounds(message.body().second);
    msg_reserved.body().value = linker.reserve_memory(library, static_cast<uintptr_t>(message.body().first), static_cast<size_t>(message.body().second));
    courier_.send(msg_reserved);
}

void Librarian::commit_memory(Linker &linker, Library &library, msg::CommitMemory const &message) {
    verify_pointer_bounds(message.address);
    linker.commit_memory(library, static_cast<uintptr_t>(message.address), message.memory, message.protection);
    msg::OK msg_ok;
    courier_.send(msg_ok);
}

void Librarian::execute(Linker &linker, Library &library, msg::Execute const &message) {
    verify_pointer_bounds(message.value);
    linker.create_thread(library, static_cast<uintptr_t>(message.value));
}

void Librarian::unlink(Linker &linker, Library &library) {
    linker.unlink(library);
    linker.unregister_library(library);
}
