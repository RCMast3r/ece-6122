#include <client.hpp>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <iostream>

/// @brief enable raw mode for terminal to get all input keys
void enableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ECHO | ICANON); 
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// 

/// @brief function to put terminal back to the way that it was 
void disableRawMode() {
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

int main()
{
    using namespace std::chrono_literals;
    std::cout << "Press keys (q to quit):" << std::endl;
    std::cout << "enter w: up, s: down, a: left, d: right"<< std::endl;
    
    Client client(54000);
    enableRawMode(); // Set terminal to raw mode

    char key;
    while (true) {
        read(STDIN_FILENO, &key, 1); // Read one character

        if (key == 'q') { // Quit on 'q'
            std::cout << "You are now exiting" << key << '\n';
            client.poll(key, true);
            break;
        } else {
            client.poll(key, false);
        }
    }
    disableRawMode(); // Restore terminal to its original mode

    return 0;
}