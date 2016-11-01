/* 
 * File:   Server.hpp
 * Author: alex
 *
 * Created on June 30, 2016, 10:42 AM
 */

#ifndef WILTON_SERVER_HPP
#define	WILTON_SERVER_HPP

#include <map>

#include <unicode/unistr.h>
#include "staticlib/serialization/JsonValue.hpp"
#include "staticlib/pimpl.hpp"

#include "wilton/Request.hpp"
#include "wilton/Response.hpp"
#include "wilton/WiltonException.hpp"

namespace wilton {

class Server : public staticlib::pimpl::PimplObject {
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
    PIMPL_CONSTRUCTOR(Server)
            
Server(const staticlib::serialization::JsonValue& conf, 
            std::map<icu::UnicodeString, std::function<void(const Request& req, Response& resp)>> callbacks = {});
};

} // namespace

#endif	/* WILTON_SERVER_HPP */

