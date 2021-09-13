#!/bin/bash
cd ~/kafka_2.13-2.8.0
bin/kafka-topics.sh --delete --topic opera-presentation --bootstrap-server localhost:9092
bin/kafka-topics.sh --delete --topic opera-state --bootstrap-server localhost:9092
bin/kafka-topics.sh --delete --topic opera-collision-notification --bootstrap-server localhost:9092