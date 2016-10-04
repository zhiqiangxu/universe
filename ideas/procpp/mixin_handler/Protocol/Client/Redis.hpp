
template <>
string Redis::packet<Redis::packet_type::GET, string>(const GUID& request_id, const string& key)
{
    return create_cmd("GET", key);
}

template <>
string Redis::packet<Redis::packet_type::SET, string, string, int*, int*, NXXX*>(
    const GUID& request_id,
    const string& key,
    const string& value,
    int* const& p_ex,// FYI http://stackoverflow.com/a/16151115
    int* const& p_px,
    NXXX* const& p_nxxx
)
{
    string element_expire;
    if (p_px) {
        element_expire = "PX " + to_string(*p_px);
    } else if (p_ex) {
        element_expire = "EX " + to_string(*p_ex);
    }

    string element_nxxx;
    if (p_nxxx) element_nxxx = Utils::enum_string(*p_nxxx);

    string cmd_string;
    if (element_expire.length()) {
        if (element_nxxx.length()) {
            cmd_string = create_cmd("SET", key, value, element_expire, element_nxxx);
        } else {
            cmd_string = create_cmd("SET", key, value, element_expire);
        }
    } else {
        if (element_nxxx.length()) {
            cmd_string = create_cmd("SET", key, value, element_nxxx);
        } else {
            cmd_string = create_cmd("SET", key, value);
        }
    }

    return cmd_string;
}





