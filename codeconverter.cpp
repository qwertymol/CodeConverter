#include "header.h"
#include "codeconverter.h"

#include <QRegExp>
#include <QDebug>

QString CodeConverter::parse (QString context, QString fromLang, QString toLang)
{
    int lind1 = -1, lind2 = -1;
    int len = sizeof(LANGS) / sizeof(LanguageDecl);

    for (int i = 0; i < len; i++) {
        if (LANGS[i].name == fromLang)
            lind1 = i;
        if (LANGS[i].name == toLang)
            lind2 = i;
    }

    Q_ASSERT (lind1 != -1 && lind2 != -1);

    QRegExp re;
    QString pattern = LANGS[lind1].mainFuncTpl.pattern;
//    QString replace = LANGS[lind2].mainFuncTpl;
    re.setPattern (pattern);

    if (re.indexIn (context) != -1) {
        QString buf = re.cap(1);
//        qDebug () << buf;
        QString repl = LANGS[lind2].mainFuncTpl.after;
        repl.replace ("{BODY}", buf);
        context.replace (re, repl);
    }

    return context;
}
