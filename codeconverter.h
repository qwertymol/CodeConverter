#ifndef CODECONVERTER_H
#define CODECONVERTER_H

#include <QString>

class CodeConverter {
public:
    CodeConverter () {}
    QString parse (QString context, QString fromLang, QString toLang);

private:
    QString text;
};

#endif
