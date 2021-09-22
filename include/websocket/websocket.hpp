#include "server_ws.hpp"
#include <future>
#include <iostream>
#include <ctype.h>
#include "deserialisation.hpp"
#include <algorithm>

using namespace std;
using namespace Opera;

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

shared_ptr<WsServer::Connection> connections;
std::vector<String> presentations;
std::vector<BodyStatePacket> states;

class websocket {

  public:
    websocket() {}
    start_websocket(int port)
    {
      // WebSocket (WS)-server at port 8080 using 1 thread
      WsServer server;
      server.config.port = port;

      cout<<"Open connection"<<endl;

      // Example 1: echo WebSocket endpoint
      // Added debug messages for example use of the callbacks
      // Test with the following JavaScript:
      //   var ws=new WebSocket("ws://localhost:8080/echo");
      //   ws.onmessage=function(evt){console.log(evt.data);};
      //   ws.send("test");
      auto &echo_presentation = server.endpoint["^/presentation/?$"];
      auto &echo_state = server.endpoint["^/state/?$"];

      echo_presentation.on_message = [](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::InMessage> in_message) {

        //cout << "Server: Presentation received: \"" << in_message->string() << "\" from " << connection.get() << endl;

        presentations.push_back(in_message->string());

        //cout << "Server: Sending message \"" << "presentation received" << "\" to " << connection.get() << endl;

        // connection->send is an asynchronous function
        connection->send("presentation received", [](const SimpleWeb::error_code &ec) {
          if(ec) {
            cout << "Server: Error sending message. " <<
                // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                "Error: " << ec << ", error message: " << ec.message() << endl;
          }
        });
      };

      echo_state.on_message = [](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::InMessage> in_message) {

        //cout << "Server: State received: \"" << in_message->string() << "\" from " << connection.get() << endl;
        
        /*.c_str() put some dirty char at the end of the converted string, &* somehow doesn't have the same problem*/
        states.push_back(BodyStatePacketDeserialiser(&*in_message->string().begin()).make());

        //cout << "Server: Sending message \"" << in_message->string() << "\" to " << connection.get() << endl;

        // connection->send is an asynchronous function
        connection->send(std::to_string(states.size()), [](const SimpleWeb::error_code &ec) {
          if(ec) {
            cout << "Server: Error sending message. " <<
                // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                "Error: " << ec << ", error message: " << ec.message() << endl;
          }
        });
      };

      echo_presentation.on_open = [](shared_ptr<WsServer::Connection> connection) {
        connections = connection;
        cout << "Server: Opened connection " << connection.get() << endl;
      };

      echo_state.on_open = [](shared_ptr<WsServer::Connection> connection) {
        connections = connection;
        cout << "Server: Opened connection " << connection.get() << endl;
      };

      // See RFC 6455 7.4.1. for status codes
      echo_presentation.on_close = [](shared_ptr<WsServer::Connection> connection, int status, const string & /*reason*/) {
        cout << "Server: Closed connection " << connection.get() << " with status code " << status << endl;
        connections = NULL;
      };

      echo_state.on_close = [](shared_ptr<WsServer::Connection> connection, int status, const string & /*reason*/) {
        cout << "Server: Closed connection " << connection.get() << " with status code " << status << endl;
        connections = NULL;
      };

      // Can modify handshake response headers here if needed
      echo_presentation.on_handshake = [](shared_ptr<WsServer::Connection> /*connection*/, SimpleWeb::CaseInsensitiveMultimap & /*response_header*/) {
        return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
      };

      echo_state.on_handshake = [](shared_ptr<WsServer::Connection> /*connection*/, SimpleWeb::CaseInsensitiveMultimap & /*response_header*/) {
        return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
      };

      // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
      echo_presentation.on_error = [](shared_ptr<WsServer::Connection> connection, const SimpleWeb::error_code &ec) {
        cout << "Server: Error in connection " << connection.get() << ". "
            << "Error: " << ec << ", error message: " << ec.message() << endl;

        connections = NULL;
      };

      echo_state.on_error = [](shared_ptr<WsServer::Connection> connection, const SimpleWeb::error_code &ec) {
        cout << "Server: Error in connection " << connection.get() << ". "
            << "Error: " << ec << ", error message: " << ec.message() << endl;

        connections = NULL;
      };

      // Start server and receive assigned port when server is listening for requests
      promise<unsigned short> server_port;
      thread server_thread([&server, &server_port]() {
        // Start server
        server.start([&server_port](unsigned short port) {
          server_port.set_value(port);
        });
      });
      cout << "Server listening on port " << server_port.get_future().get() << endl
          << endl;

      server_thread.join();
    }

    send_notification_message(String out_message) {
      connections->send(out_message, [](const SimpleWeb::error_code &ec) {
          if(ec) {
            cout << "Server: Error sending message. " <<
                // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                "Error: " << ec << ", error message: " << ec.message() << endl;
          }
        });
    }

    std::vector<BodyStatePacket> get_human_states() {
      return states;
    }

};
