#!/bin/bash
echo Deleting topics...
kafka-topics.sh --delete --topic opera-presentation --bootstrap-server localhost:9092
kafka-topics.sh --delete --topic opera-state --bootstrap-server localhost:9092
kafka-topics.sh --delete --topic opera-collision-notification --bootstrap-server localhost:9092
echo Closing server and zookeeper...
sleep 3
kafka-server-stop.sh
sleep 3
zookeeper-server-stop.sh