#ifndef QSTDOUT
#define QSTDOUT
#include <QTextStream>

QTextStream& qStdOut()
{
    //non debug Stream
    static QTextStream ts( stdout );
    return ts;
}

#endif // QSTDOUT

