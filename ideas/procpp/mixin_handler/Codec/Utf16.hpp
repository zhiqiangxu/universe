
template <Endian endian>
size_t Utf16::length(const string& s)
{
    if (s.length() < 2) return 0;
    
    size_t n = 0;

    uint16_t two_bytes;
    uint16_t high_surrogate;
    uint16_t low_surrogate;
    bool need_low_surrogate = false;

    memcpy(&two_bytes, s.data(), sizeof(two_bytes));

    auto same_endian = endian == Utils::get_endian();

    if (two_bytes == 0xFEFF || two_bytes == 0xFFFE) {
    } else {
        if (!same_endian) Utils::swap_endian(two_bytes);

        if (two_bytes <= 0xD7FF || two_bytes >= 0xE000) {
            //U+0000 to U+D7FF and U+E000 to U+FFFF
            n += 1;
        } else {
            //U+D800 to U+DFFF
            // high surrogate should be in range 0xD800..0xDBFF
            if (two_bytes > 0xDBFF) return 0;

            high_surrogate = two_bytes;
            need_low_surrogate = true;
        }
    }

    size_t offset = 2;

    while (offset + 2 < s.length()) {
        memcpy(&two_bytes, s.data() + offset, sizeof(two_bytes));

        if (!same_endian) Utils::swap_endian(two_bytes);

        if (need_low_surrogate) {
            // low surrogate should be in range 0xDC00..0xDFFF.
            if (two_bytes >= 0xDC00 && two_bytes <= 0xDFFF) {
                n += 1;
                need_low_surrogate = false;
            } else return n;
        } else {
            if (two_bytes <= 0xD7FF || two_bytes >= 0xE000) {
                //U+0000 to U+D7FF and U+E000 to U+FFFF
                n += 1;
            } else {
                //U+D800 to U+DFFF
                // high surrogate should be in range 0xD800..0xDBFF
                if (two_bytes > 0xDBFF) return n;

                high_surrogate = two_bytes;
                need_low_surrogate = true;
            }
        }

        offset += 2;
    }

    return n;

}

template <Endian endian>
size_t Utf16::decode_char_length(const string& s, size_t& offset)
{
    size_t additional = 0;

    auto same_endian = endian == Utils::get_endian();

NEXT:
    if (offset + 2 < s.length()) {
        uint16_t two_bytes;
        memcpy(&two_bytes, s.data(), sizeof(two_bytes));

        if (!same_endian) Utils::swap_endian(two_bytes);

        if (offset == 0) {
            if (two_bytes == 0xFEFF || two_bytes == 0xFFFE) {
                offset += 2;
                additional = 2;
                goto NEXT;
            }
        }

        if (two_bytes <= 0xD7FF || two_bytes >= 0xE000) return 2 + additional;


        return 4 + additional;
    }

    return 0;
}

template <Endian endian>
bool Utf16::decode_char(const string& s, string& ch, size_t& offset)
{
    auto length = decode_char_length<endian>(s, offset);

    if (length == 0) return false;

    ch.assign(&s[offset], length);
    offset += length;

    return true;
}

