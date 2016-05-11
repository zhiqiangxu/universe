template <>
string Soa::packet<Soa::packet_type::JSON, string>(const GUID& request_id, const string& json)
{
	return SoaRequest::to_string(request_id, json);
}



