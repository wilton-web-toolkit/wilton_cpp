/* 
 * File:   Request.hpp
 * Author: alex
 *
 * Created on June 30, 2016, 11:04 AM
 */

#ifndef WILTON_REQUEST_HPP
#define	WILTON_REQUEST_HPP

#include <map>

#include <unicode/unistr.h>
#include "staticlib/pimpl.hpp"

#include "wilton/WiltonException.hpp"

namespace wilton {

class Request : public staticlib::pimpl::PimplObject {
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
    PIMPL_CONSTRUCTOR(Request)
            
    const icu::UnicodeString& get_http_version() const;
    
    const icu::UnicodeString& get_protocol() const;
    
    const icu::UnicodeString& get_method() const;
    
    const icu::UnicodeString& get_pathname() const;
    
    const icu::UnicodeString& get_quiery() const;
    
    const std::map<icu::UnicodeString, icu::UnicodeString>& get_queries() const;
    
    const icu::UnicodeString& get_url() const;
    
    const std::map<icu::UnicodeString, icu::UnicodeString>& get_headers() const;
    
    icu::UnicodeString get_data() const;
    
    icu::UnicodeString get_data_filename() const;
    
    Request(void* /* wilton_Request* */);
    
};

} // namespace

#endif	/* WILTON_REQUEST_HPP */

