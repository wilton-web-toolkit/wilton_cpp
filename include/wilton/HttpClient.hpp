/* 
 * File:   HttpClient.hpp
 * Author: alex
 *
 * Created on July 12, 2016, 2:17 PM
 */

#ifndef WILTON_HTTPCLIENT_HPP
#define	WILTON_HTTPCLIENT_HPP

#include <functional>

#include <unicode/unistr.h>
#include "staticlib/serialization/JsonValue.hpp"
#include "staticlib/pimpl.hpp"

#include "wilton/ClientResponse.hpp"
#include "wilton/WiltonException.hpp"

namespace wilton {

class HttpClient : public staticlib::pimpl::PimplObject {
protected:
    /**
     * Implementation class
     */
    class Impl;
public:
    /**
     * PIMPL-specific constructor
     * 
     * @param pimpl impl object
     */
    PIMPL_CONSTRUCTOR(HttpClient)

    HttpClient(const staticlib::serialization::JsonValue& conf = staticlib::serialization::JsonValue());
    
    ClientResponse execute(const icu::UnicodeString& url,
            const icu::UnicodeString& data = icu::UnicodeString(), 
            const staticlib::serialization::JsonValue& metadata = staticlib::serialization::JsonValue());
    
    ClientResponse send_file(const icu::UnicodeString& url, const icu::UnicodeString& file_path,
            const staticlib::serialization::JsonValue& metadata = staticlib::serialization::JsonValue(),
            std::function<void(bool)> finalizer = [](bool){});
};

} // namespace

#endif	/* WILTON_HTTPCLIENT_HPP */

