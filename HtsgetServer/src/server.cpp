#include <iostream>
#include "server.h"
#include "foo.h"

using namespace std;

int main()
{
    if (FOO_MACRO == 1)
    {
        cout << "here" << endl;
    }
    else
    {
        cout << "not here" << endl;
    }

    return 0;
}