#include "testsqlite.h"
#include "testmpcparser.h"
#include "testdecompressgzip.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    int status = 0;
    TestSqlite testSqlite;
    TestMpcParser testMpcParser;
    TestDecompressGzip testDecompressGzip;

    status |= QTest::qExec(&testSqlite);
    status |= QTest::qExec(&testMpcParser);
    status |= QTest::qExec(&testDecompressGzip);

    return status;
}
