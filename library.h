#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cstdint>

#include <Windows.h>

namespace rrl {

    class Library {
    public:
        struct Symbol {
            Symbol() = default;
            explicit Symbol(uintptr_t address)
                : address(address)
            {}

            uintptr_t address;

            template<typename T, typename ...Args>
            T stdcall(Args... args) const {
                return (reinterpret_cast<T(__stdcall*)(Args...)>(address))(args...);
            }

            template<typename T, typename ...Args>
            T ccall(Args... args) const {
                return (reinterpret_cast<T(__cdecl*)(Args...)>(address))(args...);
            }
        };

        Library(HANDLE process, std::string const &name);
        Library(Library const &other) = delete;
        Library(Library &&other) = default;
        virtual ~Library();

        Library& operator=(Library const &other) = delete;
        Library& operator=(Library &&other) = default;

        void add_module_dependency(std::string const &module, HMODULE handle);
        void add_library_dependency(Library &library);
        void remove_library_dependency(Library &library);
        void add_dependent_library(Library &library);
        void remove_dependent_library(Library &library);

        void add_thread(HANDLE hThread);
        void add_memory_space(LPVOID address, SIZE_T size);

        std::unordered_map<LPVOID, SIZE_T> const& get_memory_spaces() { return memory_spaces_; }
        std::unordered_map<std::string, Symbol> const& get_symbols() { return symbols_; }
        std::unordered_map<std::string, HMODULE> const& get_module_dependencies() { return module_dependencies_; }
        std::unordered_map<std::string, Library&> const& get_library_dependencies() { return library_dependencies_; }
        std::unordered_map<std::string, Library&> const& get_dependent_libraries() { return dependent_libraries_; }
        std::unordered_set<HANDLE> const& get_threads() { return threads_; }

        void set_symbol_address(std::string, uintptr_t address);
        uintptr_t get_symbol_address(std::string const &symbol) const;
        Symbol const& operator[](std::string const &symbol) const;

        virtual void unlink();

        std::string const name;
        HANDLE const process;

        static DWORD const UNLINK_THREAD_EXIT_CODE;

    protected:
        std::unordered_map<LPVOID, SIZE_T> memory_spaces_;
        std::unordered_map<std::string, Symbol> symbols_;
        std::unordered_map<std::string, HMODULE> module_dependencies_;
        std::unordered_map<std::string, Library&> library_dependencies_;
        std::unordered_map<std::string, Library&> dependent_libraries_;
        std::unordered_set<HANDLE> threads_;
    };

}
