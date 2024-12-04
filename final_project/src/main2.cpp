#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <vector>

// File descriptors for input and output pipes
int inputPipe[2], outputPipe[2];

void StartEngine() {
    // Create pipes for communication
    if (pipe(inputPipe) == -1 || pipe(outputPipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process: Set up pipes and execute the engine
        close(inputPipe[1]); // Close the write end of the input pipe
        close(outputPipe[0]); // Close the read end of the output pipe

        // Redirect stdin and stdout to the pipes
        dup2(inputPipe[0], STDIN_FILENO);
        dup2(outputPipe[1], STDOUT_FILENO);
        dup2(outputPipe[1], STDERR_FILENO);

        // Close unused pipe ends
        close(inputPipe[0]);
        close(outputPipe[1]);

        // Path to the engine executable
        const char* enginePath = "./komodo"; // Ensure the path is correct and executable is present
        execlp(enginePath, enginePath, nullptr);

        // If execlp fails
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: Close unused pipe ends
        close(inputPipe[0]); // Close the read end of the input pipe
        close(outputPipe[1]); // Close the write end of the output pipe
    }
}

void SendToEngine(const std::string& command) {
    std::string cmdWithNewline = command + "\n";
    write(inputPipe[1], cmdWithNewline.c_str(), cmdWithNewline.size());
}

std::string ReadFromEngine() {
    char buffer[4096];
    ssize_t bytesRead = read(outputPipe[0], buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the string
        return std::string(buffer);
    }
    return {};
}

int main() {
    StartEngine();

    SendToEngine("uci");
    std::cout << "Engine Response: " << ReadFromEngine() << std::endl;

    SendToEngine("isready");
    std::cout << "Engine Response: " << ReadFromEngine() << std::endl;

    // Example: position, followed by best move
    SendToEngine("position startpos moves e2e4");
    SendToEngine("go depth 10");

    std::string response;
    while ((response = ReadFromEngine()).find("bestmove") == std::string::npos) {
        std::cout << "Engine Response: " << response << std::endl;
    }

    std::cout << "Engine best move: " << response << std::endl;

    SendToEngine("position startpos moves d2d3");
    SendToEngine("go depth 10");

    while ((response = ReadFromEngine()).find("bestmove") == std::string::npos) {
        std::cout << "Engine Response: " << response << std::endl;
    }

    std::cout << "Engine best move: " << response << std::endl;

    return 0;
}
