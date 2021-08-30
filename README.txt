-used kafka_2.13-2.8.0
--the topic name for the moment is quickstaart-events, future works will see different 	topic for different messages....

-there are 2 file cpp: Producer and Consumer

function in Producer:
	-void print<nameStruct>() 			it prints out the structure data
	-string encode<nameStruct>(nameStruct var)	encode the struct into string

function in Consumer:
	-void print<nameStruct>() 			it prints out the structure data
	-nameStruct decode<nameStruct>(string str)	gets the string and return the corresponding struct

to run the examples:

(in the kafka directory)
#START THE ZOOKEEPER SERVICE
bin/zookeeper-server-start.sh config/zookeeper.properties 

#START THE KAFKA BROKER SERVICE
bin/kafka-server-start.sh config/server.properties 

#CREATE TOPIC named "quickstart-events" on localhost:9092
bin/kafka-topics.sh --create --topic quickstart-events --bootstrap-server localhost:9092


