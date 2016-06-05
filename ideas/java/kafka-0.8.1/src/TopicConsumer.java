import kafka.consumer.Consumer;
import kafka.consumer.ConsumerConfig;
import kafka.consumer.KafkaStream;
import kafka.javaapi.consumer.ConsumerConnector;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import java.lang.reflect.InvocationTargetException;
import kafka.utils.ZkUtils;

import java.util.concurrent.BlockingQueue;

public class TopicConsumer {
    private ConsumerConnector consumer;
    private String topic;
    private  ExecutorService executor;

    public TopicConsumer(String a_zookeeper, String a_groupId, String a_topic) {

        this.consumer = Consumer.createJavaConsumerConnector(createConsumerConfig(a_zookeeper, a_groupId));

        this.topic = a_topic;
    }

    public int run(int nStreams, Class<? extends Runnable> c, BlockingQueue queue) /* throws NoSuchMethodException, InstantiationException, IllegalAccessException, InvocationTargetException*/ {

        try {

            Map<String, Integer> topicCountMap = new HashMap<String, Integer>();
            topicCountMap.put(this.topic, new Integer(nStreams));


            Map<String, List<KafkaStream<byte[], byte[]>>> consumerMap = this.consumer.createMessageStreams(topicCountMap);

            List<KafkaStream<byte[], byte[]>> streams = consumerMap.get(this.topic);

            this.executor = Executors.newFixedThreadPool(streams.size());

            int threadNumber = 0;
            for (KafkaStream stream : streams) {
                this.executor.submit( c.getConstructor(KafkaStream.class, int.class, BlockingQueue.class).newInstance(stream, threadNumber, queue) );
                threadNumber++;
            }

            return threadNumber;

        } catch (Exception e) {
            // TODO deal with exception

            System.out.println("exception");
            System.out.println(e.getMessage());

            return 0;
        }
    }

    private static ConsumerConfig createConsumerConfig(String a_zookeeper, String a_groupId) {
        Properties props = new Properties();
        props.put("zookeeper.connect", a_zookeeper);
        props.put("group.id", a_groupId);
        props.put("zookeeper.session.timeout.ms", "5000");
        props.put("zookeeper.connection.timeout.ms", "10000");
        props.put("rebalance.backoff.ms", "2000");
        props.put("rebalance.max.retries", "10");

        props.put("zookeeper.sync.time.ms", "2000");
        props.put("auto.commit.interval.ms", "1000");

        //以下为debug，上线必须注释掉
        ZkUtils.maybeDeletePath(a_zookeeper, "/consumers/" + a_groupId);
        props.put("auto.offset.reset", "smallest");

        return new ConsumerConfig(props);
    }

}
