import kafka.consumer.KafkaStream;
import kafka.consumer.ConsumerIterator;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Date;

import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.BlockingQueue;


public class TopicAggregator {
    private TopicConsumer consumer;
    private BlockingQueue queue;

    public TopicAggregator(String a_zookeeper, String a_groupId, String a_topic) {

        this.consumer = new TopicConsumer(a_zookeeper, a_groupId, a_topic);

        this.queue = new LinkedBlockingQueue<ConsumerResult>(100);
    }

    public void run() throws Exception {

        int threadCount = this.consumer.run(1, ConsumerLogic.class, this.queue);

        TopicProducer<String, String> producer = new TopicProducer<String, String>("127.0.0.1:2181", "kafka_producer");

        while (true) {
            try {
                ConsumerResult result = (ConsumerResult)this.queue.take();
                System.out.println("got result from thread");

                JSONObject json = new JSONObject();
                json.put("key", "value");
                producer.sendMessage("sharding_key", json.toString());

            } catch (InterruptedException e) {
                System.out.println("InterruptedException in aggregator");
            }
        }

    }

    public static void main(String[] args) throws Exception  {
        TopicAggregator agg = new TopicAggregator("127.0.0.1:2181", "test", "kafka");

        agg.run();

    }
}

class ConsumerResult {
}

class ConsumerLogic implements Runnable {
    private KafkaStream stream;
    private int index;
    private BlockingQueue queue;

    public ConsumerLogic(KafkaStream stream, int index, BlockingQueue queue) {
        this.stream = stream;
        this.index = index;
        this.queue = queue;
    }

    public void run() {

        ConsumerIterator<byte[], byte[]> it = this.stream.iterator();
        while (it.hasNext()) {
            String jsonText = new String(it.next().message());
            System.out.println("Thread " + index + ": " + jsonText);

            try {

                JSONObject json = Utils.jsonDecode(jsonText);
                System.out.println(json.getString("flags"));

                Object timeObject = json.get("time");
                Integer timeInteger = Utils.to_integer(timeObject);

                if (timeInteger == null) continue;

                System.out.println(timeInteger);
                Date date = Utils.to_date(timeInteger);
                System.out.println(date);

                this.queue.put(new ConsumerResult());

            } catch (JSONException e) {
                System.out.println("invalid json: " + jsonText);
            } catch (InterruptedException e) {
                System.out.println("InterruptedException in consumer");
            }
        }

        System.out.println("Shutting down Thread: " + index);
    }
}


