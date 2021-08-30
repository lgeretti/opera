#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include <librdkafka/rdkafkacpp.h>


struct presentazione{
  std::string id; // "body"|"id"
  bool type; // "body"|"type" o human? 0->bot, 1->human
  unsigned int sampleFrequency;
  std::vector<std::pair<unsigned int, unsigned int>> pointlds;
  std::vector<double> thicknesses;
};

struct notifica{
  /*
- "human"|"id": identificativo di corpo umano [string]
- "human"|"segmentId": identificativo di segmento dell'umano [unsigned int]
- "robot"|"id": identificativo di corpo robot [string]
- "robot"|"segmentId": identificativo di segmento del robot [unsigned int]
- "discreteState": vettore di valori di variabili discrete rappresentanti lo stato discreto del robot [string]
- "minimumCollisionTime": istante minimo di collisione [long unsigned int]
  */
  std::string humanId;
  unsigned int humanSegmentId;
  std::string robotId;
  unsigned int robotSegmentId;
  std::string discreteState;
  long unsigned int minimumCollisionTime;

};

void printNotifica(notifica ntf){
  std::cout<<"Human Id:" <<ntf.humanId<<"\n";
  std::cout<<"Human segment Id:" <<ntf.humanSegmentId<<"\n";
  std::cout<<"Robot Id:" <<ntf.robotId<<"\n";
  std::cout<<"Robot segment Id:" <<ntf.robotSegmentId<<"\n";
  std::cout<<"Discrete State variables of robot:" <<ntf.discreteState<<"\n";
  std::cout<<"Minimum collision time:" <<ntf.minimumCollisionTime<<"\n";
}

presentazione decoderPresentazione(std::string str){
  
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

notifica decoderNotifica(std::string str){
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

void printPresentazione(presentazione prs){
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

static volatile sig_atomic_t run = 1;
static bool exit_eof = false;


void msg_consume(RdKafka::Message* message, void* opaque) {
  const RdKafka::Headers *headers;
  presentazione prs;
  notifica ntf;
  
  switch (message->err()) {
    case RdKafka::ERR__TIMED_OUT:
      break;

    case RdKafka::ERR_NO_ERROR:
      /* Real message */
      //std::cout << "Read msg at offset " << message->offset() << std::endl;
      if (message->key()) {
        std::cout << "Key: " << *message->key() << std::endl;
      }
       printf("%s\n", static_cast<const char *>(message->payload()));

       //prs = decoderPresentazione(static_cast<const char *> (message->payload()));  //test decoder  
       //printPresentazione(prs);
       ntf = decoderNotifica(static_cast<const char *> (message->payload()));
       printNotifica(ntf);

      break;

    case RdKafka::ERR__PARTITION_EOF:
      /* Last message */
      if (exit_eof) {
        run = 0;
      }
      break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      run = 0;
      break;

    default:
      /* Errors */
      std::cerr << "Consume failed: " << message->errstr() << std::endl;
      run = 0;
  }
}

int main(){
    std::string brokers = "localhost:9092";
    std::string errstr;
    std::string topic_str = "quickstart-events";
    int32_t partition = 0; 
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    bool do_conf_dump = false;
    int opt;
    int use_ccb = 0;

  /*
   * Create configuration objects
   */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    conf->set("metadata.broker.list", brokers, errstr);

    /*
     * Create consumer using accumulated global configuration.
     */
    RdKafka::Consumer *consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) {
      std::cerr << "Failed to create consumer: " << errstr << std::endl;
      exit(1);
    }

    std::cout << "% Created consumer " << consumer->name() << std::endl;

    /*
     * Create topic handle.
     */
    RdKafka::Topic *topic = RdKafka::Topic::create(consumer, topic_str,
						   tconf, errstr);
    if (!topic) {
      std::cerr << "Failed to create topic: " << errstr << std::endl;
      exit(1);
    }

    /*
     * Start consumer for topic+partition at start offset
     */

    
    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);  // !! start del consumer
    if (resp != RdKafka::ERR_NO_ERROR) {
      std::cerr << "Failed to start consumer: " <<
	RdKafka::err2str(resp) << std::endl;
      exit(1);
    }

    //ExampleConsumeCb ex_consume_cb;

    /*
     * Consume messages
     */
    while (run) {             
 
        RdKafka::Message *msg = consumer->consume(topic, partition, 1000);   // !! qua prende il messaggio
        msg_consume(msg, NULL);
        delete msg;
        consumer->poll(0);  // interroga l'handler degli eventi di Kafka 
    }

    /*
     * Stop consumer
     */
    consumer->stop(topic, partition);

    consumer->poll(1000);

    delete topic;
    delete consumer;
    }