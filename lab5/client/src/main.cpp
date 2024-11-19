#include <client.hpp>
#include <thread>
#include <chrono>

int main()
{
    using namespace std::chrono_literals;

    Client test_client(54000);
    while(true)
    {
        std::this_thread::sleep_for(10ms);
        test_client.poll();
    }
    return 0;
}