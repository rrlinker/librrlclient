#include "librarian.h"
#include <librlcom/bound_check.h>
#include <stdexcept>

using namespace rrl;

Librarian::Librarian(Courier &courier)
    : courier_(courier)
{
}

void Librarian::link(Linker &linker, Library &library) {
    if (linker.kind() != library.kind()) {
        throw std::logic_error("linker and library kinds are different!");
    }
    request_library(library);
    perform_linkage(linker, library);
    linker.register_library(library);
}

void Librarian::request_library(Library const &library) {
    msg::LinkLibrary msg_link_lib(library.name);
    courier_.send(msg_link_lib);
    courier_.receive().assert<msg::OK>();
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

void Librarian::resolve_external_symbol(Linker &linker, Library &library, msg::ResolveExternalSymbol const &data) {
    msg::ResolvedSymbol msg_resolved;
    msg_resolved.value = linker.resolve_symbol(library, data.library, data.symbol);
    courier_.send(msg_resolved);
}

void Librarian::accept_exported_symbol(Linker &linker, Library &library, msg::ExportSymbol const &data) {
    verify_pointer_bounds(data.address);
    linker.add_export(library, data.symbol, static_cast<uintptr_t>(data.address));
}

void Librarian::reserve_memory_space(Linker &linker, Library &library, msg::ReserveMemorySpace const &data) {
    msg::ReservedMemory msg_reserved;
    verify_size_bounds(data.value);
    msg_reserved.value = linker.reserve_memory(library, static_cast<size_t>(data.value));
    courier_.send(msg_reserved);
}

void Librarian::commit_memory(Linker &linker, Library &library, msg::CommitMemory const &data) {
    verify_pointer_bounds(data.address);
    linker.commit_memory(library, static_cast<uintptr_t>(data.address), data.memory, data.protection);
    courier_.send(msg::OK{});
}

void Librarian::execute(Linker &linker, Library &library, msg::Execute const &data) {
    verify_pointer_bounds(data.value);
    linker.create_thread(library, static_cast<uintptr_t>(data.value));
    courier_.send(msg::OK{});
}

void Librarian::unlink(Linker &linker, Library &library) {
    if (linker.kind() != library.kind()) {
        throw std::logic_error("linker and library kinds are different!");
    }
    linker.unlink(library);
    linker.unregister_library(library);
}
