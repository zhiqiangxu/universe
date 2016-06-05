import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.KeeperException;

import org.json.JSONObject;
import org.json.JSONException;

import java.util.List;
import java.util.ArrayList;
import java.io.IOException;

public class BrokerInfoFetcher {
    private String a_zookeeper;

    public BrokerInfoFetcher (String a_zookeeper) {

        this.a_zookeeper = a_zookeeper;

    }

    public List<String> getBrokerList() {

        List<String> brokerList = new ArrayList<String>();

        try {
            ZooKeeper zk = new ZooKeeper(this.a_zookeeper, 10000, null);
            List<String> ids = zk.getChildren("/brokers/ids", false);
            for (String id : ids) {
                String brokerInfoString = new String(zk.getData("/brokers/ids/" + id, false, null));

                JSONObject brokerInfo = Utils.jsonDecode(brokerInfoString);

                brokerList.add( brokerInfo.getString("host") + ":" + Integer.toString(brokerInfo.getInt("port")) );
            }
        } catch(Exception e) {

            System.out.println("exception");
            System.out.println(e.getMessage());
        }

        return brokerList;

    }

    public int getPartitionCount(String topic) {

        try {
            ZooKeeper zk = new ZooKeeper(this.a_zookeeper, 10000, null);
            List<String> ids = zk.getChildren("/brokers/topics/" + topic + "/partitions", false);

            return ids.size();

        } catch(Exception e) {

            System.out.println("exception");
            System.out.println(e.getMessage());
        }

        return 0;
    }

    public static void main(String[] args) throws IOException, JSONException, KeeperException, InterruptedException {
        BrokerInfoFetcher fetcher = new BrokerInfoFetcher("127.0.0.1:2181");

        List<String> brokerList = fetcher.getBrokerList();

        System.out.println("partition count of kafka: " + Integer.toString(fetcher.getPartitionCount("kafka")));

        System.out.println(Utils.join(",", brokerList));
    }
}
