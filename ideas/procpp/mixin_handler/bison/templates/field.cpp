{% if field.type %}
[&message, this] () {
    if ( (_parsed_length + {{ calc_builtin_type_size(field.type) }} * {{ calc_field_n(field) }} ) > message.length()) return ParseResult::AGAIN;

    return 
} ()
{% endif %}
