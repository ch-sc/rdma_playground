#pragma once

#ifndef DATAREPOSITORY_H
#define DATAREPOSITORY_H

#include <fstream>
#include <vector>
#include "DataStore.h"
#include <fcntl.h>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
//#include <boost/archive/basic_archive.hpp>
#include <immintrin.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <climits>

#include <cstdint>
#include <dirent.h>
#include <regex>
//#include <valarray>

namespace rdma {

    static uint32_t read_uint(const char *buffer, uint32_t idx) {
        uint32_t i;
        i = buffer[idx] | buffer[idx + 1] | buffer[idx + 2] | buffer[idx + 3];
//        memcpy(&i, &buffer[idx], 4);
        return i;
    }

    template<typename V>
    static void
    copy_slice(V *destination, const char *buffer, uint32_t start_index = 0, uint32_t end_index = UINT_MAX) {
        if (end_index == UINT_MAX) {
            size_t size = sizeof(V);
            end_index = start_index + size;
        }

        memcpy(&destination, &buffer[start_index], end_index);
    }

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


 * If serializing a more complex struct or class use: @code friend class boost::serialization::access
 *
 * // EXAMPLE:
 *
 * struct customType {
 *     string string1;
 *     string string2;
 *     int i;
 *     list <string> list;
 *
 *     // boost serialize
 * private:
 *     friend class boost::serialization::access;
 *
 *     template<typename Archive>
 *     void serialize(Archive &ar, const unsigned int version) {
 *         ar & string1;
 *         ar & string2;
 *         ar & i;
 *         ar & list;
 *     }
 * }
 *
    template<typename ClassTo>
    static bool serializeAny(const string &file_path, const ClassTo &c) {
        ofstream out_stream(file_path.c_str(), ios::binary);
        if (out_stream.fail()) {
            Logging::warn(__FILE__, __LINE__, "could not write to file: " + file_path + " ERROR: " + strerror(errno));
            return false;
        }
        boost::archive::binary_oarchive oa(out_stream, boost::archive::archive_flags::no_tracking);
        oa << c;
        return true;
    }

    template<typename ClassTo>
    static bool deserializeAny(const string &file_path, const ClassTo &c) {
        ifstream in_stream(file_path.c_str(), ios::binary & ios::in);
        if (in_stream.fail()) {
            Logging::warn(__FILE__, __LINE__, "could not read from file: " + file_path + " ERROR: " + strerror(errno));
            return false;
        }
        boost::archive::binary_iarchive in_archive(in_stream, boost::archive::archive_flags::no_tracking);
        in_archive >> c;
        return true;
    }

    */

    template<typename T, typename Allocator = std::allocator<T>>
    static bool readColumnFromFile(const std::string &file_path, std::vector<T, Allocator> &vec) {
        std::fstream cache_file;
        cache_file.open(file_path.c_str(), std::ios::binary | std::ios::in);
        if (!cache_file.is_open()) {
            Logging::error(__FILE__, __LINE__, "could not open file '" + file_path + '\'');
            return false;
        }

        cache_file.seekg(0, std::fstream::end);
        size_t size = cache_file.tellg();
        cache_file.seekg(0);
        vec.resize(size / sizeof(T));
        // read content of cache_file and fill vector of records with data
        cache_file.read(reinterpret_cast<char *>(vec.data()), size);
        cache_file.close();
        return true;
    }

    template<typename T, typename Allocator = std::allocator<T>>
    static bool writeColumnToFile(const std::string &file_path, const std::vector<T, Allocator> &vec) {
        std::fstream cache_file;
        cache_file.open(file_path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);

        if (!cache_file.is_open()) {
            Logging::error(__FILE__, __LINE__, "could not open file '" + file_path + '\'');
            return false;
        }
        cache_file.write(reinterpret_cast<const char *>(vec.data()), vec.size() * sizeof(T));
        cache_file.close();
        return true;
    }

