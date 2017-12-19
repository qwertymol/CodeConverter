//#include "header.h"
#include "codeconverter.h"
#include <iostream>

using namespace std;

int main (int argc, char *argv[])
{
    CodeConverter cc;
    cout << cc.parse ("begin\nbla\nend.", "PASCAL", "C++").toStdString ();
    return 0;
}
