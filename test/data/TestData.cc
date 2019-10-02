#include "TestData.h"
#include <boost/filesystem.hpp>
//#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/detail/error_code.ipp>
#include <system_error>
//#include <boost/filesystem/detail/>
#include <iostream>
#include <sys/stat.h>
#include <../../src/utils/StringHelper.h>
#include <cmath>

//using boost::filesystem::detail::remove_all;
//using boost::filesystem::detail::status;
namespace fs = boost::filesystem;
namespace detail = boost::filesystem::detail;

static bool next_csv_value(const string &line, int &pos, string &value) {
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

static void read_test_csv_file(const string &csv_file_path, DataStore &data_store) {
    rdma::read_csv_file(csv_file_path, data_store, [](DataStore &ds, string &line, vector<string> header) {
        int pos = 0;
        string value;
        if (next_csv_value(line, pos, value)) {
            ds.add_value(header[0], value);
        }
        if (next_csv_value(line, pos, value)) {
            ds.add_value(header[1], value);
        }
        if (next_csv_value(line, pos, value)) {
            ds.add_value(header[2], (float) rdma::seconds_from_epoch(value, DATE_TIME_FORMAT_NO_SECONDS));
        }
        if (next_csv_value(line, pos, value)) {
            ds.add_value(header[3], std::stof(value));
        }
        if (next_csv_value(line, pos, value)) {
            ds.add_value(header[4], std::stof(value));
        }
    });
}

void TestData::setUp() {
    // fixing output - test framework does not add line break...
    cout << endl;

//    const fs::path &p = fs::path(data_location.c_str());
//    if (!fs::exists(p)) {
    if (mkdir(data_location.c_str(), 0777) == -1) {
        string err_no = strerror(errno);
        rdma::Logging::warn(__FILE__, __LINE__, "could not create test directory. Cause: " + err_no);
        tearDown();
    }
//    }

    std::ofstream csvFile(data_location + "/" + test_csv_file, std::ofstream::out);

    csvFile << "id (string),title (choice),date (numeric),number (numeric),double_number (numeric)\n\
0,AAA,1997-07-16T19:20,1000,0.0001\n\
1,BBB,2007-07-16T19:20,100,0.0010\n\
2,CCC,2017-07-16T19:20,10,0.0100\
  " << std::endl;

    csvFile.close();

    data_store.clear();
    read_test_csv_file(data_location + "/" + test_csv_file, data_store);
}

void TestData::tearDown() {
    // REMOVE FILES AND FOLDERS

    auto dir = opendir(data_location.c_str());
    if (dir == nullptr) {
        string err_no = strerror(errno);
        rdma::Logging::error(__FILE__, __LINE__,
                             "I/O error while trying to read from dir " + data_location + ": " + err_no);
        FAIL;
    }

//    printf("removing cache files...");
    while (true) {
        auto file = readdir(dir);
        if (file == nullptr) {
            break;
        }
        if (string(file->d_name) == "." || string(file->d_name) == "..") {
            continue;
        }
//        printf("remove %s\n", file->d_name);
        std::remove(file->d_name);
    }

    rmdir(data_location.c_str());
    data_store.clear();
}

void TestData::testReadAndWriteOfStrings() {
    // ARRANGE
    ulong write_count = 0;
    ulong read_count = 0;
    rdma::store test_ds = {};
    test_ds.choice_dictionaries = unordered_map<string, map<string, int>>{};
    test_ds.strings = unordered_map<std::string, Column<std::string>>{};
    test_ds.numerics = unordered_map<std::string, Column<float>>{};
    test_ds.choice_stats = unordered_map<std::string, choice_statistics>{};
    test_ds.num_stats = unordered_map<std::string, numeric_statistics>{};


    // ACT
    auto ds_p = data_store.get_store();
    vector<string> file_paths;
    for (auto &map_entry : ds_p->strings) {
        const string file_path = data_location + "/cache_string_" + map_entry.first + ".bin";
        file_paths.push_back(file_path);
        rdma::writeColumnToFile(file_path, map_entry.second);
        test_ds.strings[map_entry.first] = Column<string>();
        write_count++;
    }
    for (auto &map_entry : ds_p->strings) {
        string filename = data_location + "/cache_string_" + map_entry.first + ".bin";
//        rdma::readColumnFromFile(filename, test_ds.strings[map_entry.first]);
        read_count++;
    }

    // ASSERT
    for (string &path : file_paths) {
        struct stat st{};
        stat(path.c_str(), &st);
        CPPUNIT_ASSERT_MESSAGE("FILE HAS CONTENT", st.st_size > 0);
    }

    CPPUNIT_ASSERT_MESSAGE("SERIALIZE_COLUMN_BIN", write_count == ds_p->strings.size());
    CPPUNIT_ASSERT_MESSAGE("SERIALIZE_COLUMN_BIN", read_count == ds_p->strings.size());

    for (uint32_t i = 0; i < ds_p->strings["id"].size(); ++i) {
        cout << ds_p->strings["id"][i] << "==" << test_ds.strings["id"][i] << "?" << endl;
        cout << &ds_p->strings["id"][i] << "!=" << &test_ds.strings["id"][i] << "?" << endl;
        CPPUNIT_ASSERT_EQUAL(ds_p->strings["id"][i], test_ds.strings["id"][i]);
    }
//    test_ds.strings.erase("id");
    for (uint32_t i = 0; i < ds_p->strings["title"].size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(ds_p->strings["title"][i], test_ds.strings["title"][i]);
    }
//    test_ds.strings.erase("title");

    ds_p.release();
}

void TestData::testReadAndWriteOfNumerics() {
    // ARRANGE
    ulong write_count = 0;
    ulong read_count = 0;
    store test_ds{};

    // ACT
    unique_ptr<store> ds_ptr = data_store.get_store();
    for (auto &map_entry : ds_ptr->numerics) {
        rdma::writeColumnToFile(data_location + "/cache_numeric_" + map_entry.first + ".bin", map_entry.second);
        test_ds.strings[map_entry.first] = Column<string>();
        write_count++;
    }
    for (auto &map_entry : ds_ptr->numerics) {
        rdma::readColumnFromFile(data_location + "/cache_numeric_" + map_entry.first + ".bin",
                                 test_ds.numerics[map_entry.first]);
        read_count++;
    }

    // ASSERT
    CPPUNIT_ASSERT_MESSAGE("SERIALIZE_COLUMN_BIN", write_count == ds_ptr->numerics.size());
    CPPUNIT_ASSERT_MESSAGE("SERIALIZE_COLUMN_BIN", read_count == ds_ptr->numerics.size());

    for (uint32_t i = 0; i < ds_ptr->numerics["id"].size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(ds_ptr->numerics["id"][i], test_ds.numerics["id"][i]);
    }
    for (uint32_t i = 0; i < ds_ptr->numerics["title"].size(); ++i) {
        CPPUNIT_ASSERT_EQUAL(ds_ptr->numerics["title"][i], test_ds.numerics["title"][i]);
    }

    ds_ptr.release();
}

void TestData::testSerializationAndDeserialization() {
    // ARRANGE

    // ACT
    bool success = rdma::serialize_data_store(data_store, data_location);
    auto test_data_store = rdma::deserialize_data_store(data_location);
    auto ref_ds_ptr = this->data_store.get_store();
    auto test_ds_ptr = test_data_store->get_store();

    // ASSERT
    CPPUNIT_ASSERT_MESSAGE("RESULT SUCCESS", success);
    CPPUNIT_ASSERT_MESSAGE("SAME AMOUNT STRING COLUMNS",
                           ref_ds_ptr->strings.size() == test_ds_ptr->strings.size());
    for (auto &col_entry : ref_ds_ptr->strings) {
        auto test_col = test_ds_ptr->strings[col_entry.first];
        for (ulong i = 0; i < col_entry.second.size(); ++i) {
            CPPUNIT_ASSERT_MESSAGE("SAME VALUE", col_entry.second[i] == test_col[i]);
        }
    }
    CPPUNIT_ASSERT_MESSAGE("SAME AMOUNT NUMERIC COLUMNS",
                           ref_ds_ptr->numerics.size() == test_ds_ptr->numerics.size());
    for (auto &col_entry : ref_ds_ptr->numerics) {
        auto test_col = test_ds_ptr->numerics[col_entry.first];
        for (ulong i = 0; i < col_entry.second.size(); ++i) {
            CPPUNIT_ASSERT_MESSAGE("SAME VALUE", col_entry.second[i] == test_col[i]);
        }
    }
    CPPUNIT_ASSERT_MESSAGE("SAME AMOUNT CHOICE COLUMNS",
                           ref_ds_ptr->choices.size() == test_ds_ptr->choices.size());
    for (auto &col_entry : ref_ds_ptr->choices) {
        auto test_col = test_ds_ptr->choices[col_entry.first];
        for (ulong i = 0; i < col_entry.second.size(); ++i) {
            CPPUNIT_ASSERT_MESSAGE("SAME VALUE", col_entry.second[i] == test_col[i]);
        }
    }
    CPPUNIT_ASSERT_MESSAGE("SAME AMOUNT DICT ENTRIES",
                           ref_ds_ptr->choice_dictionaries.size() == test_ds_ptr->choice_dictionaries.size());
    for (auto &dict_entry : ref_ds_ptr->choice_dictionaries) {
        map<string, int> test_map = test_ds_ptr->choice_dictionaries[dict_entry.first];
        for (auto &entry : dict_entry.second) {
            CPPUNIT_ASSERT_MESSAGE("KEY FOUND", test_map.count(entry.first) > 0);
            CPPUNIT_ASSERT_MESSAGE("SAME VALUE", test_map[entry.first] == entry.second);
        }
    }
    ref_ds_ptr.release();
    auto p = test_ds_ptr.release();
}


void test_query(unique_ptr<store> data_store) {
    auto titles = data_store->choices["title (choice)"];
    auto title_stats = data_store->choice_stats["title (choice)"];

    auto stats = data_store->strings["id (text)"];

    auto number = data_store->numerics["number (numeric)"];
    auto number_stats = data_store->num_stats["number (numeric)"];

    float ratio = (float) title_stats.cardinality / (float) max((int) title_stats.count, 1);

    for (auto &t : titles) {

    }
}

void TestData::testBenchmarkQuery() {
    // Arrange
    auto data_store = rdma::deserialize_data_store(data_location);

//    test_query(data_store_p->get_store());
}



