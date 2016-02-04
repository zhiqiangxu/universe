
{% for field in fields %}

auto ret = {{ parse_field(field) }}
switch (ret)
{
	case ParseResult::AGAIN:
		need_buf(client, message, true);
		return ParseResult::AGAIN;
	case ParseResult::OK:
		break;
	case ParseResult::NG:
		erase_buf(client);
		return ParseResult::NG;

}

{% endfor %}

return ParseResult::OK;

