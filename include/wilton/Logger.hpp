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
    static void debug(const icu::UnicodeString& message);
    
    static void info(const icu::UnicodeString& message);
    
    static void warn(const icu::UnicodeString& message);
    
    static void error(const icu::UnicodeString& message);

};

} // namespace

#endif	/* WILTON_LOGGER_HPP */

