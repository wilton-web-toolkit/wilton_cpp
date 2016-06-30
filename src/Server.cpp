/* 
 * File:   Server.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Server.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "wilton/wilton.h"

namespace wilton {

namespace { // anonymous

namespace ss = staticlib::serialization;
typedef std::map<icu::UnicodeString, std::function<void(const Request& req, Response& resp)>> callbacks_map_type;

} // namespace

class Server::Impl : public staticlib::pimpl::PimplObject::Impl {
public:
    Impl(const ss::JsonValue& conf, callbacks_map_type callbacks) { 
        (void) conf;
        (void) callbacks;
    }

    void stop(Server&) {
        throw WiltonException("");
    }

};
PIMPL_FORWARD_CONSTRUCTOR(Server, (const ss::JsonValue&)(callbacks_map_type), (), WiltonException)
PIMPL_FORWARD_METHOD(Server, void, stop, (), (), WiltonException)

} // namespace
