#include "TestData.h"
#include <iostream>
#include <sys/stat.h>
#include <../../src/utils/StringHelper.h>


rdma::store read_test_csv_file(const string &csv_file_path) {
    return rdma::read_csv_file(csv_file_path, [](store &ds, string l, vector<string> header) {
        int pos = 0;
        string id, title, date_s, number, double_number;
        if (rdma::get_next_value(l, pos, id)) {
            ds.strings[header[0]].push_back(id);
        }
        if (rdma::get_next_value(l, pos, title)) {
            ds.strings[header[1]].push_back(title);
        }
        if (rdma::get_next_value(l, pos, date_s)) {
            ds.numerics[header[2]].push_back((float) rdma::seconds_from_epoch(date_s, DATE_TIME_FORMAT_NO_SECONDS));
        }
        if (rdma::get_next_value(l, pos, number)) {
            ds.numerics[header[3]].push_back(std::stof(number));
        }
        if (rdma::get_next_value(l, pos, double_number)) {
            ds.numerics[header[4]].push_back(std::stof(double_number));
        }

        cout << "record: " << id << "," << title << "," << date_s << "," << number << "," << double_number << endl;
    });
}

void TestData::setUp() {
    // test framework does not add line break
    cout << endl;

    if (mkdir(data_location.c_str(), 0777) == -1) {
        string err_no = strerror(errno);
        rdma::Logging::warn(__FILE__, __LINE__, "could not create test directory. Cause: " + err_no);
    }

    std::ofstream confFile(data_location + "/" + test_csv_file, std::ofstream::out);

    confFile << "id,title,date,number,double_number\n\
0,AAA,1997-07-16T19:20,1000,0.0001\n\
1,BBB,2007-07-16T19:20,100,0.0010\n\
2,CCC,2017-07-16T19:20,10,0.0100\n\
  " << std::endl;

    confFile.close();

    data_store = read_test_csv_file(data_location + "/" + test_csv_file);
}

void TestData::tearDown() {
    // REMOVE FILES AND FOLDERS
    string filename = data_location + "/" + test_csv_file;
    std::remove(filename.c_str());
    rmdir(data_location.c_str());
}


void TestData::testWriteDataToFile() {
    // ARRANGE
    ulong i = 0;

    // ACT
    for (auto &map_entry : data_store.strings) {
//        std::cout << it->first << " => " << it->second << '\n';
        rdma::writeColumnToFile(data_location + "/cache_string_" + map_entry.first + ".bin", map_entry.second);
        i++;
    }

    // ASSERT
    CPPUNIT_ASSERT_MESSAGE("SERIALIZE_COLUMN_BIN", i == data_store.strings.size());
}

void TestData::testReadDataFromFile() {
    // ARRANGE

    // ACT
//    for (ulong i = 0;
//         i < ds.strings.size() && rdma::readColumnFromFile(data_location + "/cache.bin", ds.strings[i]);
//         ++i);
    // ASSERT
//    rdma::readColumnFromFile(data_location + "/cache.bin",)
}