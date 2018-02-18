#pragma once

#include "library.h"
#include "win32exception.h"

namespace rrl {

    class RemoteLibrary : public Library {
    public:
        using Library::Library;
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
                explicit RemoteValue(HANDLE process, uintptr_t address)
                    : process(process)
                    , address(address)
                {}
                HANDLE process;
                uintptr_t address;
                operator T() const {
                    DWORD nbRead;
                    T value;
                    if (!ReadProcessMemory(process, reinterpret_cast<LPCVOID>(address), &value, sizeof(value), &nbRead)) {
                        throw win::Win32Exception(GetLastError());
                    }
                    if (nbRead != sizeof(value)) {
                        throw std::runtime_error("not all bytes of RemoteValue have been read!");
                    }
                    return std::move(value);
                }
                T&& operator=(T &&value) const {
                    DWORD nbWrite;
                    if (!WriteProcessMemory(process, reinterpret_cast<LPCVOID>(address), &value, sizeof(value), &nbWrite)) {
                        throw win::Win32Exception(GetLastError());
                    }
                    if (nbWrite != sizeof(value)) {
                        throw std::runtime_error("not all bytes of RemoteValue have been written!");
                    }
                    return std::forward<T>(value);
                }
            };

            template<typename T>
            RemoteValue<T> value() const {
                return RemoteValue<T>(process, address);
            }
        };

        RemoteSymbol operator[](std::string const &symbol) const;
    };
}
