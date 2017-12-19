#ifndef HEADER_H
#define HEADER_H

#include <QString>

struct ReplaceTpl {
    QString pattern;
    QString after;
};

struct LanguageDecl {
    const QString name;
    const ReplaceTpl includingTpl;
    const ReplaceTpl funcTpl;
    const ReplaceTpl mainFuncTpl;
};

LanguageDecl LANGS[] = {
    {
        "C++",
        {
            "#include[ ]*<(.*)>",
            "#include <{LIB}>"
        },
        {
            "(.*)\\s+(.*)\\s*(\\s*(.*)\\s*)\\s*{\\s*(.*)\\s*}",
            "{RETTYPE}\\s+{FUNCNAME}\\s*(\\s*{ARGSLIST}\\s*)\\s*{\\s*{BODYLINES}\\s*}"
        },
        {
            "int\\s+main\\s*\\(\\s*(int argc, char \\*argv\\[\\])?\\s*\\)\\s*\\{\\s*(.*)\\s*\\}",
            "int main(int argc, char *argv) {\n{BODY}\n}"
        }
    },
    {
        "PASCAL",
        {
            "uses[ ]+{LIB}\\s*;",
            "uses[ ]+{LIB}\\s*;"
        },
        {
            "function\\s+{FUNCNAME}(\\s*{ARGSLIST}\\s*)\\s*:\\s*{RETTYPE}\\s*;\\s*begin\\s+{BODYLINES}\\s+end;",
            "function\\s+{FUNCNAME}(\\s*{ARGSLIST}\\s*)\\s*:\\s*{RETTYPE}\\s*;\\s*begin\\s+{BODYLINES}\\s+end;"
        },
        {
            "begin\\s+(.*)\\s+end\\.",
            "begin\n{BODY}\nend."
        }
    }
};

#endif
