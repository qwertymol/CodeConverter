#ifndef CODECONVERTER_H
#define CODECONVERTER_H

#include <QString>
#include <QStringList>

struct ReplaceTpl;

class CodeConverter {
public:
    CodeConverter () {}
    QString parse (QString &context, QString fromLang, QString toLang);

private:
    QString &returnReplacer (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo);
    QString argsReplacer (QString &args, QString fromLang);
    QString &mainFuncReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang);
    QString &funcDeclReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang);
    QString &includingDeclReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang);
    QString &varInitReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo, QString fromLang);
    QString &assignmentReplace (QString &context, const ReplaceTpl *tplFrom, const ReplaceTpl *tplTo);
    QString &functionContainReplace (QString &context, QString fromLang);
    QString &operationReplacer (QString &context, QString fromLang);
};

class OperationReplacer {
public:
    OperationReplacer ();
    QString getOperation (QString operation, QString from);
    QString getOppositeMatchPattern (QString from);

private:
    QStringList langs;
    QStringList matchPatterns;
    struct OpPair {
        QString first, second;
    };
    enum OPS {PLUS, MINUS, DIVIDE, MULTIPLY, DIV, MOD, AND, OR, XOR, INC, DEC, EQUAL, NONEQUAL, LESS, MORE, LESSOREQUAL, MOREOREQUAL};
    QList<OpPair> ops;

    QString getOperationFromPair (OpPair &op, int n);

};

class TypesReplacer {
public:
    TypesReplacer ();
    QString getType (QString type, QString from);

private:
    QStringList langs;
    struct TypePair {
        QString first, second;
    };
    enum TYPES {TINT, TSTRING, TDOUBLE, TFLOAT, TCHAR, TBYTE, TBOOL, TVOID, TSIZE};
    TypePair Int, String, Double, Float, Char, Byte, Bool, Void;

    QString getTypeFromPair (TypePair &tp, int n);
};


class LibsReplacer {
public:
    LibsReplacer ();
    QString getLib (QString lib, QString from);

private:
    QStringList langs;
    struct LibPair {
        QString first, second;
    };
    enum LIBS {MAIN, FILEIO};
    LibPair Main, FileIO;

    QString getLibFromPair (LibPair &lp, int n);
};


struct TemplatePair {
    QString pattern;
    QString after;
};


class BaseFuncsReplacer {
public:
    BaseFuncsReplacer ();
    TemplatePair getFuncTpl (QString funcName, QString from);

private:
    QStringList langs;
    struct FuncPair {
        TemplatePair first, second;
    };
    QList<FuncPair> funcs;
};

#endif
