//CPPUnit
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <strings.h>

#include "../src/config/Config.h"
#include "Tests.h"

#define no_argument 0
#define required_argument 1
#define optional_argument 2

enum run_mode {
    LOCAL, REMOTE
};

struct config_t {
    run_mode runmode = run_mode::LOCAL;
    int test_number = 0;
    int port = 0;
};

static void usage() {
    cout << "rdma_test -n #test_number" << endl;
    cout << endl;
    cout << "Tests RDMA:" << endl;

//    cout << "101: \t rdma/TestRDMAServer" << endl;
//    cout << "102: \t rdma/TestRDMAServerMultipleClients" << endl;
//    cout << "103: \t rdma/TestSimpleUD" << endl;
//    cout << "104: \t rdma/TestRDMAServerMCast" << endl;
//    cout << "105: \t rdma/TestRDMAServerSRQ" << endl;
//    cout << "106: \t RPC/TestRDMARPC" << endl;

    cout << "Tests MISC:" << endl;
    cout << "201: \t config/TestConfig" << endl;
//    cout << "202: \t thread/TestThread" << endl;
//    cout << "203: \t proto/TestProtoServer" << endl;
    cout << "204: \t data/TestData" << endl;

    cout << endl;
}

static void runtest(int t) {
    // Adds the test to the list of test to run
    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that print dots as test run.
    CPPUNIT_NS::BriefTestProgressListener progress;
    controller.addListener(&progress);

    //controller.push

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;

    //
    //  CppUnit::TextUi::TestRunner runner;
    //
    //  // Change the default outputter to a compiler error format outputter
    //  runner.setOutputter(
    //      new CppUnit::CompilerOutputter(&runner.result(), std::cerr));


    switch (t) {
        //RDMA
//        case 101:
//            runner.addTest(TestRDMAServer::suite());
//            break;
//        case 102:
//            runner.addTest(TestRDMAServerMultClients::suite());
//            break;
//        case 103:
//            runner.addTest(TestSimpleUD::suite());
//            break;
//        case 104:
//            runner.addTest(TestRDMAServerMCast::suite());
//            break;
//        case 105:
//            runner.addTest(TestRDMAServerSRQ::suite());
//            break;
//
//        case 106:
//            runner.addTest(TestRPC::suite());
//            break;
        //MISC
        case 201:
            runner.addTest(TestConfig::suite());
            break;
//        case 202:
//            runner.addTest(TestThread::suite());
//            break;
//        case 203:
//            runner.addTest(TestProtoServer::suite());
//            break;
        case 204:
            runner.addTest(TestData::suite());
            break;
        default:
            std::cerr << "No test with test_number " << t << " exists." << endl;
            return;
    }

    runner.run(controller);

    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter(&result, std::cerr);
    outputter.write();
}

int main(int argc, char *argv[]) {
    struct config_t config;
    struct option long_options[] = {
            {"number", required_argument, 0, 'n'},
            {"port", optional_argument, 0, 'p'},
            {"mode", optional_argument, 0, 'm'}
    };

    while (1) {
        int c = getopt_long(argc, argv, "n:p:m:", long_options, NULL);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'n':
                config.test_number = strtoul(optarg, NULL, 0);
                break;
            case 'p':
                config.port = strtoul(optarg, NULL, 0);
                break;
            case 'm':
                if (strncasecmp(optarg, "REMOTE", 6) == 0) {
                    config.runmode = REMOTE;
                } else if (strncasecmp(optarg, "LOCAL", 5) == 0) {
                    config.runmode = LOCAL;
                } else {
                    std::cerr << "unknown value for option [-m <mode>]. possible values: [" << LOCAL << "," << REMOTE
                              << "]." << std::endl;
                    return 1;
                }
                break;
            default:
                usage();
                return 1;
        }
    }

    std::cout << "missing option [-n <test number>]" << endl;

    std::cout << "run test: " << config.test_number << '\n'
              << "on port: " << config.port << '\n'
              << "in mode: " << config.runmode << std::endl;

    // load config
    string exec_path = string(argv[0]);
    static Config conf(exec_path);

    if (config.test_number > 0) {
        runtest(config.test_number);
        return 0;
    }

    usage();
}