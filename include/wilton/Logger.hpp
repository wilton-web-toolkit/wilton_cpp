/* 
 * File:   Logger.hpp
 * Author: alex
 *
 * Created on June 30, 2016, 11:06 AM
 */

#ifndef WILTON_LOGGER_HPP
#define	WILTON_LOGGER_HPP

#include <unicode/unistr.h>

#include "staticlib/pimpl.hpp"
#include "staticlib/serialization/JsonValue.hpp"

#include "wilton/WiltonException.hpp"

namespace wilton {

class Logger : public staticlib::pimpl::PimplObject {
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
    PIMPL_CONSTRUCTOR(Logger)
            
    static void init_logging(const staticlib::serialization::JsonValue& conf = staticlib::serialization::JsonValue());
            
    Logger(const icu::UnicodeString& logger_name);
    
    void debug(const icu::UnicodeString& message);
    
    bool is_debug_enabled();
    
    void info(const icu::UnicodeString& message);
    
    bool is_info_enabled();
    
    void warn(const icu::UnicodeString& message);
    
    bool is_warn_enabled();
    
    void error(const icu::UnicodeString& message);
        
    bool is_error_enabled();

};

} // namespace

#endif	/* WILTON_LOGGER_HPP */

