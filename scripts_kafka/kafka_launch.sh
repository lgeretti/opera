#!/bin/bash
cd ~/kafka_2.13-2.8.0
gnome-terminal -- bin/zookeeper-server-start.sh config/zookeeper.properties
sleep 2
gnome-terminal -- bin/kafka-server-start.sh config/server.properties
sleep 2
gnome-terminal -- bin/kafka-topics.sh --create --topic opera-presentation --bootstrap-server localhost:9092
gnome-terminal -- bin/kafka-topics.sh --create --topic opera-state --bootstrap-server localhost:9092
gnome-terminal -- bin/kafka-topics.sh --create --topic opera-collision-notification --bootstrap-server localhost:9092
sleep 0.5
echo Topic created:
bin/kafka-topics.sh --list --bootstrap-server localhost:9092