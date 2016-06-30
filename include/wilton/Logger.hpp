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
            
    Logger(const icu::UnicodeString& logger_name);
    
    void debug(const icu::UnicodeString& message);
    
    void info(const icu::UnicodeString& message);
    
    void warn(const icu::UnicodeString& message);
    
    void error(const icu::UnicodeString& message);

};

} // namespace

#endif	/* WILTON_LOGGER_HPP */

