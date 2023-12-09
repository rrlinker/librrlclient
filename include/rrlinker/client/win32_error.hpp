#pragma once

#include <stdexcept>
#include <cstdint>

namespace rrl::win {

    class Win32Error : public std::runtime_error {
    public:
        Win32Error(uint32_t error);

        static std::string error_to_message(uint32_t error);

        uint32_t const error;
    };

}
