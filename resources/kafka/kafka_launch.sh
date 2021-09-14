#!/bin/bash
cd "$(sudo find -name "kafka_2.13-2.8.0" 2>/dev/null)"
terminator -e "./bin/zookeeper-server-start.sh config/zookeeper.properties" -p hold.
sleep 2
terminator -e "./bin/kafka-server-start.sh config/server.properties" -p hold.
sleep 2
terminator -e "./bin/kafka-topics.sh --create --topic opera-presentation --bootstrap-server localhost:9092"
terminator -e "./bin/kafka-topics.sh --create --topic opera-state --bootstrap-server localhost:9092"
terminator -e "./bin/kafka-topics.sh --create --topic opera-collision-notification --bootstrap-server localhost:9092"
sleep 0.5
echo Topic created:
bin/kafka-topics.sh --list --bootstrap-server localhost:9092