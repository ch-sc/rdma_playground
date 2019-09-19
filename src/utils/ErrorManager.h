//
// Created by chris on 18.09.19.
//

#ifndef RDMA_ERRORMANAGER_H
#define RDMA_ERRORMANAGER_H

#include "Logging.h"
#include <errno.h>

namespace rdma {

    static
    int fail(std::string file_name, int line_number) {
        rdma::Logging::fatal(file_name, line_number, strerror(errno));
        exit(1);
        return 0;
    }

#define FAIL fail(__FILE__, __LINE__)

}

#endif //RDMA_ERRORMANAGER_H
