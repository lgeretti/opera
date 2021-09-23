/***************************************************************************
 *            kafka.cpp
 *
 *  Copyright  2021  Andrea Gagliardo, Sandro Ferrari, Luca Geretti
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

#include "kafka.hpp"



namespace Opera{

Consumer::Consumer( int partition, 
                std::string brokers,
                std::string errstr,
                int start_offset):
                partition(partition),
                brokers(brokers),
                errstr(errstr),
                start_offset(start_offset),
                run(true)
{};

ConsumerPresentation::ConsumerPresentation(int partition, std::string brokers, std::string errstr, int start_offset)
    : Consumer(partition, brokers, errstr, start_offset)
  {

    topic_string = "opera-presentation";
        
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    /*
     * Create consumer using accumulated global configuration.
     */
    consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) {
      std::cerr << "Failed to create consumer: " << errstr << std::endl;
      exit(1);
    }

    std::cout << "% Created consumer " << consumer->name() << std::endl;

    /*
     * Create topic handle.
     */
    topic = RdKafka::Topic::create(consumer, topic_string, tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }

    /*
     * Start consumer for topic+partition at start offset
     */
    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);  
    if (resp != RdKafka::ERR_NO_ERROR) {
      std::cerr << "Failed to start consumer: " <<
	    RdKafka::err2str(resp) << std::endl;
      exit(1);
    }

  }

  ConsumerPresentation::~ConsumerPresentation(){
    consumer->stop(topic,0);
    //delete consumer;
    //delete topic;
  }
  
  void ConsumerPresentation::check_new_message(){
    while (run) {          
      RdKafka::Message *msg = consumer->consume(topic, partition, 1000);   
      if(msg->err() == RdKafka::ERR_NO_ERROR){
          std::string prs_str = static_cast<const char *> (msg->payload());
          _prs_str_list.push_back(prs_str);
      }
      else if(msg->err() == RdKafka::ERR__TIMED_OUT){} 
      else{
          std::cerr << "Consume failed: " << msg->errstr() << std::endl;      
      }

      delete msg;
      consumer->poll(0);  // interroga l'handler degli eventi di Kafka 

    }
  }

  void ConsumerPresentation::set_run(bool run_val){
    run = run_val;
  }

  BodyPresentationPacket ConsumerPresentation::get_pkt(){
    std::string prs_str = _prs_str_list.front();
    _prs_str_list.pop_front();
    BodyPresentationPacketDeserialiser d(prs_str.c_str());
    return d.make();    
  }

  int ConsumerPresentation::number_new_msgs(){
    return _prs_str_list.size();
  }

;

// CONSUMER STATE--------------------------------------------------------------------

ConsumerState::ConsumerState(
                int partition,
                std::string brokers,
                std::string errstr,
                int start_offset):
  Consumer(partition, brokers, errstr, start_offset)
  {
    topic_string = "opera-state";
        
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    /*
     * Create consumer using accumulated global configuration.
     */
    consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) {
      std::cerr << "Failed to create consumer: " << errstr << std::endl;
      exit(1);
    }

    std::cout << "% Created consumer " << consumer->name() << std::endl;

    /*
     * Create topic handle.
     */
    topic = RdKafka::Topic::create(consumer, topic_string, tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }

    /*
     * Start consumer for topic+partition at start offset
     */

    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);  
    if (resp != RdKafka::ERR_NO_ERROR) {
      std::cerr << "Failed to start consumer: " <<
	RdKafka::err2str(resp) << std::endl;
      exit(1);
    }
  }

  ConsumerState::~ConsumerState(){
    consumer->stop(topic,0);
    //delete consumer;
    //delete topic;
  }
  
  void ConsumerState::check_new_message(){

    while (run) {          
      RdKafka::Message *msg = consumer->consume(topic, partition, 1000);   
      if(msg->err() == RdKafka::ERR_NO_ERROR){
          std::string st_str = static_cast<const char *> (msg->payload());
          _st_str_list.push_back(st_str);
      }
      else if(msg->err() == RdKafka::ERR__TIMED_OUT){} 
      else{
          std::cerr << "Consume failed: " << msg->errstr() << std::endl;      
      }

      delete msg;
      consumer->poll(0);  // interroga l'handler degli eventi di Kafka 

    }
  }

  void ConsumerState::set_run(bool run_val){
    run = run_val;
  }

  BodyStatePacket ConsumerState::get_pkt(){
    std::string st_str = _st_str_list.front();
    _st_str_list.pop_front();
    BodyStatePacketDeserialiser d(st_str.c_str());
    return d.make();
  }

  int ConsumerState::number_new_msgs(){
    return _st_str_list.size();
  }
