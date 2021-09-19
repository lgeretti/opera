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

#include "client_ws.hpp"
#include <future>

using namespace std;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;

shared_ptr<WsClient::Connection> connections;

int main() {
  WsClient client("localhost:8080/presentation");
  client.on_message = [](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::InMessage> in_message) {
    cout << "Client: Message received: \"" << in_message->string() << "\"" << endl;

    cout << "Client: Sending close connection" << endl;
    connection->send_close(1000);
  };

  client.on_open = [](shared_ptr<WsClient::Connection> connection) {
    cout << "Client: Opened connection" << endl;

    auto out_message = "{\n"
                        "  \"id\": \"h0\",\n"
                        "  \"isHuman\": true,\n"
                        "  \"pointIds\": [[14,12],[11,12],[5,7],[6,8]],\n"
                        "  \"thicknesses\": [1,1,1,1]\n"
                        "}";

    cout << "Client: Sending presentation message: \"" << out_message << "\"" << endl;

    connection->send(out_message);
  };

  client.on_close = [](shared_ptr<WsClient::Connection> /*connection*/, int status, const string & /*reason*/) {
    cout << "Client: Closed connection with status code " << status << endl;
  };

  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  client.on_error = [](shared_ptr<WsClient::Connection> /*connection*/, const SimpleWeb::error_code &ec) {
    cout << "Client: Error: " << ec << ", error message: " << ec.message() << endl;
  };

  client.start();

  WsClient client_state("localhost:8080/state");
  client.on_message = [](shared_ptr<WsClient::Connection> connection, shared_ptr<WsClient::InMessage> in_message) {
    cout << "Client: Message received: \"" << in_message->string() << "\"" << endl;

    cout << "Client: Sending close connection" << endl;
    connection->send_close(1000);
  };

  client_state.on_open = [](shared_ptr<WsClient::Connection> connection) {
    cout << "Client: Opened connection" << endl;

    auto out_message = "{\n"
                        "  \"id\": \"h0\",\n"
                        "  \"isHuman\": true,\n"
                        "  \"pointIds\": [[14,12],[11,12],[5,7],[6,8]],\n"
                        "  \"thicknesses\": [1,1,1,1]\n"
                        "}";

    cout << "Client: Sending presentation message: \"" << out_message << "\"" << endl;

    connection->send(out_message);
  };

  client_state.on_close = [](shared_ptr<WsClient::Connection> /*connection*/, int status, const string & /*reason*/) {
    cout << "Client: Closed connection with status code " << status << endl;
  };

  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  client_state.on_error = [](shared_ptr<WsClient::Connection> /*connection*/, const SimpleWeb::error_code &ec) {
    cout << "Client: Error: " << ec << ", error message: " << ec.message() << endl;
  };

  client_state.start();

  return 0;
}
