#ifndef RDMA_DATASTORE_H
#define RDMA_DATASTORE_H

#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <any>
#include <boost/align/aligned_allocator.hpp>
#include <boost/algorithm/string.hpp>
#include "../utils/ErrorManager.h"
#include <regex>

namespace rdma {

    template<class T>
    using Column = std::vector<T, boost::alignment::aligned_allocator<T, 32>>;

    enum column_type {
        CHOICE,
        STRING,
        NUMERIC
    };

    const string column_type_names[] = {"choice", "string", "numeric"};

    static
    void get_column_type(column_type &type, string &from) {
        if (from.compare("string") == 0) {
            type = column_type::STRING;
        } else if (from.compare("choice") == 0) {
            type = column_type::CHOICE;
        } else if (from.compare("numeric") == 0) {
            type = column_type::NUMERIC;
        } else {
            rdma::Logging::error(__FILE__, __LINE__, "column type does not exist: " + from);
        }
    }

    struct choice_statistics {
        uint32_t cardinality;
        uint32_t count;
    };

    struct numeric_statistics {
        float max;
        float min;
        float avg;
        float sum;
        uint32_t count;
    };

    struct store {
        // dictionaries
        unordered_map<string, column_type> schema;
        unordered_map<string, map<string, int>> choice_dictionaries;

        // columns
        unordered_map<string, Column<string>> strings;
        unordered_map<string, Column<int>> choices;
        unordered_map<string, Column<float>> numerics;

        // stats
        unordered_map<string, choice_statistics> choice_stats;
        unordered_map<string, numeric_statistics> num_stats;
    };

    class DataStore {
    public:

        store data_store;

        DataStore() {
            this->data_store = store{};
        }

        ~DataStore() {}

        void clear() {
            this->data_store = store{};
        }

        void add_value(string column, string value) {
            column_type column_type = this->data_store.schema[column];
            switch (column_type) {
                case STRING:
                    add_string_value(column, value);
                    break;
                case CHOICE:
                    add_choice_value(column, value);
                    break;
                default:
                    rdma::Logging::error(__FILE__, __LINE__,
                                         "type violation! column " + column + " is not a string or choice type");
            }
        }

        void add_value(string column, float value) {
            column_type column_type = this->data_store.schema[column];
            if (column_type != NUMERIC)
                rdma::Logging::error(__FILE__, __LINE__, "type violation! column " + column + " is not a numeric type");
            add_numeric_value(column, value);
        }


        inline
        Column<string> *get_string_column(string &column_name) {
            return &data_store.strings[column_name];
        }

        inline
        Column<int> *get_choice_column(string &column_name) {
            return &data_store.choices[column_name];
        }

        inline
        Column<float> *get_numeric_column(string &column_name) {
            return &data_store.numerics[column_name];
        }

        inline
        unique_ptr<store> get_store() {
            return unique_ptr<rdma::store>(&this->data_store);
        }

    private:

        void add_string_value(string &column, string &value) {
            get_string_column(column)->push_back(value);
        }

        void add_choice_value(string &column, string &value) {
            map<std::string, int> &dict = this->data_store.choice_dictionaries[column];

            int key;
            if (!dict.count(value)) {
                key = dict.size();
                dict[value] = dict.size();
            } else {
                key = dict[value];
            }

            get_choice_column(column)->push_back(key);

            // update statistics
            choice_statistics &stats = data_store.choice_stats[column];
            stats.count++;
        }

        void add_numeric_value(string &column, float &value) {
            get_numeric_column(column)->push_back(value);

            // update statistics
            numeric_statistics &stats = this->data_store.num_stats[column];
            stats.sum += value;
            if (stats.max < value) {
                stats.max = value;
            } else if (stats.min > value) {
                stats.min = value;
            }
        }
    };

    static void update_statistics(rdma::DataStore &ds) {
        auto store_p = ds.get_store();

        for (auto &choice_entry : store_p->choice_dictionaries) {
            choice_statistics &choice_stats = store_p->choice_stats[choice_entry.first];
            choice_stats.cardinality = ((map<string, int>) choice_entry.second).size();
            choice_stats.count = store_p->choices[choice_entry.first].size();
        }

        for (auto &num_entry : store_p->numerics) {
            numeric_statistics &stats = store_p->num_stats[num_entry.first];
            stats.count = num_entry.second.size();
            stats.avg = stats.sum / stats.count;
        }

        store_p.release();
    }

    static void header_to_schema(vector<string> &headers, store *store) {
        for (string &header : headers) {
            string lower_header = boost::algorithm::to_lower_copy(header, locale::classic());
            if ((long) lower_header.find("choice") >= (long) 0) {
                store->schema[header] = CHOICE;
            } else if ((long) lower_header.find("numeric") >= (long) 0) {
                store->schema[header] = NUMERIC;
            } else {
                // default case is string
                store->schema[header] = STRING;
            }
        }

#ifdef DEBUG
        string log = "SCHEMA: ";
        for (auto &schema_entry : store->schema) {
            log += "'" + schema_entry.first + "':" + column_type_names[schema_entry.second] + '|';
        }
        Logging::debug(log);
#endif
//        store;
    }

    template<typename Func>
    static void read_csv_file(const string &csv_file_path, rdma::DataStore &ds, Func parse_line) {
        std::fstream csv_file;

        csv_file.open(csv_file_path.c_str(), std::ios::in);

        string header_line;
        getline(csv_file, header_line);
        std::vector<std::string> header;

        boost::split(header, header_line, boost::is_any_of(","));

//        auto *s = (rdma::store *) malloc(sizeof(rdma::store));

//        auto *ds = (rdma::DataStore *) malloc(sizeof(rdma::DataStore));
//        ds = new(ds, s);
//        rdma::store s = *ds->get_store();

//        store s{};
//        DataStore ds = *new DataStore(s);

        auto store_ptr = ds.get_store();
        header_to_schema(header, store_ptr.get());
        store_ptr.release();

        string line;
        while (getline(csv_file, line)) {

            boost::algorithm::trim(line);

            if (line.empty() || line == "\n") break;

            parse_line(ds, line, header);
        }

        csv_file.close();

        update_statistics(ds);
    }

}

#endif