
#pragma once

#include "../../src/config/Config.h"

using namespace rdma;

class TestConfig : public CppUnit::TestFixture {
    DPI_UNIT_TEST_SUITE(TestConfig);
    DPI_UNIT_TEST(loadConfigFile);
    DPI_UNIT_TEST_SUITE_END();

public:
    void setUp();

    void tearDown();

    void loadConfigFile();

private:
    string program_name = "testconfig";
};