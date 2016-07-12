/* 
 * File:   Server.cpp
 * Author: alex
 * 
 * Created on June 30, 2016, 12:08 PM
 */

#include "wilton/Server.hpp"

#include <memory>
#include <string>

#include "staticlib/icu_utils.hpp"
#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "staticlib/serialization.hpp"

#include "wilton/wilton.h"
#include "wilton/Logger.hpp"

namespace wilton {

namespace { // anonymous

namespace siu = staticlib::icu_utils;
namespace ss = staticlib::serialization;

using callbacks_map_type = std::map<icu::UnicodeString, std::function<void(const Request&, Response&)>>;

class ServerDeleter {
public:
    void operator()(wilton_Server* server) {
        wilton_Server_stop_server(server);
    }
};

} // namespace

class Server::Impl : public staticlib::pimpl::PimplObject::Impl {
    Logger logger = Logger("wilton.Server");
    callbacks_map_type callbacks;
    std::unique_ptr<wilton_Server, ServerDeleter> ptr;
    
public:
    Impl(const ss::JsonValue& conf, callbacks_map_type callbacks) :
    callbacks(std::move(callbacks)) { 
        std::string conf_str = ss::dump_json_to_string(conf);
        wilton_Server* srv_ptr;
        char* err = wilton_Server_create(std::addressof(srv_ptr), this, Impl::gateway_cb, 
                conf_str.data(), conf_str.length());
        if (nullptr != err) {
            std::string trace = TRACEMSG(err);
            wilton_free(err);
            throw WiltonException(trace);
        }
        this->ptr = std::unique_ptr<wilton_Server, ServerDeleter>(srv_ptr, ServerDeleter());
    }
    
private:

    static void gateway_cb(void* gateway_ctx, wilton_Request* request) STATICLIB_NOEXCEPT {
        Logger logger = Logger("wilton.Server.gateway");
        Server::Impl* self = static_cast<Server::Impl*> (gateway_ctx);
        try {
            self->gateway_cb_internal(request);
        } catch (const std::exception& e) {
            logger.error(UTRACEMSG(icu::UnicodeString::fromUTF8(e.what()) + "\nGateway error"));
        } catch (...) {
            logger.error(UTRACEMSG("Gateway error"));
        }
    }
    
    void gateway_cb_internal(wilton_Request* request) {
        Request req(request);
        Response resp(request);
        auto pa = callbacks.find(req.get_pathname());
        if (callbacks.end() != pa) {
            pa->second(req, resp);
        } else {
            resp.set_status_code(404);
            resp.set_status_message("Not Found");
            resp.send("Path not found: [" + req.get_pathname() + "]");
        }
    }

};
PIMPL_FORWARD_CONSTRUCTOR(Server, (const ss::JsonValue&)(callbacks_map_type), (), WiltonException)

} // namespace
