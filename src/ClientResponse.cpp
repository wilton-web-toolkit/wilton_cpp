/* 
 * File:   ClientResponse.cpp
 * Author: alex
 * 
 * Created on July 13, 2016, 7:57 AM
 */

#include "wilton/ClientResponse.hpp"

#include "staticlib/icu_utils.hpp"
#include "staticlib/ranges.hpp"
#include "staticlib/serialization.hpp"

namespace wilton {

namespace { // anonymous

namespace sr = staticlib::ranges;
namespace ss = staticlib::serialization;

} // namespace

ClientResponse::ClientResponse(const staticlib::serialization::JsonValue& resp) {
    for (const ss::JsonField& fi : resp.as_object()) {
        if ("headers" == fi.uname()) {
            for (const ss::JsonField& he : fi.as_object()) {
                headers.insert(std::make_pair(he.uname(), he.as_ustring()));
            }
        } else if ("data" == fi.uname()) {
            this->data = fi.as_ustring();
        } else if ("effectiveUrl" == fi.uname()) {
            this->effective_url = fi.as_ustring();
        } else if ("responseCode" == fi.uname()) {
            this->response_code = fi.as_uint16();
        } else if ("totalTimeSecs" == fi.uname()) {
            this->total_time_secs = fi.as_double();
        } else if ("namelookupTimeSecs" == fi.uname()) {
            this->namelookup_time_secs = fi.as_double();
        } else if ("connectTimeSecs" == fi.uname()) {
            this->connect_time_secs = fi.as_double();
        } else if ("appconnectTimeSecs" == fi.uname()) {
            this->appconnect_time_secs = fi.as_double();
        } else if ("pretransferTimeSecs" == fi.uname()) {
            this->pretransfer_time_secs = fi.as_double();
        } else if ("starttransferTimeSecs" == fi.uname()) {
            this->starttransfer_time_secs = fi.as_double();
        } else if ("redirectTimeSecs" == fi.uname()) {
            this->redirect_time_secs = fi.as_double();
        } else if ("redirectCount" == fi.uname()) {
            this->redirect_count = fi.as_uint32();
        } else if ("speedDownloadBytesSecs" == fi.uname()) {
            this->speed_download_bytes_secs = fi.as_double();
        } else if ("speedUploadBytesSecs" == fi.uname()) {
            this->speed_upload_bytes_secs = fi.as_double();
        } else if ("headerSizeBytes" == fi.uname()) {
            this->header_size_bytes = fi.as_uint32();
        } else if ("requestSizeBytes" == fi.uname()) {
            this->request_size_bytes = fi.as_uint32();
        } else if ("sslVerifyresult" == fi.uname()) {
            this->ssl_verifyresult = fi.as_int16();
        } else if ("osErrno" == fi.uname()) {
            this->os_errno = fi.as_int32();
        } else if ("numConnects" == fi.uname()) {
            this->num_connects = fi.as_uint32();
        } else if ("primaryIp" == fi.uname()) {
            this->primary_ip = fi.as_ustring();
        } else if ("primaryPort" == fi.uname()) {
            this->primary_port = fi.as_uint16();
        }
    }
}

ss::JsonValue ClientResponse::to_json() {
    auto ra = sr::transform(sr::refwrap(headers), [](const std::pair<const icu::UnicodeString, icu::UnicodeString>& pa) {
        return ss::JsonField{pa.first, pa.second};
    });
    auto vec = sr::emplace_to_vector(std::move(ra));
    
    return {
        {"headers", std::move(vec)},
        {"data", data},
        {"effectiveUrl", effective_url},
        {"responseCode", response_code},
        {"totalTimeSecs", total_time_secs},
        {"namelookupTimeSecs", namelookup_time_secs},
        {"connectTimeSecs", connect_time_secs},
        {"appconnectTimeSecs", appconnect_time_secs},
        {"pretransferTimeSecs", pretransfer_time_secs},
        {"starttransferTimeSecs", starttransfer_time_secs},
        {"redirectTimeSecs", redirect_time_secs},
        {"redirectCount", redirect_count},
        {"speedDownloadBytesSecs", speed_download_bytes_secs},
        {"speedUploadBytesSecs", speed_upload_bytes_secs},
        {"headerSizeBytes", header_size_bytes},
        {"requestSizeBytes", request_size_bytes},
        {"sslVerifyresult", ssl_verifyresult},
        {"osErrno", os_errno},
        {"numConnects", num_connects},
        {"primaryIp", primary_ip},
        {"primaryPort", primary_port}
    };
}

} // namespace

