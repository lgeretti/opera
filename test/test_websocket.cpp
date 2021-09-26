/***************************************************************************
 *            test_websocket.cpp
 *
 *  Copyright  2021  Luca Geretti
 *
 ****************************************************************************/

/*
 *  This file is part of Opera.
 *
 *  Opera is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Opera is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Opera.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "test.hpp"
#include "serialisation.hpp"
#include "deserialisation.hpp"
#include "packet.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include "websocket.hpp"
#include "client_ws.hpp"

using namespace Opera;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;
List<BodyPresentationPacket> pr;
List<BodyStatePacket> ps;
int received = 0;
int ind_pres = 0;
int ind_state = 0;

class WebsocketTest{
public: 
websocket webSocket;
    void test(){
        //ARIADNE_TEST_CALL(test_presentation())
    }


};

int main() {
    int id = fork();
    //forka il client, nel main fai il test
    if(id == 0){
        //WebsocketTest().test();
        WsClient client("localhost:8080/opera");
            client.on_message = [](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::InMessage> in_message) {
                received ++;
                if(received == 2)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(800));
                    connection->send_close(10);
                }
            };
            
            client.on_open = [](shared_ptr<WsClient::Connection> connection) {

                BodyPresentationPacketSerialiser serialiser(pr.at(0));
                connection->send(serialiser.to_string());
                BodyPresentationPacketSerialiser serialiser2(pr.at(1));
                connection->send(serialiser2.to_string());
                BodyStatePacketSerialiser serialiser3(ps.at(0));
                connection->send(serialiser3.to_string());
                BodyStatePacketSerialiser serialiser4(ps.at(1));
                connection->send(serialiser4.to_string());
            };

            client.on_close = [](shared_ptr<WsClient::Connection> /*connection*/, int status, const string & /*reason*/) {
            };

            // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
            client.on_error = [&](shared_ptr<WsClient::Connection> /*connection*/, const SimpleWeb::error_code &ec) {
            };


        pr.append(BodyPresentationPacket("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)}));
        pr.append(BodyPresentationPacket("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)}));
        ps.append(BodyStatePacket("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290));
        ps.append(BodyStatePacket("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230));
        
        std::thread websocket_client_thread([&]{
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            client.start();
        });
        websocket_client_thread.join();
        exit(0);
    }
    else {
        
        WsServer server;
        server.config.port = 8080;

        pr.append(BodyPresentationPacket("human1", {{0, 1},{3, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp)}));
        pr.append(BodyPresentationPacket("robot1", 30, {{0, 1},{3, 2},{4, 2}}, {FloatType(1.0, Ariadne::dp),FloatType(0.5, Ariadne::dp), FloatType(0.5, Ariadne::dp)}));
        ps.append(BodyStatePacket("human0",{{Point(0.4,2.1,0.2)},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{Point(0.4,0.1,1.2)},{Point(0,0,1)}},3423235253290));
        ps.append(BodyStatePacket("robot0",DiscreteLocation({{"origin","3"},{"destination","2"},{"phase","pre"}}),{{},{Point(0,-1,0.1),Point(0.3,3.1,-1.2)},{}},93249042230));
        
        // Example 1: echo WebSocket endpoint
        // Added debug messages for example use of the callbacks
        // Test with the following JavaScript:
        //   var ws=new WebSocket("ws://localhost:8080/echo");
        //   ws.onmessage=function(evt){console.log(evt.data);};
        //   ws.send("test");
        auto &echo = server.endpoint["^/opera/?$"];

        echo.on_message = [](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::InMessage> in_message) {
            //auto out_message = in_message->string();
            
            if(in_message->string().find("isHuman") != string::npos)
            {
                BodyPresentationPacket p_received = BodyPresentationPacketDeserialiser(&*in_message->string().begin()).make();
                ARIADNE_TEST_EQUAL(p_received.id(), pr[ind_pres].id())
                ARIADNE_TEST_EQUAL(p_received.is_human(), pr[ind_pres].is_human())
                ARIADNE_TEST_EQUAL(p_received.packet_frequency(), pr[ind_pres].packet_frequency())
                for(int i = 0; i<pr[ind_pres].point_ids().size(); i++){
                    ARIADNE_TEST_EQUAL(p_received.point_ids()[i].first, pr[ind_pres].point_ids()[i].first)
                    ARIADNE_TEST_EQUAL(p_received.point_ids()[i].second, pr[ind_pres].point_ids()[i].second)
                }
                for(int i = 0; i<pr[ind_pres].thicknesses().size(); i++){
                    ARIADNE_TEST_EQUAL(p_received.thicknesses()[i], pr[ind_pres].thicknesses()[i])
                }
                ind_pres++;
            }
            else if(in_message->string().find("timestamp") != string::npos)
            {
                BodyStatePacket p_received = BodyStatePacketDeserialiser(&*in_message->string().begin()).make();
                ARIADNE_TEST_EQUAL(p_received.id(), ps[ind_state].id())
                ARIADNE_TEST_EQUAL(p_received.location(), ps[ind_state].location())
                ARIADNE_TEST_EQUAL(p_received.timestamp(), ps[ind_state].timestamp())
                for(int j = 0; j<p_received.points().size(); j++){
                    ARIADNE_TEST_EQUAL(p_received.points().at(j), ps[ind_state].points().at(j))
                }
                ind_state++;
            }
            else{
            }

            connection->send("received");
            // Alternatively use streams:
            // auto out_message = make_shared<WsServer::OutMessage>();
            // *out_message << in_message->string();
            // connection->send(out_message);
        };

        echo.on_open = [](shared_ptr<WsServer::Connection> connection) {
        };

        // See RFC 6455 7.4.1. for status codes
        echo.on_close = [](shared_ptr<WsServer::Connection> connection, int status, const string & /*reason*/) {
            close_server = true;
        };

        // Can modify handshake response headers here if needed
        echo.on_handshake = [](shared_ptr<WsServer::Connection> /*connection*/, SimpleWeb::CaseInsensitiveMultimap & /*response_header*/) {
            return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
        };

        // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
        echo.on_error = [](shared_ptr<WsServer::Connection> connection, const SimpleWeb::error_code &ec) {
        };

        // Start server and receive assigned port when server is listening for requests
        promise<unsigned short> server_port;
        thread server_thread([&server, &server_port]() {
            // Start server
            server.start([&server_port](unsigned short port) {
            server_port.set_value(port);
            });
        });

        while(!close_server)
            this_thread::sleep_for(chrono::milliseconds(5));

        server.stop();
        server_thread.join();

        return ARIADNE_TEST_FAILURES;
    }
}