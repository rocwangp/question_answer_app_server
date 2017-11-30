#include <iostream>

#include "python2.7/Python.h"

int main()
{
    using namespace std;
    Py_Initialize();
    if(!Py_IsInitialized())
    {
        cout << "call python spider error" << endl;
        return 0;
    }

    PyRun_SimpleString("import sys");

    if(!PyRun_SimpleString("execfile('./zhihu_crawler.py')"))
    {
        return -1;
    }
    Py_Finalize();
    return 0;
}
