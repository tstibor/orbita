#include "testsqlite.h"
#include "testmpcparser.h"

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    int status = 0;
    TestSqlite testSqlite;
    TestMpcParser testMpcParser;

    status |= QTest::qExec(&testSqlite);
    status |= QTest::qExec(&testMpcParser);

    return status;
}
