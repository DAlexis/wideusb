/*
 * Based on http://web.mit.edu/freebsd/head/contrib/wpa/src/utils/base64.c
 * Original file copyright:
 *
 * Base64 encoding/decoding (RFC1341)
 * Copyright (c) 2005-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "wideusb-common/utils/base64.hpp"

#include "string.h"


static const unsigned char base64_table[65] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::optional<std::string> base64_encode(const std::vector<uint8_t> src)
{
    return base64_encode(src.data(), src.size());
}

/**
 * base64_encode - Base64 encode
 * @src: Data to be encoded
 * @len: Length of the data to be encoded
 * Returns: string of encoded data
 */
std::optional<std::string> base64_encode(const uint8_t *src, size_t len)
{
    std::string result;
    char *out, *pos;
    const uint8_t *end, *in;
	size_t olen;
	int line_len;

	olen = len * 4 / 3 + 4; /* 3-byte blocks to 4-byte */
	olen += olen / 72; /* line feeds */


	olen++; /* nul termination */
	if (olen < len)
        return std::nullopt; /* integer overflow */

    result.resize(olen-1);

    out = const_cast<char*>( result.c_str() ); // No member functions will be called so const_cast is ok

	end = src + len;
	in = src;
	pos = out;
	line_len = 0;
	while (end - in >= 3) {
		*pos++ = base64_table[in[0] >> 2];
		*pos++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
		*pos++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
		*pos++ = base64_table[in[2] & 0x3f];
		in += 3;
		line_len += 4;
		if (line_len >= 72) {
			*pos++ = '\n';
			line_len = 0;
		}
	}

	if (end - in) {
		*pos++ = base64_table[in[0] >> 2];
		if (end - in == 1) {
			*pos++ = base64_table[(in[0] & 0x03) << 4];
			*pos++ = '=';
		} else {
			*pos++ = base64_table[((in[0] & 0x03) << 4) |
					      (in[1] >> 4)];
			*pos++ = base64_table[(in[1] & 0x0f) << 2];
		}
		*pos++ = '=';
		line_len += 4;
	}

    /*if (line_len)
        *pos++ = '\n';*/

	*pos = '\0';
    result.resize(pos - out);

    return result;
}


/**
 * base64_decode - Base64 decode
 * @src_string: String to be decoded
 * Returns: Allocated vector of decoded data,
 *
 */
std::optional<std::vector<uint8_t>> base64_decode(const std::string& src_string)
{
    const unsigned char * src = reinterpret_cast<const unsigned char *> (src_string.c_str());
    size_t len = src_string.size();
    std::vector<uint8_t> result;

    uint8_t dtable[256];
    uint8_t *out, *pos;
    char block[4], tmp;
	size_t i, count, olen;
	int pad = 0;

    memset(dtable, 0x80, 256);
	for (i = 0; i < sizeof(base64_table) - 1; i++)
		dtable[base64_table[i]] = (unsigned char) i;
	dtable['='] = 0;

	count = 0;
	for (i = 0; i < len; i++) {
		if (dtable[src[i]] != 0x80)
			count++;
	}

    if (count == 0 || count % 4)
    {
        return std::nullopt;
    }

	olen = count / 4 * 3;
    result.resize(olen);
    pos = out = result.data();

    if (out == NULL)
    {
        return std::nullopt;
    }

	count = 0;
	for (i = 0; i < len; i++) {
		tmp = dtable[src[i]];
		if (tmp == 0x80)
			continue;

		if (src[i] == '=')
			pad++;
		block[count] = tmp;
		count++;
		if (count == 4) {
			*pos++ = (block[0] << 2) | (block[1] >> 4);
			*pos++ = (block[1] << 4) | (block[2] >> 2);
			*pos++ = (block[2] << 6) | block[3];
			count = 0;
			if (pad) {
				if (pad == 1)
					pos--;
				else if (pad == 2)
					pos -= 2;
				else {
					/* Invalid padding */
                    //delete[] out;
                    return std::nullopt;
				}
				break;
			}
		}
	}
    result.resize(pos - out);
    return result;
}
