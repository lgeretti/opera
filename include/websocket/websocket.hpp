#include "server_ws.hpp"
#include <future>
#include <iostream>
#include <ctype.h>
#include "deserialisation.hpp"
#include "serialisation.hpp"
#include <algorithm>
#include <mutex>
#include "broker.hpp"

using namespace std;
namespace Opera {

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;

std::vector<BodyPresentationPacket> presentations;
std::vector<BodyStatePacket> states;
std::map<shared_ptr<WsServer::Connection>, std::string> robot_connection;
std::map<std::string, std::vector<std::string>> robot_human_map;
std::mutex mtx;
bool close_server = false;

class websocket : public BrokerInterface {
  public:
    BrokerKind kind() const override;

    void send(BodyPresentationPacket const& p) override;
    void send(BodyStatePacket const& p) override;
    void send(CollisionNotificationPacket const& p) override;

    void receive(std::deque<BodyPresentationPacket>& packets) override;
    void receive(std::deque<BodyStatePacket>& packets) override;
    void receive(std::deque<CollisionNotificationPacket>& packets) override;

    websocket() {}
    start_websocket(int port)
    {

      WsServer server;
      // WebSocket (WS)-server at port 8080 using 1 thread
      server.config.port = port;

      cout<<"Open connection"<<endl;

      // Example 1: echo WebSocket endpoint
      // Added debug messages for example use of the callbacks
      // Test with the following JavaScript:
      //   var ws=new WebSocket("ws://localhost:8080/echo");
      //   ws.onmessage=function(evt){console.log(evt.data);};
      //   ws.send("test");
      auto &opera_endpoint = server.endpoint["^/opera/?$"];


      opera_endpoint.on_message = [](shared_ptr<WsServer::Connection> connection, shared_ptr<WsServer::InMessage> in_message) {

        //cout << "Server: State received: \"" << in_message->string() << "\" from " << connection.get() << endl;
        
        if(in_message->string().find("isHuman") != string::npos)
        {
          BodyPresentationPacket p0 = BodyPresentationPacketDeserialiser(&*in_message->string().begin()).make();
          if (p0.is_human())
          //fare controlli che id robot e human non ci siano già (insomma, metteteli diversi cribbio!)
            robot_connection.insert(std::pair<shared_ptr<WsServer::Connection>,string>(connection,p0.id()));
          else
            robot_human_map[robot_connection[connection]].push_back(p0.id());
          presentations.push_back(p0);
          //cout<<"added presentation"<<endl;
        }
        else if(in_message->string().find("timestamp") != string::npos)
        {
          BodyStatePacket p0 = BodyStatePacketDeserialiser(&*in_message->string().begin()).make();
          states.push_back(p0);
        }
        else
        {
          connection->send("Message type not recognized", [](const SimpleWeb::error_code &ec) {
          if(ec) {
            cout << "Server: Error sending message. " <<
                // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                "Error: " << ec << ", error message: " << ec.message() << endl;
          }
        });
        connection->send_close(-1);
        }
        //cout << "Server: Sending message \"" << in_message->string() << "\" to " << connection.get() << endl;

      };

      opera_endpoint.on_open = [](shared_ptr<WsServer::Connection> connection) {
        cout << "Server: Opened connection " << connection.get() << endl;
      };

      opera_endpoint.on_close = [](shared_ptr<WsServer::Connection> connection, int status, const string & /*reason*/) {
        cout << "Server: Closed connection " << connection.get() << " with status code " << status << endl;
      };

      opera_endpoint.on_handshake = [](shared_ptr<WsServer::Connection> /*connection*/, SimpleWeb::CaseInsensitiveMultimap & /*response_header*/) {
        return SimpleWeb::StatusCode::information_switching_protocols; // Upgrade to websocket
      };

      opera_endpoint.on_error = [](shared_ptr<WsServer::Connection> connection, const SimpleWeb::error_code &ec) {
        cout << "Server: Error in connection " << connection.get() << ". "
            << "Error: " << ec << ", error message: " << ec.message() << endl;
      };
      
      // Start server and receive assigned port when server is listening for requests
        promise<unsigned short> server_port;
        thread server_thread([&server, &server_port]() {
            // Start server
            server.start([&server_port](unsigned short port) {
            server_port.set_value(port);
            });
        });
      cout << "Server listening on port " << server_port.get_future().get() << endl;

      while(!close_server)
        this_thread::sleep_for(chrono::milliseconds(5));

      server.stop();
      server_thread.join();
    }

    void send_notification_message(CollisionNotificationPacket out_message) {
      for ( const auto &p : robot_connection )
      {
        if(p.second == out_message.robot_id())
        {
          CollisionNotificationPacketSerialiser serialiser(out_message);
          p.first->send(serialiser.to_string(), [](const SimpleWeb::error_code &ec) {
            if(ec) {
              cout << "Server: Error sending message. " <<
                  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
                  "Error: " << ec << ", error message: " << ec.message() << endl;
            }
          });
          return;
        }
      }
      
    }

    int num_received_presentations(){
      return presentations.size();
    }

    BodyPresentationPacket get_presentation(int index){
      return presentations[index];
    }

    void stop(){
      close_server = true;
    }


    /*inactivity_remove(string id_robot = "h0")
    {
      mtx.lock();

      std::vector<std::string> temp = robot_human_map[id_robot];
      //delete all the human entries connected with id_robot
      for(int i = 0; i < temp.size(); i++)
      {
        presentations.erase(temp[i]);
        states.erase(temp[i]);
      }
      //delete all the robot entries with id_robot
      presentations.erase(id_robot);
      states.erase(id_robot);

      //close the connection (just in case) and remove the entry
      for ( const auto &p : robot_connection )
        if(p.second == id_robot)
        {
          p.first->send_close(-1);
          robot_connection.erase(p.first);
          break;
        }

      mtx.unlock();
    }*/

};
}
