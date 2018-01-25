//#include "header.h"
#include "codeconverter.h"
#include <iostream>
#include <QFile>
#include <QDebug>

using namespace std;

int main (int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage:\n\t%s FILENAME\n"
               "(if file without extension \".pas\", \".cpp\" then you get an error)");

        return -1;
    }

    CodeConverter cc;
    bool inverted;
    QString name (argv[1]);
    QFile in (name);
    QFile out;
    in.open (QFile::ReadOnly);

    if (name.endsWith (".pas", Qt::CaseInsensitive)) {
        out.setFileName (name.chopped (4) + ".cpp");
        inverted = false;
    } else if (name.endsWith (".cpp", Qt::CaseInsensitive)) {
        out.setFileName (name.chopped (4) + ".pas");
        inverted = true;
    } else  {
        printf ("Only .pas or .cpp files applies");
    }

    out.open (QFile::WriteOnly);

    QByteArray ba = in.readAll();
    QString data (ba);

    QString outData = cc.parse (data, inverted ? "C++" : "PASCAL", inverted ? "PASCAL" : "C++");
    QByteArray b = outData.toUtf8 ();
    out.write (b, b.length ());

    in.close ();
    out.close ();

    return 0;
}
