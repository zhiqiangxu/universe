#include "Codec/Utf16.h"
#include <string.h>


Endian Utf16::detect_endian(const string& s)
{
    if (s.length() < 2) return Endian::Big;//一个字都不满，随便返回

    uint16_t bom;

    memcpy(&bom, s.data(), sizeof(bom));

    if (bom == 0xFEFF) {
        return Utils::get_endian();
    } else if (bom == 0xFFFE) {
        return Utils::get_opposite_endian();
    }

    // If the BOM is missing, RFC 2781 says that big-endian encoding should be assumed. 
    // (In practice, due to Windows using little-endian order by default, many applications similarly assume little-endian encoding by default.)

    //TODO improve auto detection when no bom
    return Endian::Big;
}

bool Utf16::to_codepoint(uint16_t pair1, uint16_t pair2, uint32_t& result)
{
    if (pair1 >= 0xD800 && pair1 <= 0xDBFF && pair2 >= 0xDC00 && pair2 <= 0xDFFF) {
        auto top_ten = pair1 - 0xD800;
        auto low_ten = pair2 - 0xDC00;
        result = 0x10000 + (top_ten << 10) + low_ten;

        return true;
    }

    return false;
}
