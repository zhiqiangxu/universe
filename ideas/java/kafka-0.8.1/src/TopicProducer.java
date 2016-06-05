
import java.util.List;
import java.util.Properties;

import kafka.producer.ProducerConfig;
import kafka.producer.KeyedMessage;
import kafka.javaapi.producer.Producer;

public class TopicProducer<T1, T2> {

    private String topic;
    private Producer<T1, T2> producer;

    public TopicProducer(String a_zookeeper, String a_topic) throws Exception {

        this.producer = new Producer<T1, T2>(createProducerConfig(a_zookeeper));

        this.topic = a_topic;
    }

    public void sendMessage(T1 key, T2 value) {

        KeyedMessage<T1, T2> data = new KeyedMessage<T1, T2>(this.topic, key, value);

        this.producer.send(data);

    }

    private static ProducerConfig createProducerConfig(String a_zookeeper) throws Exception {
        Properties props = new Properties();

        BrokerInfoFetcher fetcher = new BrokerInfoFetcher(a_zookeeper);
        List<String> brokerList = fetcher.getBrokerList();
        if (brokerList.size() == 0) {
            throw new Exception("brokerList empty");
        }

        props.put("metadata.broker.list", Utils.join(",", brokerList));
        props.put("serializer.class", "kafka.serializer.StringEncoder");
        props.put("request.required.acks", "1");


        return new ProducerConfig(props);
    }
}
