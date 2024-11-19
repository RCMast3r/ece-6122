#include <server.hpp>
#include <thread>
#include <chrono>

int main()
{
    using namespace std::chrono_literals;

    Server test_server(54000);
    while(true)
    {
        std::this_thread::sleep_for(10ms);
        test_server.poll();
    }
    return 0;
}