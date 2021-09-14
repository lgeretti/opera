#!/bin/bash
echo Launching kafka test system
pwd
cd
cd "$(find ~ -path "*opera/resources/kafka")"

terminator -e "source ~/.bashrc && zookeeper-server-start.sh zookeeper.properties | sleep 10" -p hold
sleep 2
terminator -e "source ~/.bashrc && kafka-server-start.sh server.properties | sleep 10" -p hold
echo Started server and broker
echo Creating the 3 topics
sleep 2
terminator -e "kafka-topics.sh --create --topic opera-presentation --bootstrap-server localhost:9092"
terminator -e "kafka-topics.sh --create --topic opera-state --bootstrap-server localhost:9092"
terminator -e "kafka-topics.sh --create --topic opera-collision-notification --bootstrap-server localhost:9092"
sleep 0.5
echo Topic created:
bin/kafka-topics.sh --list --bootstrap-server localhost:9092