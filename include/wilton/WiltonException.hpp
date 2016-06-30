/* 
 * File:   WiltonException.hpp
 * Author: alex
 *
 * Created on June 30, 2016, 12:56 PM
 */

#ifndef WILTON_WILTONEXCEPTION_HPP
#define	WILTON_WILTONEXCEPTION_HPP

#include <string>

#include "staticlib/config/BaseException.hpp"

namespace wilton {

/**
 * Module specific exception
 */
class WiltonException : public staticlib::config::BaseException {
public:
    /**
     * Default constructor
     */
    WiltonException() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    WiltonException(const std::string& msg) :
    staticlib::config::BaseException(msg) { }

};

} //namespace

#endif	/* WILTON_WILTONEXCEPTION_HPP */

