/* 
 * File:   Response.hpp
 * Author: alex
 *
 * Created on June 30, 2016, 11:04 AM
 */

#ifndef WILTON_RESPONSE_HPP
#define	WILTON_RESPONSE_HPP

#include <cstdint>
#include <functional>
#include <map>

#include <unicode/unistr.h>
#include "staticlib/pimpl.hpp"

#include "wilton/WiltonException.hpp"

namespace wilton {

class Response : public staticlib::pimpl::PimplObject {
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
    PIMPL_CONSTRUCTOR(Response)
    
    void set_status_code(uint16_t code);
    
    void set_status_message(const icu::UnicodeString& message);
    
    void set_header(const icu::UnicodeString& name, const icu::UnicodeString& value);
    
    void send(const icu::UnicodeString& data);
    
    void send_file(const icu::UnicodeString& filepath, 
            std::function<void(bool)> finalizer = [](bool) {});

    Response(void* /* wilton_Request* */);
};

} // namespace

#endif	/* WILTON_RESPONSE_HPP */

