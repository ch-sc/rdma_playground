//
// Created by chris on 18.09.19.
//

#pragma once

#include "../../src/config/Config.h"
#include "../../src/data/DataRepository.h"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

using namespace rdma;

class TestData : public CppUnit::TestFixture {
DPI_UNIT_TEST_SUITE(TestData);
        DPI_UNIT_TEST(testReadAndWriteOfStrings);
        DPI_UNIT_TEST(testReadAndWriteOfNumerics);
        DPI_UNIT_TEST(testSerializationAndDeserialization);
        DPI_UNIT_TEST(testQuery);
    DPI_UNIT_TEST_SUITE_END();

public:
    void setUp() override;

    void tearDown() override;

    void testReadAndWriteOfStrings();

    void testReadAndWriteOfNumerics();

    void testSerializationAndDeserialization();

    void testQuery();

private:
    string data_location = "testlocation";
    string test_csv_file = "data.csv";
    rdma::DataStore data_store = rdma::DataStore();
};

