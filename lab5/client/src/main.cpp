/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 11/19/24
Description: main file for the main() function. 
Also contains the commandline argument handling and terminal reading for the key presses
*/


#include <iostream>
#include <thread>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <client.hpp>
#include <thread>
#include <chrono>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <string>

// Shared variable to store the last key press
std::atomic<char> lastKey{0};

// Function to set terminal mode to raw
void enableRawMode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

// Function to restore terminal to its original mode
void disableRawMode()
{
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= (ECHO | ICANON); // Re-enable echo and canonical mode
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

/// function to handle user input in a separate thread
void inputThreadFunction()
{
    char key;
    while (true)
    {
        read(STDIN_FILENO, &key, 1); // Blocking call
        lastKey.store(key);

        if (key == 'q')
        {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    uint16_t port = 54000;
    if(argc >1)
    {
        port = std::stoi(argv[1]);
    }
    std::cout << "Press keys (q to quit):" << std::endl;
    std::cout << "w: up, s: down, a: left, d:right" << std::endl;
    enableRawMode(); // Set terminal to raw mode
    Client client(port);
    // Start the input thread
    std::thread inputThread(inputThreadFunction);

    // Main thread loop
    while (true)
    {
        char key = lastKey.exchange(0); // Get and clear the last key press
        if (key)
        {
            if (key == 'q')
            { // Quit on 'q'
                (void)client.poll(std::nullopt, true);
                break;
            }
        }

        if (!key)
        {
            if (!client.poll(std::nullopt, false))
            {
                std::cout << "server disconnected" << std::endl;
            }
        }
        else
        {
            if (!client.poll(key, false))
            {
                std::cout << "server disconnected" << std::endl;
            }
        }
    }

    inputThread.join(); // Wait for the input thread to finish
    disableRawMode();   // Restore terminal to its original mode

    return 0;
}
