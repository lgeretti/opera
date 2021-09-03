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

ConsumerPresentazione::ConsumerPresentazione(
                int partition = 0,
                std::string brokers = "localhost:9092",
                std::string errstr = "",
                int start_offset = 0):
  Consumer(partition, brokers, errstr, start_offset)
  {
    //topic_string = "opera.presentazione";

    std::cout<<"\n\n\nSONO IL COSTRUTTOREEEEEEEEE\n\n\n";

    topic_string = "quickstart-events";
        
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
  
  void ConsumerPresentazione::check_new_message(){

    std::cout<<"AYEYEYE";
    while (run) {          
      RdKafka::Message *msg = consumer->consume(topic, partition, 1000);   
      if(msg->err() == RdKafka::ERR_NO_ERROR){
          decoder(static_cast<const char *> (msg->payload())); // asign to prs the relative data from the msg
      }
      else if(msg->err() == RdKafka::ERR__TIMED_OUT){}  //do nothing
      else{
          std::cerr << "Consume failed: " << msg->errstr() << std::endl;      
      }

      delete msg;
      consumer->poll(0);  // interroga l'handler degli eventi di Kafka 

    }
  }

  void ConsumerPresentazione::decoder(std::string str){
      
      _prs = decoder_presentazione(str);
  }

  void ConsumerPresentazione::set_run(bool run_val){
    run = run_val;
  }

  presentazione ConsumerPresentazione::get_struct(){
    return _prs;
  }
;

// CONSUMER STATO IBRIDOOO--------------------------------------------------------------------

ConsumerStatoIbrido::ConsumerStatoIbrido(
                int partition = 0,
                std::string brokers = "localhost:9092",
                std::string errstr = "",
                int start_offset = 0):
  Consumer(partition, brokers, errstr, start_offset)
  {
    //topic_string = "opera.statoibrido";

    std::cout<<"\n\n\nSONO IL COSTRUTTOREEEEEEEEE\n\n\n";

    topic_string = "quickstart-events";
        
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
  
  void ConsumerStatoIbrido::check_new_message(){

    std::cout<<"AYEYEYE";
    while (run) {          
      RdKafka::Message *msg = consumer->consume(topic, partition, 1000);   
      if(msg->err() == RdKafka::ERR_NO_ERROR){
          decoder(static_cast<const char *> (msg->payload())); // asign to prs the relative data from the msg
      }
      else if(msg->err() == RdKafka::ERR__TIMED_OUT){}  //do notthing
      else{
          std::cerr << "Consume failed: " << msg->errstr() << std::endl;      
      }

      delete msg;
      consumer->poll(0);  // interroga l'handler degli eventi di Kafka 

    }
  }

  void ConsumerStatoIbrido::decoder(std::string str){
      
      _si = decoder_stato_ibrido(str);
  }

  void ConsumerStatoIbrido::set_run(bool run_val){
    run = run_val;
  }

  stato_ibrido ConsumerStatoIbrido::get_struct(){
    return _si;
  }
;

//CONSUMER NOTIFICA ----------------

ConsumerNotifica::ConsumerNotifica(
                int partition = 0,
                std::string brokers = "localhost:9092",
                std::string errstr = "",
                int start_offset = 0):
  Consumer(partition, brokers, errstr, start_offset)
  {
    //topic_string = "opera.statoibrido";

    std::cout<<"\n\n\nSONO IL COSTRUTTOREEEEEEEEE\n\n\n";

    topic_string = "quickstart-events";
        
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
  
  void ConsumerNotifica::check_new_message(){

    std::cout<<"AYEYEYE";
    while (run) {          
      RdKafka::Message *msg = consumer->consume(topic, partition, 1000);
      if(msg->err() == RdKafka::ERR_NO_ERROR){
          decoder(static_cast<const char *> (msg->payload())); // asign to prs the relative data from the msg
      }
      else if(msg->err() == RdKafka::ERR__TIMED_OUT){}  //do notthing
      else{
          std::cerr << "Consume failed: " << msg->errstr() << std::endl;      
      }

      delete msg;
      consumer->poll(0);  // interroga l'handler degli eventi di Kafka 

    }
  }

  void ConsumerNotifica::decoder(std::string str){
      
      _ntf = decoder_notifica(str);
  }

  void ConsumerNotifica::set_run(bool run_val){
    run = run_val;
  }

  notifica ConsumerNotifica::get_struct(){
    return _ntf;
  }
;

//===========================END CLASSES=================================



std::string encoder_presentazione(presentazione * prs){
  std::string str;
  str.append((prs->id));
  str.append(";");
  str.append(std::to_string(prs->type));
  str.append(";");
  str.append(std::to_string(prs->sampleFrequency));
  str.append(";");

  std::vector<std::pair<unsigned int, unsigned int>> pointlds = prs->pointlds;
  for(int i = 0; i<pointlds.size(); i++){
    unsigned int n1 = pointlds[i].first;
    unsigned int n2 = pointlds[i].second;
    str.append(std::to_string(n1));
    str.append(" ");
    str.append(std::to_string(n2));
    str.append(" ");
  }
  str.append(";");
  std::vector<double> thicknesses = prs->thicknesses;
  for(int i = 0; i<thicknesses.size(); i++){
    double n1 = thicknesses[i];
    str.append(std::to_string(n1));
    str.append(" ");
  }
  str.append(";");

  return str;
}

std::string encoder_stato_ibrido(stato_ibrido * si){
    std::string str;

    str.append((si->bodyId));
    str.append(";");
    str.append((si->discreteState));
    str.append(";");
    
    for(int i=0; i<si->continuousState.size(); i++){
        for (std::tuple<double, double, double> j : si->continuousState[i]) {
            str.append(std::to_string(std::get<0>(j))); // x
            str.append(" ");
            str.append(std::to_string(std::get<1>(j))); // y
            str.append(" ");
            str.append(std::to_string(std::get<2>(j))); // z
            str.append(" ");
        }
        str.append("£");
    }

    str.append(";");
    str.append(std::to_string(si->timestamp));
    str.append(";");

    return str;
}

std::string encoder_notifica(notifica * ntf){
  std::string str;
  str.append((ntf->humanId));
  str.append(";");
  str.append(std::to_string((ntf->humanSegmentId)));
  str.append(";");
  str.append((ntf->robotId));
  str.append(";");
  str.append(std::to_string((ntf->robotSegmentId)));
  str.append(";");
  str.append((ntf->discreteState));
  str.append(";");
  str.append(std::to_string((ntf->minimumCollisionTime)));
  str.append(";");
  return str;
}


presentazione decoder_presentazione(std::string str){
  
  presentazione prs;
  int fieldNumber = 0;  // utilizzato per sapere a che campo siamo 


  std::string delimiter = ";";
  int del_dim = delimiter.length(); // dimensione delimitatore
  int pos_delim;
  std::string substring;

  std::string delim_pair = " ";
  int del_pair_dim = delim_pair.length();
  int pos_delim_pair;
  std::string pairstring;
  bool first = true;
  unsigned int numberPair;
  int n1, n2;


  std::string delim_thick = " ";
  int del_thick_dim = delim_thick.length();
  int pos_delim_thick;
  double numberthick;
  int idx_thick = 0;

  //questo while gira fino a quando il .find non ritorna npos (ovvero quando non trova nessun ";")
  while(str.find(delimiter) != std::string::npos){
    pos_delim = str.find(delimiter);
    //std::cout<<"Iterazione numero"<<fieldNumber<<"\n";    DEBUG
    //std::cout<<"Pos_delim = "<<pos_delim<<"\n";           DEBUG
    
    substring = str.substr(0, pos_delim);   // prendo la sottostringa fino a ";"
    //std::cout<<"Substring:"<<substring<< "\n";  //      DEBUG
    switch (fieldNumber)
    {
    case 0:
      /* id string*/
      prs.id = substring;
      break;

    case 1:
      /* type bool */
      prs.type = substring == "1";  // se substring è 1 ritorna true, altrimenti false
      break;

    case 2:
      /* sampleFrequency unsigned int */
      
      prs.sampleFrequency = std::stoul(substring);  //(string to unsigned long stoul)
      break;

    case 3:
      /* pointlds vector<pair<uns int, uns int>> */
        pos_delim_pair = substring.find(delim_pair);
        while(pos_delim_pair  != std::string::npos ){

          //std::cout<<"\n----- substring di pair "<<substring.substr(0,pos_delim_pair)<<"------";  //DEBUG
          //std::cout<<"\n pos_delim_pair: "<< pos_delim_pair;                                      //DEBUG
          numberPair = std::stoul(substring.substr(0,pos_delim_pair)); // prendo il numero (string to unsigned long stoul)  ..;10 20 20 30
          if(first){
            n1 = numberPair;// se first = True vuol dire che è il primo numero della coppia
            
          }
          else{
            n2 = numberPair;      // se first = False vuol dire che è il secondo numero della coppia
            prs.pointlds.push_back(std::make_pair(n1,n2));   
          }
          substring.erase(0, pos_delim_pair + del_pair_dim);    
          first = not first; // switcho tra primo e secondo numero della coppia ogni volta
          pos_delim_pair = substring.find(delim_pair);
        }
      break;

    case 4:
      /* thikness array<double> */
      pos_delim_thick = substring.find(delim_thick);
      while(pos_delim_thick != std::string::npos ){
          numberthick = std::stod(substring.substr(0,pos_delim_thick)); // prendo il numero (string to double stod)

          prs.thicknesses.push_back(numberthick);
          substring.erase(0, pos_delim_thick + del_thick_dim);    
          pos_delim_thick = substring.find(delim_thick);
        }
      break;

    default:
      break;
    }

    fieldNumber++;
    str.erase(0,pos_delim + del_dim);       // rimuovo la prima sottostringa
  }
  std::cout<<"\n\n----Decode ended with success----\n\n";
  return prs;
}

stato_ibrido decoder_stato_ibrido(std::string str){
  stato_ibrido si;
  si.continuousState.clear();

  std::string delimiter = ";";
  int del_dim = delimiter.length(); // dimensione delimitatore
  int pos_delim;
  int fieldNumber = 0;  // utilizzato per sapere a che campo siamo 
  std::string substring;

  std::string delim_coordinate = " ";
  int del_coordinate_dim = delim_coordinate.length();
  int pos_delim_coordinate;
  std::list<std::tuple<double, double, double>> lista_tupla;
  std::string coordinate0String;
  std::string coordinate1String;
  std::string coordinate2String;

  std::string delim_list = "£";
  int del_list_dim = delim_list.length();
  int pos_delim_list;
  double numberlist;
  std::string list_substring;
  //questo while gira fino a quando il .find non ritorna npos (ovvero quando non trova nessun ";")
  while(str.find(delimiter) != std::string::npos){
    pos_delim = str.find(delimiter);
    
    substring = str.substr(0, pos_delim);   // prendo la sottostringa fino a ";"
    std::cout<<"Substring:"<<substring<< "\n";
    switch (fieldNumber)
    {
    case 0:
      /* bodyId string*/
      si.bodyId = substring;
      break;

    case 1:
      /* type string */
      si.discreteState = substring;
      break;

    case 2:
      /* continuousState vector<list<double, double, double>> */
      pos_delim_list = substring.find(delim_list);
      while(pos_delim_list  != std::string::npos ){
        list_substring = substring.substr(0, pos_delim_list);
        pos_delim_coordinate = list_substring.find(delim_coordinate);

        while(pos_delim_coordinate  != std::string::npos ){

          std::cout<<"List Substring:"<<list_substring<< "\n";

          coordinate0String = list_substring.substr(0,pos_delim_coordinate); // prendo il numero (string to unsigned long stoul)
          list_substring.erase(0, pos_delim_coordinate + del_coordinate_dim);
          pos_delim_coordinate = list_substring.find(delim_coordinate);

          std::cout<<list_substring<<"\n";

          coordinate1String = list_substring.substr(0,pos_delim_coordinate);
          list_substring.erase(0, pos_delim_coordinate + del_coordinate_dim);
          pos_delim_coordinate = list_substring.find(delim_coordinate);

          std::cout<<"2!\n";
          std::cout<<list_substring<<"\n";

          coordinate2String = list_substring.substr(0,pos_delim_coordinate);
          list_substring.erase(0, pos_delim_coordinate + del_coordinate_dim);

          std::cout<<"3!\n";
          std::cout<<list_substring<<"\n";
          
          std::cout<<coordinate0String<<"\n";
          std::cout<<coordinate1String<<"\n";
          std::cout<<coordinate2String<<"\n";

          lista_tupla.push_back(std::make_tuple(std::stod(coordinate0String),std::stod(coordinate1String),std::stod(coordinate2String)));
          
          pos_delim_coordinate = list_substring.find(delim_coordinate);

          std::cout<<"4!\n";
        }
        si.continuousState.push_back(lista_tupla);

        lista_tupla.clear();
        substring.erase(0, pos_delim_list + del_list_dim);

        pos_delim_list = substring.find(delim_list);
      }
      break;

    case 3:
      /* timestamp unsigned long int */
      si.timestamp = std::stoul(substring);
      break;

    default:
      break;
    }

    fieldNumber++;
    str.erase(0,pos_delim + del_dim);       // rimuovo la prima sottostringa
  }
  std::cout<<"\n\n----Decode ended with success----\n\n";
  return si;
}

notifica decoder_notifica(std::string str){
  notifica ntf;
  int fieldNumber=0;

  std::string delim = ";";
  int dim_delim = delim.length();
  int pos_delim;
  std::string substring;

  pos_delim = str.find(delim);
  while(pos_delim != std::string::npos){
    substring = str.substr(0, pos_delim);
    std::cout<<"\n\n"<<"Substring in decode: "<< substring << "\nfieldNumber:"<<fieldNumber <<"\n\n";

    switch (fieldNumber)
    {
    case 0:
      /* string humanId */
      ntf.humanId = substring;
      break;
    
    case 1:
      /* unsigned int humanSegmentId */
      ntf.humanSegmentId = stoul(substring);
      break;

    case 2:
      /* string robotId */
      ntf.robotId = substring;
      break;
    
    case 3:
      /* unsigned int robotSegmentId */
      ntf.robotSegmentId = stoul(substring);
      break;

    case 4:
      /* string discreteState */
      ntf.discreteState = substring;
      break;

    case 5:
      /* long unsigned int minimumCollisionTime */
      ntf.minimumCollisionTime = stoul(substring);
      break;
    }
    fieldNumber ++;
    str = str.erase(0, pos_delim + dim_delim);
    pos_delim = str.find(delim);
  }
  std::cout<<"\n\n----Decode ended with success----\n\n";
  return ntf;
};


void print_presentazione(presentazione prs){
  std::cout<<"Id: "<<prs.id<<"\n";
  std::cout<<"Human: ";
  if(prs.type){
    std::cout<<"TRUE\n";
  }
  else{
    std::cout<<"FALSE\n";
  }
  std::cout<<"Sample frequency: "<<prs.sampleFrequency<<"\n";
  std::cout<<"Link's joints pair list with relative thikness:\n";
  for(int i = 0; i< prs.pointlds.size(); i++){
    std::cout<<"["<<prs.pointlds[i].first<<", "<< prs.pointlds[i].second<<"] - "<<prs.thicknesses[i]<<"\n";
  }
}

void print_stato_ibrido(stato_ibrido si){
  std::cout<<"bodyId: "<<si.bodyId<<"\n";
  std::cout<<"discreteState: "<<si.discreteState<<"\n";
  
  std::cout<<"continuousState:\n";
  for(int i = 0; i< si.continuousState.size(); i++){
    std::cout<<"list "<<i<<":\n";
    for (std::tuple<double, double, double> j : si.continuousState[i]) {
      std::cout<<"["<<std::get<0>(j)<<", "<<std::get<1>(j)<<", "<<std::get<2>(j)<<"]\n";
    }
  }
  std::cout<<"timestamp: "<<si.timestamp<<"\n";  
}

void print_notifica(notifica ntf){
  std::cout<<"Human Id:" <<ntf.humanId<<"\n";
  std::cout<<"Human segment Id:" <<ntf.humanSegmentId<<"\n";
  std::cout<<"Robot Id:" <<ntf.robotId<<"\n";
  std::cout<<"Robot segment Id:" <<ntf.robotSegmentId<<"\n";
  std::cout<<"Discrete State variables of robot:" <<ntf.discreteState<<"\n";
  std::cout<<"Minimum collision time:" <<ntf.minimumCollisionTime<<"\n";
}


void consumer_prs_thread(ConsumerPresentazione * o){
    o->check_new_message();
}

void consumer_si_thread(ConsumerStatoIbrido * o){
    o->check_new_message();
}

void consumer_ntf_thread(ConsumerNotifica * o){
    o->check_new_message();
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

void send_presentazione(presentazione stuct_to_send, RdKafka::Producer * producer){
	std::string msg_to_send = encoder_presentazione(& stuct_to_send);
	std::string topic_name = "quickstart-events";
	int partition = 0;

	producer->produce(topic_name, partition,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

void send_stato_ibrido(stato_ibrido stuct_to_send, RdKafka::Producer * producer){
	std::string msg_to_send = encoder_stato_ibrido(& stuct_to_send);
	std::string topic_name = "quickstart-events";
	int partition = 0;
	
	producer->produce(topic_name, partition,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}

void send_notifica(notifica stuct_to_send, RdKafka::Producer * producer){
	std::string msg_to_send = encoder_notifica(& stuct_to_send);
	std::string topic_name = "quickstart-events";
	int partition = 0;

	producer->produce(topic_name, partition,
						RdKafka::Producer::RK_MSG_COPY,
						const_cast<char *>(msg_to_send.c_str()),
						msg_to_send.size(),NULL, 0, 0, NULL);
}
}