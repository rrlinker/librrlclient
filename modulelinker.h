#pragma once

#include "connection.h"
#include "linker.h"
#include "module.h"

namespace rrl {

    class ModuleLinker {
        public:
            ModuleLinker(Connection &connection);

            void link(Linker &linker, Module &module);

        private:
            Connection &m_connection;
    };

}

