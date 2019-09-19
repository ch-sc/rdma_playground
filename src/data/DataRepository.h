/*
Copyright 2017 Sebastian Breß, German Research Center for Artificial
Intelligence (DFKI GmbH), Technical University of Berlin

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#ifndef DATAREPOSITORY_H
#define DATAREPOSITORY_H

#include <fstream>
#include <iostream>
#include <vector>
#include "DataStore.h"
#include "../utils/ErrorManager.h"
#include <fcntl.h>

#include <immintrin.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <cstdint>
#include <stdio.h>

namespace rdma {

/*
    static
    int read_int(char *buffer, int idx) {
        int i;
        memcpy(&i, &buffer[idx], 4);
        return i;
    }

    struct mapped_byte_buffer {
        char *buffer;
        uint32_t size;
    };

    static
    mapped_byte_buffer map_byte_buffer(char *path, int expected_type) {
        int cache_file = open(path, O_RDONLY);

        struct stat st{};
        stat(path, &st);
//        off_t size = st.st_size;
        size_t size = st.st_size;

        char *buffer = new char[size];
        mmap(buffer, size, PROT_READ,  MAP_PRIVATE | MAP_POPULATE, cache_file, 0L);

        close(cache_file);

        int type = read_int(buffer, (int) size - 4);

        if (type != expected_type) {
            printf("Unexpected type %x\n", type);
            FAIL;
        }

        struct mapped_byte_buffer ret = {buffer, (uint32_t) size - 4};
        return ret;
    }
*/

    template<typename T, typename Allocator = std::allocator<T>>
    bool readColumnFromFile(const std::string &binary_file_path,
                            std::vector<T, Allocator> &v) {
        std::fstream cache_file;
        cache_file.open(binary_file_path.c_str(), std::ios::binary | std::ios::in);
        if (!cache_file.is_open()) {
            std::cout << "Error: could not open file '" << binary_file_path << "'"
                      << std::endl;
            return false;
        }

        // get size of file
        // NOTE: changed 'cache_file.end' to 'std::fstream'
        cache_file.seekg(0, std::fstream::end);
        size_t size = cache_file.tellg();
        cache_file.seekg(0);
        v.resize(size / sizeof(T));
        // read content of cache_file and fill vector of records with data
        cache_file.read(reinterpret_cast<char *>(v.data()), size);
        cache_file.close();
        return true;
    }

    template<typename T, typename Allocator = std::allocator<T>>
    bool writeColumnToFile(const std::string &binary_file_path, const std::vector<T, Allocator> &v) {
        std::fstream cache_file;
        cache_file.open(binary_file_path.c_str(), std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

        if (!cache_file.is_open()) {
            std::cout << "Error: could not open file '" << binary_file_path << "'"
                      << std::endl;
            return false;
        }
        cache_file.write(reinterpret_cast<const char *>(v.data()),
                         v.size() * sizeof(T));
        cache_file.close();
        return true;
    }

}
#endif /* DATAREPOSITORY_H */