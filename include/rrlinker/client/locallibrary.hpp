#pragma once

#include <rrlinker/client/library.hpp>

namespace rrl {

    class LocalLibrary : public Library {
    public:
        LocalLibrary(std::string const &name);
        LocalLibrary(LocalLibrary const&) = delete;
        LocalLibrary(LocalLibrary&&) = default;

        using Library::operator=;

        struct LocalSymbol : public Symbol {
            LocalSymbol(Symbol symbol)
                : Symbol(symbol)
            {}
            template<typename T, typename ...Args>
            T stdcall(Args... args) const {
                return (reinterpret_cast<T(__stdcall*)(Args...)>(address))(args...);
            }
            template<typename T, typename ...Args>
            T ccall(Args... args) const {
                return (reinterpret_cast<T(__cdecl*)(Args...)>(address))(args...);
            }
            template<typename T>
            T& value() const {
                return *reinterpret_cast<T*>(address);
            }
        };

        virtual LocalSymbol operator[](std::string const &symbol) const;
    };

}
