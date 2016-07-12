/* 
 * File:   Server.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Server.hpp"

#include <memory>
#include <string>

#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "staticlib/serialization.hpp"
#include "wilton/wilton.h"

namespace wilton {

namespace { // anonymous

namespace ss = staticlib::serialization;
using callbacks_map_type = std::map<icu::UnicodeString, std::function<void(const Request& req, Response& resp)>>;

class ServerDeleter {
public:
    void operator()(wilton_Server* server) {
        wilton_Server_stop_server(server);
    }
};

} // namespace

class Server::Impl : public staticlib::pimpl::PimplObject::Impl {
    callbacks_map_type callbacks;
    std::unique_ptr<wilton_Server, ServerDeleter> ptr;
    
public:
    Impl(const ss::JsonValue& conf, callbacks_map_type callbacks) :
    callbacks(std::move(callbacks)) { 
        std::string conf_str = ss::dump_json_to_string(conf);
//        wilton_Server* srv_ptr;
//        char* err = wilton_Server_create(
//                wilton_Server** std::adressof(srv_ptr),
//                void* this,
//                void (*gateway_cb)(
//                void* gateway_ctx,
//                wilton_Request * request),
//                const char* conf_json,
//                int conf_json_len);
//        this->ptr = std::unique_ptr<wilton_Server, ServerDeleter>(
//                std,
//                ServerDeleter());
    }

};
PIMPL_FORWARD_CONSTRUCTOR(Server, (const ss::JsonValue&)(callbacks_map_type), (), WiltonException)

} // namespace
