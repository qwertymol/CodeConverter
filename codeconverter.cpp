
#include "header.h"
#include "codeconverter.h"

#include <QRegExp>
#include <QDebug>

QString CodeConverter::parse (QString &context, QString fromLang, QString toLang)
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

    context = assignmentReplace (context, &LANGS[lind1].assignmentTpl, &LANGS[lind2].assignmentTpl);
    context = returnReplacer (context, &LANGS[lind1].returnTpl, &LANGS[lind2].returnTpl);
    context = varInitReplace (context, &LANGS[lind1].varInitTpl, &LANGS[lind2].varInitTpl, fromLang);
    context = includingDeclReplace (context, &LANGS[lind1].includingTpl, &LANGS[lind2].includingTpl, fromLang);
    context = mainFuncReplace (context, &LANGS[lind1].mainFuncTpl, &LANGS[lind2].mainFuncTpl, fromLang);
    context = funcDeclReplace (context, &LANGS[lind1].funcTpl, &LANGS[lind2].funcTpl, fromLang);

    if (fromLang == "PASCAL")
        context = "#include<iostream>\n" + context;

    context = context.trimmed ();

    return context;
}


QString &CodeConverter::returnReplacer (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo)
{
    QRegExp re;
    QString pattern = tplFrom->pattern;
    re.setPattern (pattern);
    int offset = 0;

    while (re.indexIn (context, offset) != -1) {
        QString buf = re.cap(1);
        QString repl = tplTo->after;
        repl.replace ("{RETVAL}", buf.trimmed ());
        context.replace (re.cap (), repl);
        offset += re.cap ().length ();
    }

    return context;
}


QString CodeConverter::argsReplacer (QString &args, QString fromLang)
{
    TypesReplacer tr;

    if (fromLang == "C++") {
        QStringList sl = args.split (",");
        QStringList buf;

        for (int i = 0; i < sl.length (); i++) {
            QStringList arg = sl[i].trimmed ().split(" ");
            buf << arg[1] + ":" + tr.getType(arg[0], fromLang);
        }

        QString res = buf.join ("; ");
        return res;
    } else if (fromLang == "PASCAL")  {
        QStringList sl = args.split (";");
        QStringList buf;

        for (int i = 0; i < sl.length (); i++) {
            QStringList arg = sl[i].trimmed ().split(":");
            buf << tr.getType(arg[1], fromLang) + " " + arg[0];
        }

        QString res = buf.join (", ");
        return res;
    }/* else {
        return QString ();
    }*/
}


QString &CodeConverter::mainFuncReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang)
{
    QRegExp re;
    QString pattern = tplFrom->pattern;
    re.setPattern (pattern);

    if (re.indexIn (context) != -1) {

        QString body = re.cap(2);
        int index = 0, innerBlock = 0;
        while (1) {
            int openBrace = body.indexOf (re.cap(1), index + 1), closeBrace = body.indexOf (re.cap(3), index + 1);
            if (closeBrace != -1 && openBrace != -1 && openBrace < closeBrace) {
                innerBlock++;
                index = openBrace;
            } else if (closeBrace != -1 && openBrace != -1 && openBrace > closeBrace) {
                innerBlock--;
                index = closeBrace;
            } else {
                break;
            }

            if (innerBlock < 0) {
                body = body.left (index);
                break;
            }
        }

        QString repl = tplTo->after;
        repl.replace ("{BODY}", body);
        QString res = re.cap(0).replace (re.cap(2), body);
        context.replace (res, repl);

        context = functionContainReplace(context, fromLang);
    }

    return context;
}


