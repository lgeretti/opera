#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <tuple>

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

struct statoIbrido{
    std::string bodyId;         // identificativo di corpo
    std::string discreteState;  // vettore di valori di variabili discrete (si applica solo al robot e rappresenta lo stato discreto, ossia la modalità operativa)
    std::vector<std::list<std::tuple<double, double, double>>> continuosState; // vettore di liste di coordinate x-y-z [double] per ognuno dei punti; la posizione nel vettore è l'identificativo del punto; la dimensione di una lista può essere nulla se manca un valore (nel caso acquisizione solo tramite videocamere) o multipla nel caso di sorgenti multiple
    unsigned long int timestamp; // il timestamp [long unsigned int]

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

statoIbrido decoderStatoIbrido(std::string str){
  statoIbrido si;
  si.continuosState.clear();

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
    // std::cout<<"Iterazione numero"<<fieldNumber<<"\n";  // debug !!  
    // std::cout<<"Pos_delim = "<<pos_delim<<"\n";         // debug !!
    
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
      /* continuosState vector<list<double, double, double>> */
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
        si.continuosState.push_back(lista_tupla);

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


void printStatoIbrido(statoIbrido si){
  std::cout<<"bodyId: "<<si.bodyId<<"\n";
  std::cout<<"discreteState: "<<si.discreteState<<"\n";
  
  std::cout<<"continuosState:\n";
  for(int i = 0; i< si.continuosState.size(); i++){
    std::cout<<"list "<<i<<":\n";
    for (std::tuple<double, double, double> j : si.continuosState[i]) {
      std::cout<<"["<<std::get<0>(j)<<", "<<std::get<1>(j)<<", "<<std::get<2>(j)<<"]\n";
    }
  }
  std::cout<<"timestamp: "<<si.timestamp<<"\n";  
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