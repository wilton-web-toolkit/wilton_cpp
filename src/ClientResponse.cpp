/* 
 * File:   ClientResponse.cpp
 * Author: alex
 * 
 * Created on July 13, 2016, 7:57 AM
 */

#include "wilton/ClientResponse.hpp"

#include "staticlib/icu_utils.hpp"
#include "staticlib/serialization.hpp"

namespace wilton {

namespace { // anonymous

namespace ss = staticlib::serialization;

} // namespace

ClientResponse::ClientResponse(const staticlib::serialization::JsonValue& resp) {
    for (const ss::JsonField& fi : resp.get_object()) {
        if ("headers" == fi.get_uname()) {
            for (const ss::JsonField& he : fi.get_object()) {
                headers.insert(std::make_pair(he.get_uname(), he.get_ustring()));
            }
        } else if ("data" == fi.get_uname()) {
            this->data = fi.get_ustring();
        } else if ("effectiveUrl" == fi.get_uname()) {
            this->effective_url = fi.get_ustring();
        } else if ("responseCode" == fi.get_uname()) {
            this->response_code = fi.get_uint16();
        } else if ("totalTimeSecs" == fi.get_uname()) {
            this->total_time_secs = fi.get_real();
        } else if ("namelookupTimeSecs" == fi.get_uname()) {
            this->namelookup_time_secs = fi.get_real();
        } else if ("connectTimeSecs" == fi.get_uname()) {
            this->connect_time_secs = fi.get_real();
        } else if ("appconnectTimeSecs" == fi.get_uname()) {
            this->appconnect_time_secs = fi.get_real();
        } else if ("pretransferTimeSecs" == fi.get_uname()) {
            this->pretransfer_time_secs = fi.get_real();
        } else if ("starttransferTimeSecs" == fi.get_uname()) {
            this->starttransfer_time_secs = fi.get_real();
        } else if ("redirectTimeSecs" == fi.get_uname()) {
            this->redirect_time_secs = fi.get_real();
        } else if ("redirectCount" == fi.get_uname()) {
            this->redirect_count = fi.get_uint32();
        } else if ("speedDownloadBytesSecs" == fi.get_uname()) {
            this->speed_download_bytes_secs = fi.get_real();
        } else if ("speedUploadBytesSecs" == fi.get_uname()) {
            this->speed_upload_bytes_secs = fi.get_real();
        } else if ("headerSizeBytes" == fi.get_uname()) {
            this->header_size_bytes = fi.get_uint32();
        } else if ("requestSizeBytes" == fi.get_uname()) {
            this->request_size_bytes = fi.get_uint32();
        } else if ("sslVerifyresult" == fi.get_uname()) {
            this->ssl_verifyresult = fi.get_int16();
        } else if ("osErrno" == fi.get_uname()) {
            this->os_errno = fi.get_int32();
        } else if ("numConnects" == fi.get_uname()) {
            this->num_connects = fi.get_uint32();
        } else if ("primaryIp" == fi.get_uname()) {
            this->primary_ip = fi.get_ustring();
        } else if ("primaryPort" == fi.get_uname()) {
            this->primary_port = fi.get_uint16();
        }
    }
}

} // namespace

