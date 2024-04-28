#include "addosc.h"
#include <iostream>

using namespace soutel;

int main(int arc, char *argv[])
{
    AddOsc<double> osc(400.0);

    osc.set_frequency(2.0);

    for (int i = 0; i < 400; i++)
    {
        std::cout << osc.run() << std::endl;
    }
}