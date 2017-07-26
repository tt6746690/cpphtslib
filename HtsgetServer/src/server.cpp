#include <iostream>
#include <chrono>

#include "server.h"

using namespace std;

int main()
{
    cout << "here" << endl;

    asio::io_service io;
    asio::deadline_timer(io, std::chrono::seconds(1));

    return 0;
}