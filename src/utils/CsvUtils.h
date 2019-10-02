//
// Created by chris on 02.10.19.
//

#ifndef RDMA_CSVUTILS_H
#define RDMA_CSVUTILS_H

#include "../data/DataStore.h"

namespace rdma {

    const char DELIMITER = ',';

    static bool next_csv_value(const string &line, int &pos, string &value) {
        int next_pos = line.find(DELIMITER, pos);
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
    static void read_csv_file(const string &csv_file_path, rdma::DataStore &ds, Func parse_line) {
        std::fstream csv_file;

        csv_file.open(csv_file_path.c_str(), std::ios::in);

        string header_line;
        getline(csv_file, header_line);
        std::vector<std::string> header;

        boost::split(header, header_line, boost::is_any_of((char*)&DELIMITER));

        auto store_ptr = ds.get_store();
        header_to_schema(header, store_ptr.get());
        store_ptr.release();

        string line;
        while (getline(csv_file, line)) {
            boost::algorithm::trim(line);
            if (line.empty() || line == "\n") {
                break;
            }
            parse_line(ds, line, header);
        }
        csv_file.close();

        update_statistics(ds);
    }
}


#endif //RDMA_CSVUTILS_H