    static vector<unsigned char> intToBytes(int number) {
        vector<unsigned char> arrayOfByte(4);
        for (int i = 0; i < 4; i++)
            arrayOfByte[3 - i] = (number >> (i * 8));
        return arrayOfByte;
    }

    template<typename V>
    static bool writeDictToFile(const string &file_path, const map<string, V> &dict) {
        std::stringstream sting_builder;
//        auto stringBuffer = new vector<char>(/*dict.size() * 15*/);
        auto values = new vector<V>(/*dict.size()*/);
        const char sanitizer_c = '_';

        if (dict.empty()) {
            Logging::error(__FILE__, __LINE__, "Cannot serialize empty dictionary");
            return false;
        }

        for (auto &entry : dict) {
            string key = entry.first;
            V value = entry.second;
            // sanitize key - replace ',' with '_'
            int pos;
            while ((pos = key.find(',')) >= 0) {
                Logging::warn("column [" + key + "] contains forbidden character ','");
                key = key.replace(pos, 1, &sanitizer_c);
            }
            sting_builder << key << (',');
            values->push_back(value);
        }

        std::fstream cache_file;
        cache_file.open(file_path.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        if (!cache_file.is_open()) {
            Logging::error(__FILE__, __LINE__, "could not open file '" + file_path + '\'');
            return false;
        }

        string keys = sting_builder.str();
        uint32_t buffer_size = keys.size() - 1; // cut last char (',')
        cache_file.write(reinterpret_cast<const char *>(intToBytes(buffer_size).data()), sizeof(uint32_t));
        cache_file.write(reinterpret_cast<const char *>(keys.c_str()),
                         buffer_size * sizeof(char));
        cache_file.write(reinterpret_cast<const char *>(values->data()), values->size() * sizeof(V));

        cache_file.close();
        return true;
    }

    template<typename V>
    static bool readDictFromFile(const string &file_path, map<string, V> &dict_p) {
        std::fstream cache_file;
        cache_file.open(file_path.c_str(), std::ios::binary | std::ios::in);
        if (!cache_file.is_open()) {
            Logging::error(__FILE__, __LINE__, "could not open file '" + file_path + '\'');
            return false;
        }
        cache_file.seekg(0, std::fstream::end);
        size_t file_content_size = cache_file.tellg();
        cache_file.seekg(0);
        char file_content[file_content_size];

        // read content of cache_file and fill vector of records with data
        cache_file.read(reinterpret_cast<char *>(file_content), file_content_size);
        cache_file.close();

        uint32_t key_end_idx = read_uint(file_content, 0);

        string keys_raw_s(&file_content[4], key_end_idx);
        std::vector<std::string> keys;
        boost::split(keys, keys_raw_s, boost::is_any_of(","));

        uint32_t pos = key_end_idx + 4;
        const size_t value_type_size = sizeof(V);
        for (auto &key : keys) {
            // copies from stack to heap are described as undefined behavior (UB). Thus, instead we do the copy by hand.
            V v{};
            memcpy(&v, &file_content[pos], value_type_size);
            dict_p[key] = v;
            pos += value_type_size;
        }

        return true;
    }


/**
 * @param data_store
 * @param data_location directory path for the data to be stored
 * @return the number of serialized columns
 */
    static bool serialize_data_store(DataStore &data_store, const string &data_location) {
        bool ret = true;

        auto store = data_store.get_store();
        for (auto &dict_entry : store->choice_dictionaries) {
            const string filename = data_location + "/dict_choice_" + dict_entry.first + ".bin";
            map<string, int> mapping = dict_entry.second;
            writeDictToFile(filename, mapping);
//            ret &= serializeAny<map<string, int>>(filename, mapping);
        }

        for (auto &map_entry : store->strings) {
            const string filename = data_location + "/cache_string_" + map_entry.first + ".bin";
            ret &= rdma::writeColumnToFile(filename, map_entry.second);
        }
        for (auto &map_entry : store->numerics) {
            const string &filename = data_location + "/cache_numeric_" + map_entry.first + ".bin";
            ret &= rdma::writeColumnToFile(filename, map_entry.second);
        }
        for (auto &map_entry : store->choices) {
            const string &filename = data_location + "/cache_choice_" + map_entry.first + ".bin";
            ret &= rdma::writeColumnToFile(filename, map_entry.second);
        }
        store.release();
        return ret;
    }

    static void build_statistics(DataStore *dataStore) {
        auto store = dataStore->get_store();

        for (auto &string_column_entry : store->strings) {
            string column_name = string_column_entry.first;
            store->choice_stats[column_name].count = string_column_entry.second.size();
        }

        for (auto &choice_column_entry : store->choices) {
            string column_name = choice_column_entry.first;
            store->choice_stats[column_name].count = choice_column_entry.second.size();
            store->choice_dictionaries[column_name] = map<string, int>();

            map<string, int> dict = store->choice_dictionaries[column_name];

//            for (int &value : choice_column_entry.second) {
//                if (dict.count(value) == 0) {
//                    dict[value] = (int) dict.size();
//                } else {
//                    dict[value] += 1;
//                }
//            }
        }

        store.release();
    }

    static DataStore * deserialize_data_store(const string &data_location) {
        auto data_store = new DataStore();
        auto dir = opendir(data_location.c_str());

        if (dir == nullptr) {
            string err_no = strerror(errno);
            rdma::Logging::error(__FILE__, __LINE__,
                                 "I/O error while trying to read from dir " + data_location + ": " + err_no);
//            return shared_ptr<DataStore>(data_store);
            return data_store;
        }

        bool ret = true;

        auto ds_ptr = data_store->get_store();
        while (true) {
            auto file = readdir(dir);
            if (file == nullptr)
                break;
            if (string(file->d_name) == "." || string(file->d_name) == "..") {
                continue;
            }

            string filename = file->d_name;
            if (!(std::regex_match(filename, std::regex("^cache_.*_.*[.]bin$")) ||
                  std::regex_match(filename, std::regex("^dict_.*_.*[.]bin$")))) {
                continue;
            }

            // remove file ending (.bin)
            string filename_short = filename.substr(0, filename.find_last_of('.'));

            std::vector<std::string> strs;
            boost::split(strs, filename_short, boost::is_any_of("_"));

            // data structure type: [column, map]
            string ds = strs[0];

            // data type
            column_type type;
            rdma::get_column_type(type, strs[1]);

            string column = strs[2];
            // repair column name if split() did too much
            for (uint32_t i = 3; i < (uint32_t) strs.size(); i++) {
                column += "_" + strs[i];
            }

            const string file_path = data_location + "/" + filename;

            if (ds == "cache") {
                switch (type) {
                    case NUMERIC :
                        ret &= rdma::readColumnFromFile(file_path, ds_ptr->numerics[column]);
                        break;
                    case CHOICE:
                        ret &= rdma::readColumnFromFile(file_path, ds_ptr->choices[column]);
                        break;
                    case STRING:
                        ret &= rdma::readColumnFromFile(file_path, ds_ptr->strings[column]);
                        break;
                    default:
                        closedir(dir);
                        FAIL;
                }
            } else if (ds == "dict") {
//                auto dict = new map<string, int>();
//                readDictFromFile<int>(file_path, *dict);
//                ds_ptr->choice_dictionaries[column] = *dict;
                readDictFromFile<int>(file_path, ds_ptr->choice_dictionaries[column]);
            }
        }
        // clean up
        closedir(dir);
        ds_ptr.release();

        if (ret) {
            build_statistics(data_store);
//            return shared_ptr<DataStore>(data_store);
            return data_store;
        } else {
            throw runtime_error("Could not deserialze cache into store");
        }
    }

}
#endif /* DATAREPOSITORY_H */