;

//CONSUMER NOTIFICA ----------------

ConsumerCollisionNotification::ConsumerCollisionNotification(
                int partition,
                std::string brokers,
                std::string errstr,
                int start_offset):
  Consumer(partition, brokers, errstr, start_offset)
  {
    topic_string = "opera-collision-notification";
        
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    /*
     * Create consumer using accumulated global configuration.
     */
    consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) {
      std::cerr << "Failed to create consumer: " << errstr << std::endl;
      exit(1);
    }

    std::cout << "% Created consumer " << consumer->name() << std::endl;

    /*
     * Create topic handle.
     */
    topic = RdKafka::Topic::create(consumer, topic_string, tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }

    /*
     * Start consumer for topic+partition at start offset
     */

    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);  
    if (resp != RdKafka::ERR_NO_ERROR) {
      std::cerr << "Failed to start consumer: " <<
	RdKafka::err2str(resp) << std::endl;
      exit(1);
    }
  }

  ConsumerCollisionNotification::~ConsumerCollisionNotification(){
    consumer->stop(topic,0);
    //delete consumer;
    //delete topic;
  }
  
  void ConsumerCollisionNotification::check_new_message(){
    while (run) {          
      RdKafka::Message *msg = consumer->consume(topic, partition, 1000);
      if(msg->err() == RdKafka::ERR_NO_ERROR){
          //decoder(static_cast<const char *> (msg->payload())); // asign to prs the relative data from the msg
          std:: string ntf_str = static_cast<const char *> (msg->payload());
          _ntf_str_list.push_back(ntf_str);        
      }
      else if(msg->err() == RdKafka::ERR__TIMED_OUT){}  //do notthing
      else{
          std::cerr << "Consume failed: " << msg->errstr() << std::endl;      
      }

      delete msg;
      consumer->poll(0);  // interroga l'handler degli eventi di Kafka
    }
  }

  void ConsumerCollisionNotification::set_run(bool run_val){
    run = run_val;
  }

  CollisionNotificationPacket ConsumerCollisionNotification::get_pkt(){
    std::string ntf_str = _ntf_str_list.front();
    _ntf_str_list.pop_front();
    CollisionNotificationPacketDeserialiser d(ntf_str.c_str());
    return d.make();
  }

  int ConsumerCollisionNotification::number_new_msgs(){
    return _ntf_str_list.size();
  }

//=========================================================================
// PRODUCER FUNCTIONS
//=========================================================================
RdKafka::Producer * create_producer(std::string brokers){
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    std::string errstr;
    conf->set("metadata.broker.list", brokers, errstr);
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
      std::cerr << "Failed to create producer: " << errstr << std::endl;
      exit(1);
    }
    return producer;
}

void send_presentation(BodyPresentationPacket p , RdKafka::Producer * producer){
    BodyPresentationPacketSerialiser serialiser(p);
    std::string msg_to_send = serialiser.to_string();

	producer->produce(OPERA_PRESENTATION_TOPIC, 0,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

void send_state(BodyStatePacket p, RdKafka::Producer * producer){
	BodyStatePacketSerialiser serialiser(p);
    std::string msg_to_send = serialiser.to_string();
	
	producer->produce(OPERA_STATE_TOPIC, 0,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

void send_collision_notification(CollisionNotificationPacket p, RdKafka::Producer * producer){
    CollisionNotificationPacketSerialiser serialiser(p);
    std::string msg_to_send = serialiser.to_string();

	producer->produce(OPERA_COLLISION_NOTIFICATION, 0,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

}