QString &CodeConverter::funcDeclReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang)
{
    QRegExp re;
    bool reversed = tplFrom->after.indexOf ("{RETTYPE}") < tplTo->after.indexOf ("{RETTYPE}");
    QString pattern = tplFrom->pattern;
    re.setPattern (pattern);
    int offset = 0;

    while (re.indexIn (context, offset) != -1) {
        QStringList txts = re.capturedTexts ();
        QString body = txts[6];
        int index = 0, innerBlock = 0;
        while (1) {
            int openBrace = body.indexOf (txts[5], index + 1), closeBrace = body.indexOf (txts[7], index + 1);
            if (closeBrace != -1 && openBrace != -1 && openBrace < closeBrace) {
                innerBlock++;
                index = openBrace;
            } else if (closeBrace != -1 && openBrace != -1 && openBrace > closeBrace) {
                innerBlock--;
                index = closeBrace;
            } else {
                break;
            }

            if (innerBlock < 0) {
                body = body.left (index);
                break;
            }
        }

        offset += body.length ();

        QString repl = tplTo->after;
        TypesReplacer tr;

        repl.replace ("{RETTYPE}", reversed ? tr.getType(txts[1], fromLang) : tr.getType(txts[4], fromLang));
        repl.replace ("{FUNCNAME}", reversed ? txts[2] : txts[1]);
        repl.replace ("{ARGSLIST}", argsReplacer(reversed ? txts[3] : txts[2], fromLang));
        repl.replace ("{BODYLINES}", funcDeclReplace(body, tplFrom, tplTo, fromLang));

        QString res = txts[0].replace (txts[6], body);

        context.replace (res, repl);
    }

    return context;
}


QString &CodeConverter::includingDeclReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang)
{
    QRegExp re;
    QString pattern = tplFrom->pattern;
    re.setPattern (pattern);

//    if (re.indexIn (context) != -1) {
        QString buf = re.cap(1);
        QString repl = tplTo->after;

        LibsReplacer lr;
        QString lib = lr.getLib (buf, fromLang);

        if (!lib.isEmpty ()) {
            repl.replace ("{LIB}", buf);
            context.replace (re, repl);
        } else {
            context.replace (re, "");
        }
//    }

    return context;
}


QString &CodeConverter::varInitReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang)
{
    QRegExp re;
    QString pattern = tplFrom->pattern;
    bool reversed = tplFrom->after.indexOf ("{TYPE}") > tplTo->after.indexOf ("{TYPE}");
    re.setPattern (pattern);
    int index = 0;

    while ((index = re.indexIn (context, index)) != -1) {
        QStringList txts = re.capturedTexts ();
        QString repl = tplTo->after;

        TypesReplacer tr;

        repl.replace ("{VARNAME}", reversed ? txts[1] : txts[2]);
        repl.replace ("{TYPE}", reversed ? tr.getType(txts[2], fromLang) : tr.getType(txts[1], fromLang));

        if(txts[3].isEmpty()) {
            int in = repl.indexOf("{");
            repl.truncate(in);
            repl += ";";
        } else {
            repl.replace ("{VALUE}", txts[3]);
            repl.replace ("{", "");
            repl.replace ("}", "");
        }

        context.replace (re.cap (), repl);
    }

    return context;
}


QString &CodeConverter::assignmentReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo)
{
    QRegExp re;
    QString pattern = tplFrom->pattern;
    re.setPattern (pattern);

    if (re.indexIn (context) != -1) {
        QString repl = tplTo->after;
        repl.replace ("{LEFT}", re.cap(1));
        repl.replace ("{RIGHT}", re.cap(2));
        context.replace (re, repl);
    }

    return context;

}


