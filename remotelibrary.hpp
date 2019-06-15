#pragma once

#include "library.hpp"
#include "win32exception.hpp"

#include <vector>

namespace rrl {

    class RemoteLibrary : public Library {
    public:
        RemoteLibrary(HANDLE process, std::string const &name);
        RemoteLibrary(RemoteLibrary const&) = delete;
        RemoteLibrary(RemoteLibrary&&) = default;

        using Library::operator=;

        struct RemoteSymbol : public Symbol {
            RemoteSymbol(Symbol symbol, HANDLE process)
                : Symbol(symbol)
                , process(process)
            {}

            HANDLE process;

            DWORD stdcall(LPVOID arg, DWORD timeout = INFINITE) const;

            template<typename T>
            struct RemoteValue {
            public:
                RemoteValue(HANDLE process, uintptr_t address)
                    : process(process)
                    , address(address)
                {}
                operator T() const {
                    DWORD nbRead;
                    T value;
                    if (!ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), reinterpret_cast<LPVOID>(&value), sizeof(value), &nbRead)) {
                        throw win::Win32Exception(GetLastError());
                    }
                    if (nbRead != sizeof(value)) {
                        throw std::runtime_error("not all bytes of RemoteValue have been read!");
                    }
                    return std::move(value);
                }
                T&& operator=(T &&value) {
                    DWORD nbWrite;
                    if (!WriteProcessMemory(process, reinterpret_cast<LPVOID>(address), reinterpret_cast<LPCVOID>(&value), sizeof(value), &nbWrite)) {
                        throw win::Win32Exception(GetLastError());
                    }
                    if (nbWrite != sizeof(value)) {
                        throw std::runtime_error("not all bytes of RemoteValue have been written!");
                    }
                    return std::forward<T>(value);
                }
            private:
                HANDLE process;
                uintptr_t address;
            };

            template<typename T>
            struct RemoteArray {
            public:
                RemoteArray(HANDLE process, uintptr_t address)
                    : process(process)
                    , address(address)
                {}
                std::vector<T> read(size_t size) const {
                    DWORD nbRead;
                    std::vector<T> array(size);
                    if (!ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), reinterpret_cast<LPVOID>(array.data()), array.size() * sizeof(T), &nbRead)) {
                        throw win::Win32Exception(GetLastError());
                    }
                    if (nbRead != array.size() * sizeof(T)) {
                        throw std::runtime_error("not all bytes of RemoteArray have been read!");
                    }
                    return array;
                }
                void write(T const *data, size_t size) {
                    DWORD nbWrite;
                    if (!WriteProcessMemory(process, reinterpret_cast<LPVOID>(address), reinterpret_cast<LPCVOID>(data), size * sizeof(T), &nbWrite)) {
                        throw win::Win32Exception(GetLastError());
                    }
                    if (nbWrite != size * sizeof(T)) {
                        throw std::runtime_error("not all bytes of RemoteArray have been written!");
                    }
                }
            private:
                HANDLE process;
                uintptr_t address;
            };

            template<typename T>
            RemoteValue<T> value() const {
                return RemoteValue<T>(process, address);
            }

            template<typename T>
            RemoteArray<T> array() const {
                return RemoteArray<T>(process, address);
            }
        };

        RemoteSymbol operator[](std::string const &symbol) const;
    };
}
