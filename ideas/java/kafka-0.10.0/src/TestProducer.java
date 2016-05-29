import java.util.*;
import java.util.concurrent.ExecutionException;

import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.common.serialization.StringSerializer;


public class TestProducer {
    public static void main(String[] args) throws InterruptedException, ExecutionException {
        long events = Long.parseLong(args[0]);
        Random rnd = new Random();

        Properties props = new Properties();
        props.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, "localhost:9092 ");
        props.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        props.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
        //props.put("partitioner.class", "SimplePartitioner");
        //props.put("request.required.acks", "1");

        KafkaProducer<String,String> producer = new KafkaProducer<String,String>(props);

        boolean sync = false;
        String topic = "page_visits";
        for (long nEvents = 0; nEvents < events; nEvents++) {
            long runtime = new Date().getTime();
            String ip = "192.168.2." + rnd.nextInt(255);
            String msg = runtime + ",www.example.com," + ip;

            ProducerRecord<String,String> producerRecord = new ProducerRecord<String,String>(topic, ip, msg);

            if (sync) {
                producer.send(producerRecord).get();
            } else {
                producer.send(producerRecord);
            }
        }
        producer.close();
    }
}