QString &CodeConverter::functionContainReplace (QString &context, QString fromLang)
{
    QRegExp r("([^<>(\n)]+)[^;\\n=]*;");

    int offset = 0;

    while (r.indexIn (context, offset) != -1) {
        BaseFuncsReplacer bfr;
        QString first = r.cap (1);
        offset = r.indexIn (context, offset) + first.length () + 1;

        TemplatePair tpl1 = bfr.getFuncTpl (first.trimmed (), fromLang);

        if (tpl1.pattern.isEmpty ())
            continue;

        QRegExp re (tpl1.pattern);
        if (re.indexIn (context) != -1) {
            QString argsStr = re.cap (1);
            QString delim = re.cap (2);

            QStringList args;
            if (!delim.isEmpty ()) {
                args = argsStr.split (delim);
            } else {
                args << argsStr;
            }

            args.removeAll ("");

            if (args.length () == 0) {
                tpl1.after.replace (QRegExp ("{[^}]+}[^G]+G"), "");
            } else if (args.length () == 1) {
                tpl1.after.replace (QRegExp ("\\{[^}]+\\}"), "");
                tpl1.after.replace ("ARG", args [0].trimmed ());
            } else {
                QRegExp rr ("\\{[^}]*ARG[^}]*\\}");

                while (args.length () > 1) {
                    int pos = rr.indexIn (tpl1.after);
                    tpl1.after.replace (rr.cap (0), rr.cap (0) + rr.cap (0));

                    int kwPos = tpl1.after.indexOf ("ARG", pos);
                    tpl1.after.replace (kwPos, 3, args[0].trimmed ());

                    args.removeFirst ();
                }
                tpl1.after.replace (rr, "");

                tpl1.after.replace ("ARG", args [0].trimmed ());
                tpl1.after.replace ("{", "");
                tpl1.after.replace ("}", "");
            }

            context.replace (r.cap (0).trimmed (), tpl1.after);
        }
    }

    return operationReplacer (context, fromLang);
}


QString &CodeConverter::operationReplacer (QString &context, QString fromLang)
{
    OperationReplacer o;
    QString pattern = o.getOppositeMatchPattern (fromLang);
    QRegExp re (pattern);
    int index = 0;

    while ((index = re.indexIn (context, index + re.cap ().length ())) != -1) {
        QString op = o.getOperation (re.cap (), fromLang);
        if (op.isEmpty ())
            continue;

        context.replace (index, re.cap ().length (), op);
    }

    return context;
}


TypesReplacer::TypesReplacer ()
{
    langs <<  "C++" <<  "PASCAL";

    Int =    {"int",    "integer"};
    String = {"char *", "string"};
    Double = {"double", "real"};
    Float  = {"float",  "real"};
    Char   = {"char",   "char"};
    Byte   = {"char",   "byte"};
    Bool   = {"bool",   "boolean"};
    Void   = {"void",   ""};
}


QString TypesReplacer::getType (QString type, QString from)
{
    int index = langs.indexOf (from);

    for(int i = 0; i < TSIZE; i++) {
        switch(i){
        case TINT:
            if (getTypeFromPair (Int, index) == type)
                return getTypeFromPair (Int, 1 - index);
            break;

        case TSTRING:
            if (getTypeFromPair (String, index) == type)
                return getTypeFromPair (String, 1 - index);
            break;

        case TDOUBLE:
            if (getTypeFromPair (Double, index) == type)
                return getTypeFromPair (Double, 1 - index);
            break;

        case TFLOAT:
            if (getTypeFromPair (Float, index) == type)
                return getTypeFromPair (Float, 1 - index);
            break;

        case TCHAR:
            if (getTypeFromPair (Char, index) == type)
                return getTypeFromPair (Char, 1 - index);
            break;

        case TBYTE:
            if (getTypeFromPair (Byte, index) == type)
                return getTypeFromPair (Byte, 1 - index);
            break;

        case TBOOL:
            if (getTypeFromPair (Bool, index) == type)
                return getTypeFromPair (Bool, 1 - index);
            break;

        case TVOID:
            if (getTypeFromPair (Void, index) == type)
                return getTypeFromPair (Void, 1 - index);
            break;

        }
    }
}


QString TypesReplacer::getTypeFromPair (TypesReplacer::TypePair &tp, int n)
{
    return n == 0 ? tp.first : tp.second;
}


LibsReplacer::LibsReplacer ()
{
    langs <<  "C++" <<  "PASCAL";

    Main = {"iostream", ""};
    FileIO = {"fstream", ""};
}


