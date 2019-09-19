//
// Created by chris on 17.09.19.
//

#ifndef RDMA_DATASTORE_H
#define RDMA_DATASTORE_H

#include <iostream>
#include <iterator>
#include <vector>
#include <any>
#include <boost/algorithm/string.hpp>

namespace rdma {

//    template <class T>
//    using Column = std::vector<T, boost::alignment::aligned_allocator<T, 32>>;
    template<typename T, typename Allocator = std::allocator<T>>
    using Column = std::vector<T, Allocator>;

    struct choice_statistics {
        uint32_t cardinality;
    };

    struct numeric_statistics_i {
        int max;
        int min;
        float avg;
        uint32_t count;
    };

    struct numeric_statistics_f {
        float max;
        float min;
        float avg;
        uint32_t count;
    };

    struct store {
        unordered_map<string, Column<std::map<string, int>>> choice_dictionaries;
        unordered_map<string, Column<std::map<int, std::any>>> resolved_dictionaries;

        map<string, Column<string>> strings;
//        unordered_map<string, Column<int>> numerics_i;
        unordered_map<string, Column<float>> numerics;

        unordered_map<string, choice_statistics> choice_stats;
//        unordered_map<string, numeric_statistics_i> num_stats_i;
        unordered_map<string, numeric_statistics_f> num_stats_f;
    };

    static
    bool get_next_value(string &line, int &pos, string &value) {
        int next_pos = line.find(',', pos);
        if (next_pos < 0) {
            if (line.empty()) return false;

            value = line.substr(pos, line.length());
            pos = line.length() - 1;
            return true;
        }
        value = line.substr(pos, next_pos - pos);
        pos = next_pos + 1;
        return true;
    }

    template<typename Func>
    static
    rdma::store read_csv_file(const string &csv_file_path, Func parse_line) {
        std::fstream csv_file;

        csv_file.open(csv_file_path.c_str(), std::ios::in);

        string header_line;
        getline(csv_file, header_line);
        cout << "HEADER: " << header_line << endl;
        std::vector<std::string> header;

        boost::split(header, header_line,boost::is_any_of(","));

        string line;

        store data_store{};

        while (getline(csv_file, line)) {

            boost::algorithm::trim(line, locale::classic());

            if (line.empty() || line == "\n") break;

            parse_line(data_store, line, header);
        }

        csv_file.close();
        return data_store;
    }

}

#endif