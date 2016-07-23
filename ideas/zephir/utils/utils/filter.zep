namespace Utils;

class Filter
{

    protected adapter;
    public function alpha(string str) -> string
    {

        char ch;
        string filtered = "";

        for ch in str {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                let filtered .= ch;
            }
        }

        return filtered;

    }
}