QString LibsReplacer::getLib (QString lib, QString from)
{
    int index = langs.indexOf (from);

    for (int i = 0; i < sizeof (LIBS); i++) {
        switch (i) {
        case MAIN:
            if (getLibFromPair (Main, index) == lib)
                return getLibFromPair (Main, 1 - index);
            break;
        case FILEIO:
            if (getLibFromPair (FileIO, index) == lib)
                return getLibFromPair (FileIO, 1 - index);
            break;
        }
    }

    return "";
}


QString LibsReplacer::getLibFromPair (LibsReplacer::LibPair &lp, int n)
{
    return n == 0 ? lp.first : lp.second;
}


BaseFuncsReplacer::BaseFuncsReplacer()
{
    langs <<  "C++" <<  "PASCAL";

    funcs << FuncPair
    {
        {
             "(?:std\\s*::\\s*)?cout\\s*(?:<<\\s*((?:[^<]+\\s*(<<)\\s*)*[^<]+)\\s*)?<<\\s*(?:std\\s*::\\s*)?endl\\s*;",
             "writeln ({ARG, }ARG);"
        },
        {
             "writeln\\s*\\(\\s*((?:(?:[^(]+(?:\\([^)]*\\))?\\s*(,)\\s*)*(?:[^(]+(?:\\([^)]*\\))?))?)\\s*\\)\\s*;",
             "std::cout{ << ARG} << ARG << std::endl;"
        }
    }    << FuncPair
    {
        {
             "(?:std\\s*::\\s*)?cin\\s*(?:>>\\s*((?:\\w+\\s*(>>)\\s*)*\\w+)\\s*)?;",
             "readln ({ARG, }ARG);"
        },
        {
             "readln\\s*\\(\\s*((?:(?:\\w+\\s*(,)\\s*)*(?:\\w+))?)\\s*\\)\\s*;",
             "std::cin {>> ARG} >> ARG;"
        }
    };
}


TemplatePair BaseFuncsReplacer::getFuncTpl (QString funcName, QString from)
{
    int index = langs.indexOf (from);

    if (funcName.trimmed ().isEmpty ())
        return TemplatePair ();

    for (int i = 0; i < funcs.length (); i++) {
        TemplatePair tpl = index == 0 ? funcs[i].first : funcs[i].second;
        TemplatePair alterTpl = index == 0 ? funcs[i].second : funcs[i].first;

        if (alterTpl.after.contains (QRegExp("\\b" + funcName + "\\b")))
            return tpl;
    }

    return TemplatePair ();
}


OperationReplacer::OperationReplacer ()
{
    langs <<  "C++" <<  "PASCAL";
    matchPatterns << "(?:\\+\\+|--|\\+|-|\\/|\\*|%|&&|\\|\\||\\^\\^|==|<=|<|>=|>)"
                  << "(?:\\+=1|-=1|mod|div|and|xor|or|\\+|-|\\/|\\*|=|<=|<|>=|>)";

    ops << OpPair {"+", "+"} << OpPair {"-", "-"} << OpPair {"/", "/"} << OpPair {"*", "*"}
        << OpPair {"/", "div"} << OpPair {"%", "mod"} << OpPair {"&&", "and"} << OpPair {"||", "or"}
        << OpPair {"^^", "xor"} << OpPair {"++", "+=1"} << OpPair {"--", "-="} << OpPair {"==", "="}
        << OpPair {"!=", "<>"} << OpPair {"<", "<"} << OpPair {">", ">"} << OpPair {"<=", "<="}
        << OpPair {">=", ">="};
}


QString OperationReplacer::getOperation (QString operation, QString from)
{
    int index = langs.indexOf (from);

    for (int i = 0; i < ops.length (); i++)
        if (getOperationFromPair (ops[i], index) == operation)
            return getOperationFromPair (ops[i], 1 - index);

    return QString ();
}


QString OperationReplacer::getOppositeMatchPattern (QString from)
{
    int index = langs.indexOf (from);

    return matchPatterns[index];
}


QString OperationReplacer::getOperationFromPair (OperationReplacer::OpPair &op, int n)
{

    return n == 0 ? op.first : op.second;
}
