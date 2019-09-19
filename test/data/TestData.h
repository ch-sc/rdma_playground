//
// Created by chris on 18.09.19.
//

#pragma once

#include "../../src/config/Config.h"
#include "../../src/data/DataRepository.h"

using namespace rdma;

class TestData : public CppUnit::TestFixture {
    DPI_UNIT_TEST_SUITE(TestData);
    DPI_UNIT_TEST(testWriteDataToFile);
    DPI_UNIT_TEST(testReadDataFromFile);
    DPI_UNIT_TEST_SUITE_END();

public:
    /**
     * do some preparation before all tests
     */
    void setUp();

    /**
     * clean up after all tests
     */
    void tearDown();

    void testWriteDataToFile();

    void testReadDataFromFile();

private:
    string data_location = "testlocation";
    string test_csv_file = "data.csv";
    rdma::store data_store{};
};

