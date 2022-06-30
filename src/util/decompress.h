#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <QByteArray>

int decompressGzip(QByteArray &out, const QByteArray &in);

#endif // DECOMPRESS_H
