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
    const ReplaceTpl returnTpl;
    const ReplaceTpl funcTpl;
    const ReplaceTpl mainFuncTpl;
    const ReplaceTpl varInitTpl;
    const ReplaceTpl assignmentTpl;
};

LanguageDecl LANGS[] =
{
    {
        "C++",
        {
            "#include\\s*[<\"]([\\w\\/]+)[>\"]",
            "#include <{LIB}>"
        },
        {
            "return\\s*([^;]*);",
            "return {RETVAL};"
        },
        {
            "(\\w+)\\s+(\\w+)\\s*\\(\\s*((?:\\w+\\s*\\*?\\s*\\w+(?:\\[[0-9]*\\])?\\s*(,)\\s*)*(?:\\w+\\s*\\*?\\s*\\w+(?:\\[[0-9]*\\])?))?\\s*\\)\\s*(\\{)([\\s\\S]*)(\\})",
            "{RETTYPE} {FUNCNAME}({ARGSLIST}) {{BODYLINES}}"
        },
        {
            "(?:(?:int)|(?:void))\\s+main\\s*\\((?:int\\s+argc\\s*,\\s*char\\s+\\*\\s*argv\\s*\\[\\s*\\]\\s*)?\\)\\s*(\\{)([\\s\\S]*)(\\})",
            "int main(int argc, char *argv[]) {{BODY}}"
        },
        {
            "(\\w+)\\s+([a-zA-Z]\\w*)\\s*(?:=\\s*((?:'[\\s\\S]?')|(?:\"\\w*\")|(?:\\w+)|(?:\\w+\\([^\\)]*\\))))?;",
            "{TYPE} {VARNAME}{ = {VALUE}};"
        },
        {
            "(\\w+)\\s*=\\s*(\\w+(?:\\((?:\\s*(?:\\w+\\s*,)*\\w+\\s*)?\\))?)\\s*;",
//            "(\\w+)\\s*=\\s*([^;]*)\\s*;",
            "{LEFT} = {RIGHT};"
        }
    },
    {
        "PASCAL",
        {
            "uses\\s+(\\w+)\\s*;",
            "uses {LIB};"
        },
        {
            "result\\s*:=\\s*([^;]*);\\s*exit;",
            "result := {RETVAL}; exit;"
        },
        {
            "function\\s+(\\w+)\\s*\\(\\s*((?:\\w+\\s*:\\s*\\w+\\s*(;)\\s*)*\\s*\\w+\\s*:\\s*\\w+\\s*\\s*)?\\)\\s*:\\s*(\\w+)\\s*;\\s*(begin)([\\s\\S]*)(end);",
            "function {FUNCNAME}({ARGSLIST}): {RETTYPE};\nbegin{BODYLINES}end;"
        },
        {
            "(begin)([\\s\\S]*)(end)\\.",
            "begin{BODY}end."
        },
        {
            "var\\s+([a-zA-Z]\\w*)\\s*:\\s*(\\w+)\\s*(?::=\\s*((?:'[\\s\\S]*')|(?:\\w+)|(?:\\w+\\([^\\)]*\\))))?;",
            "var {VARNAME}: {TYPE}{ := {VALUE}};"
        },
        {
            "(\\w+)\\s*=\\s*(\\w+(?:\\((?:\\s*(?:\\w+\\s*,)*\\w+\\s*)?\\))?)\\s*;",
//            "(\\w+)\\s*=\\s*([^;]*)\\s*;",
            "{LEFT} := {RIGHT};"
        }
    }
};

#endif
