/***************************************************************************
 *            kafka.hpp
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

#ifndef OPERA_KAFKA_HPP
#define OPERA_KAFKA_HPP

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <tuple>
#include <thread>

#include <librdkafka/rdkafkacpp.h>

namespace Opera{

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

struct stato_ibrido{
/*
- "bodyId": identificativo di corpo [string]
- "discreteState": vettore di valori di variabili discrete (si applica solo al robot e rappresenta lo stato discreto, ossia la modalità operativa) [string]
- "continuousState": vettore di liste di coordinate x-y-z [double] per ognuno dei punti; la posizione nel vettore è l'identificativo del punto; la dimensione di una lista può essere nulla se manca un valore (nel caso acquisizione solo tramite videocamere) o multipla nel caso di sorgenti multiple
- "timestamp": il timestamp [long unsigned int]
*/

    std::string bodyId;         
    std::string discreteState;  
    std::vector<std::list<std::tuple<double, double, double>>> continuousState; 
    unsigned long int timestamp; 

};

struct notifica{
/*
- "humanId": identificativo di corpo umano [string]
- "humanSegmentId": identificativo di segmento dell'umano [unsigned int]
- "robotId": identificativo di corpo robot [string]
- "robotSegmentId": identificativo di segmento del robot [unsigned int]
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


class Consumer{
    public:
        Consumer(
                int partition,
                std::string brokers,
                std::string errstr,
                int start_offset);


        virtual void check_new_message() =0;

        virtual void decoder(std::string str) =0;

        virtual void set_run(bool run_value) =0;

        virtual ~Consumer(){};
    
    protected:
        
        std::string brokers;
        std::string errstr;
        int partition;
        int start_offset;
        bool run;
        std::string topic_string;
        RdKafka::Consumer *consumer;
        RdKafka::Topic *topic;

};

class ConsumerPresentazione : public Consumer{
    public:
        ConsumerPresentazione(
                int partition,
                std::string brokers,
                std::string errstr,
                int start_offset);
        

        void check_new_message();

        void decoder(std::string str);

        void set_run(bool run_val);

        presentazione get_struct();

        ~ConsumerPresentazione(){};
        
    private:
        presentazione _prs;
        
};



class ConsumerStatoIbrido : public Consumer{
    public:
        ConsumerStatoIbrido(
                int partition,
                std::string brokers,
                std::string errstr,
                int start_offset);
        

        void check_new_message();

        void decoder(std::string str);

        void set_run(bool run_val);

        stato_ibrido get_struct();

        ~ConsumerStatoIbrido(){};
        
    private:
        stato_ibrido _si;
        
};

class ConsumerNotifica : public Consumer{
    public:
        ConsumerNotifica(
                int partition,
                std::string brokers,
                std::string errstr,
                int start_offset);
        

        void check_new_message();

        void decoder(std::string str);

        void set_run(bool run_val);

        notifica get_struct();

        ~ConsumerNotifica(){};
        
    private:
        notifica _ntf;       
};

//functions

std::string encoder_presentazione(presentazione * prs);

std::string encoder_stato_ibrido(stato_ibrido * si);

std::string encoder_notifica(notifica * ntf);

presentazione decoder_presentazione(std::string str);

stato_ibrido decoder_stato_ibrido(std::string str);

notifica decoder_notifica(std::string str);

void print_presentazione(presentazione prs);

void print_stato_ibrido(stato_ibrido si);

void print_notifica(notifica ntf);

void consumer_prs_thread(ConsumerPresentazione * o);

void consumer_si_thread(ConsumerStatoIbrido * o);

void consumer_ntf_thread(ConsumerNotifica * o);


RdKafka::Producer * create_producer(std::string brokers);

void send_presentazione(presentazione stuct_to_send, RdKafka::Producer * producer);

void send_stato_ibrido(stato_ibrido stuct_to_send, RdKafka::Producer * producer);

void send_notifica(notifica stuct_to_send, RdKafka::Producer * producer);

}
//template<typename T> extern void test(T stuct_to_send, RdKafka::Producer * producer);
#endif