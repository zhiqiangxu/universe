import org.json.JSONException;
import org.json.JSONObject;

import java.util.List;
import java.util.Date;

public class Utils {
    static JSONObject jsonDecode(String jsonString) throws JSONException {
        return new JSONObject(jsonString);
    }

    static String join(String delimiter, List<String> l) {
        StringBuilder sb = new StringBuilder();

        boolean first = true;
        for (String item : l) {
            if ( !first ) {
                sb.append(delimiter);
            }

            sb.append(item);

            if ( first ) first = false;
        }

        return sb.toString();
    }

    static Integer to_integer(Object numeric) {
        try {
            if (numeric instanceof String) {
                return Integer.valueOf((String) numeric);
            }

            return (Integer) numeric;
        }
        catch (Exception e) {

            return null;

        }
    }

    static Date to_date(int timestamp) {
        return new Date( ((long)timestamp) * 1000 );
    }
}
