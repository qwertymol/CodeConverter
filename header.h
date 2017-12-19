#include <QString>

struct LanguageDecl {
    const QString name;
    const QString includingTpl;
    const QString funcTpl;
};

LanguageDecl LANGS[] = {
    {
        "C++",
        "#include[ ]*<{LIB}>",
        "{RETTYPE}\s+{FUNCNAME}\s*(\s*{ARGSLIST}\s*)\s*{{BODYLINES}}"
    },
    {
        "PASCAL",
        "uses {LIB};",
        "function\s+{FUNCNAME}(\s*{ARGSLIST}\s*)"
    }
};
