//
// Created by chris on 19.09.19.
//

#ifndef RDMA_STRINGHELPER_H
#define RDMA_STRINGHELPER_H

#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>


namespace rdma {

    namespace bt = boost::posix_time;

    const std::locale formats[] = {
            std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%m-%dT%H:%M:%S")),
            std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%m-%dT%H:%M")),
            std::locale(std::locale::classic(), new bt::time_input_facet("%Y-%m-%d")),
            std::locale(std::locale::classic(), new bt::time_input_facet("%H:%M:%S")),
            std::locale(std::locale::classic(), new bt::time_input_facet("%H:%M"))
    };

#define DATE_TIME_FORMAT 0
#define DATE_TIME_FORMAT_NO_SECONDS 1
#define DATE_FORMAT 2
#define TIME_FORMAT 3
#define TIME_FORMAT_NO_SECONDS 4

    const size_t formats_n = sizeof(formats) / sizeof(formats[0]);

    std::time_t pt_to_time_t(const bt::ptime &pt) {
        bt::ptime timet_start(boost::gregorian::date(1970, 1, 1));
        bt::time_duration diff = pt - timet_start;
        return diff.ticks() / bt::time_duration::rep_type::ticks_per_second;
    }

    long seconds_from_epoch(const std::string &input_string, uint32_t format_idx) {
        bt::ptime pt;
//        for (size_t i = 0; i < formats_n; ++i) {
//            std::istringstream is(input_string);
//            is.imbue(formats[i]);
//            is >> pt;
//            if (pt != bt::ptime()) break;
//        }

        std::istringstream is(input_string);
        if (format_idx >= formats_n) {
            Logging::error(__FILE__, __LINE__, "unknown date time parse format");
        }
        is.imbue(formats[format_idx]);
        is >> pt;
        if (pt == bt::ptime()) {
            Logging::warn(__FILE__, __LINE__, "could not parse date: " + input_string);
        }

        return pt_to_time_t(pt);
//        std::cout << " ptime is " << pt << '\n';
//        std::cout << " seconds from epoch are " << pt_to_time_t(pt) << '\n';
    }

    /**
     * Encodes a date as integer of the form <year><month><day>; e.g., the date '1998-01-05' will be encoded as integer
     * 19980105
     *
     * @param date_time_source
     * @param result
     * @return
     */
    bool convertStringToInternalDateType(const std::string &date_time_source,
                                         uint32_t &result) {
        std::vector<std::string> strs;
        boost::split(strs, date_time_source, boost::is_any_of("-"));

        if (strs.size() != 3) {
            return false;
        }

        auto res_value = boost::lexical_cast<uint32_t>(strs[2]);
        res_value += boost::lexical_cast<uint32_t>(strs[1]) * 100;
        res_value += boost::lexical_cast<uint32_t>(strs[0]) * 100 * 100;
        result = res_value;
        return true;
    }


}

#endif //RDMA_STRINGHELPER_H
