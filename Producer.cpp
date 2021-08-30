
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <tuple>

#include <librdkafka/rdkafkacpp.h>

struct presentazione{
  /*
  - "id": identificativo di corpo (prescinde che sia un robot oppure un umano) [string]
- "type": flag di tipologia di corpo (robot oppure umano) [bool]
- "sampleFrequency": frequenza in Hz dell'invio periodico di pacchetti di stato [unsigned int]
- "pointIds": vettore di coppie di identificativi numerici [unsigned int] che rappresentano i punti (testa e coda) di ognuno dei segmenti; la posizione nel vettore rappresenta l'identificativo di segmento
- "thicknesses": vettore degli spessori [double] per ogni segmento
*/
  std::string id; 
  bool type; 
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

void printNotifica(notifica ntf){
  std::cout<<"Human Id:" <<ntf.humanId<<"\n";
  std::cout<<"Human segment Id:" <<ntf.humanSegmentId<<"\n";
  std::cout<<"Robot Id:" <<ntf.robotId<<"\n";
  std::cout<<"Robot segment Id:" <<ntf.robotSegmentId<<"\n";
  std::cout<<"Discrete State variables of robot:" <<ntf.discreteState<<"\n";
  std::cout<<"Minimum collision time:" <<ntf.minimumCollisionTime<<"\n";
}

std::string encoderNotifica(notifica * ntf){
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

std::string encoderPresentazione(presentazione * prs){
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

std::string encoderStatoIbrido(statoIbrido * si){
    std::string str;

    str.append((si->bodyId));
    str.append(";");
    str.append((si->discreteState));
    str.append(";");
    
    for(int i=0; i<si->continuosState.size(); i++){
        for (std::tuple<double, double, double> j : si->continuosState[i]) {
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


static volatile sig_atomic_t run = 1;

int main(){

    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
  

	  std::string brokers = "localhost:9092";
    std::string errstr;
    std::string topic_str = "quickstart-events";
    std::string mode;
    std::string debug;
    int32_t partition = 0; 
    int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
    bool do_conf_dump = false;
    int opt;
    int use_ccb = 0;

    conf->set("metadata.broker.list", brokers, errstr);


    /*
     * Create producer using accumulated global configuration.
     */
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);  // !!
    if (!producer) {
      std::cerr << "Failed to create producer: " << errstr << std::endl;
      exit(1);
    }


    presentazione prs;
    prs.id = "ID_del_body";
    prs.type = true;
    prs.pointlds.clear();
    prs.pointlds.push_back(std::make_pair(10,10));
    prs.pointlds.push_back(std::make_pair(20,20));
    prs.sampleFrequency = 500;
    prs.thicknesses.clear();
    prs.thicknesses.push_back(12.12);
    prs.thicknesses.push_back(2.15);

    //printPresentazione(prs);

    notifica ntf;
    ntf.humanId = "idHuman";
    ntf.humanSegmentId = 0010;
    ntf.robotId = "idRobot";
    ntf.robotSegmentId = 0020;
    ntf.discreteState = "discrete state string";
    ntf.minimumCollisionTime = 99;

    printNotifica(ntf);

    //std::string nostroMsg = encoderPresentazione(&prs);
    std::string nostroMsg = encoderNotifica(&ntf);


    std::cout << "% Created producer " << producer->name() << std::endl;

  
    /*
    * Read messages from stdin and produce to broker.
    */
    for (std::string line; run && std::getline(std::cin, line);) {    //legge da tastiera e mette in line
      if (line.empty()) {
        producer->poll(0);
	      continue;
      }

      /*
       * Produce message
       */
      
      RdKafka::ErrorCode resp =
        producer->produce(topic_str, partition,
                          RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                          /* Value */
                          //const_cast<char *>(line.c_str()), 
                          const_cast<char *>(nostroMsg.c_str()),
                          nostroMsg.size(),    // !! line contains the msg that will be sent
                          /* Key */
                          NULL, 0,
                          /* Timestamp (defaults to now) */
                          0,
                          /* Message headers, if any */
                          //headers,
                          /* Per-message opaque value passed to
                           * delivery report */
                          NULL);
      if (resp != RdKafka::ERR_NO_ERROR) {
        std::cerr << "% Produce failed: " <<
          RdKafka::err2str(resp) << std::endl; 
      } else {
        std::cerr << "% Produced message (" << line.size() << " bytes)" <<
          std::endl;
      }

      producer->poll(0);
    }
    run = 1;

    while (run && producer->outq_len() > 0) {
      std::cerr << "Waiting for " << producer->outq_len() << std::endl;
      producer->poll(1000);
    }

    delete producer;
}