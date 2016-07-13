/* 
 * File:   ClientResponse.hpp
 * Author: alex
 *
 * Created on July 13, 2016, 7:33 AM
 */

#ifndef WILTON_CLIENTRESPONSE_HPP
#define	WILTON_CLIENTRESPONSE_HPP

#include <cstdint>
#include <map>

#include <unicode/unistr.h>
#include "staticlib/serialization/JsonValue.hpp"

#include "wilton/WiltonException.hpp"

namespace wilton {

struct ClientResponse {   
    std::map<icu::UnicodeString, icu::UnicodeString> headers;
    icu::UnicodeString data;
    icu::UnicodeString effective_url;
    uint16_t response_code;
    double total_time_secs;
    double namelookup_time_secs;
    double connect_time_secs;
    double appconnect_time_secs;
    double pretransfer_time_secs;
    double starttransfer_time_secs;
    double redirect_time_secs;
    uint32_t redirect_count;
    double speed_download_bytes_secs;
    double speed_upload_bytes_secs;
    uint32_t header_size_bytes;
    uint32_t request_size_bytes;
    int16_t ssl_verifyresult;
    int32_t os_errno;
    uint32_t num_connects;
    icu::UnicodeString primary_ip;
    uint16_t primary_port;
    
    ClientResponse(const staticlib::serialization::JsonValue& resp);
    
    staticlib::serialization::JsonValue to_json();
    
};

} // namespace

#endif	/* WILTON_CLIENTRESPONSE_HPP */

