#include <zlib.h>
#include "decompress.h"
#include "logger.h"

#define GZIP_ZLIB_WINDOWSIZE 15

int decompressGzip(QByteArray &out, const QByteArray &in)
{
    int rc;
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    rc = inflateInit2(&zs, GZIP_ZLIB_WINDOWSIZE + 16);
    if (rc != Z_OK) {
	qWarning() << "inflateInit2 failed";
	return rc;
    }

    zs.next_in = (Bytef*)in.data();
    zs.avail_in = in.size();

    char outbuffer[32768];

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        rc = inflate(&zs, 0);

        if (out.size() < zs.total_out)
            out.append(outbuffer, zs.total_out - out.size());

    } while (rc == Z_OK);

    inflateEnd(&zs);

    if (rc != Z_STREAM_END) {
	qWarning() << "error during decompression";
	return Z_ERRNO;
    }

    return Z_OK;
}
