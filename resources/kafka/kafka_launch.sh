#!/bin/bash
echo Launching kafka test system
pwd
cd
cd "$(find ~ -path "*opera/resources/kafka")"

#terminator -e "cd ~ && zookeeper-server-start.sh  && sleep 10"
gnome-terminal -e 'sh -c "zookeeper-server-start.sh zookeeper.properties"'
sleep 2
gnome-terminal -e 'sh -c "kafka-server-start.sh server.properties"'
echo Started server and broker
echo Creating the 3 topics
sleep 3
gnome-terminal -e "kafka-topics.sh --create --topic opera-presentation --bootstrap-server localhost:9092"
gnome-terminal -e "kafka-topics.sh --create --topic opera-state --bootstrap-server localhost:9092"
gnome-terminal -e "kafka-topics.sh --create --topic opera-collision-notification --bootstrap-server localhost:9092"
sleep 0.5
echo Topic created:
bin/kafka-topics.sh --list --bootstrap-server localhost:9092