
{% for field in fields %}

auto ret = {{ field.on_message() }};
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

