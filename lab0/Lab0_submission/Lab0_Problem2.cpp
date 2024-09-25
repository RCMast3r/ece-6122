#include <iostream>
#include <vector>
#include <cmath>

bool is_prime(int input)
{
    for (int i = 2; i < input; i++)
    {
        if (!(input % i))
        {
            return false;
        }
    }
    return true;
}

int main()
{
    while (true)
    {
        int input = 0;
        int sum_of_primes = 0;
        int prev_number = 0;

        std::cout << "Please enter a natural number (0 to quit): " << std::flush;
        std::cin >> input;
        if (std::cin.fail() || input < 0)
        {
            std::cout << "Error! invalid input!" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (input == 0)
        {
            std::cout << "Program terminated" << std::endl;
            std::cout << "Have a nice day!" << std::endl;
            break;
        }
        

        for(int i=2; i <= input;i++)
        {
            if(is_prime(i))
            {
                sum_of_primes += i;
            }
        }

        std::cout << "The sum of primes: " << sum_of_primes << std::endl;

        
    }
    return 0;
}