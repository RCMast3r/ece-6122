#include <ChessBackend.h>

bool ECE_ChessEngine::InitializeEngine()
{
    // Create pipes for communication
    if (pipe(_inputPipe) == -1 || pipe(_outputPipe) == -1) {
        perror("pipe");
        // exit(EXIT_FAILURE);
        return false;
    }
    // std::cout <<"created pipes" <<std::endl;
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return false;
    }
    // std::cout <<"pid done" <<std::endl;

    if (pid == 0) {
        // Child process: Set up pipes and execute the engine
        close(_inputPipe[1]); // Close the write end of the input pipe
        close(_outputPipe[0]); // Close the read end of the output pipe

        // Redirect stdin and stdout to the pipes
        dup2(_inputPipe[0], STDIN_FILENO);
        dup2(_outputPipe[1], STDOUT_FILENO);
        dup2(_outputPipe[1], STDERR_FILENO);

        // Close unused pipe ends
        close(_inputPipe[0]);
        close(_outputPipe[1]);

        // Path to the engine executabl
        std::cout <<"opening w/ execlp" <<std::endl;

        const char* enginePath = "./komodo"; // Ensure the path is correct and executable is present
        execlp(enginePath, enginePath, nullptr);

        // If execlp fails
        perror("execlp");
        return false;
        // exit(EXIT_FAILURE);
    } else {
        // Parent process: Close unused pipe ends
        // std::cout <<"erm" <<std::endl;
        close(_inputPipe[0]); // Close the read end of the input pipe
        close(_outputPipe[1]); // Close the write end of the output pipe

    }
    return true;
}

bool ECE_ChessEngine::sendMove(const std::string& strMove)
{
    std::string cmdWithNewline = strMove + "\n";
    write(_inputPipe[1], cmdWithNewline.c_str(), cmdWithNewline.size());
    return true;
}

bool ECE_ChessEngine::getResponseMove(std::string& strMove)
{
    char buffer[4096];
    ssize_t bytesRead = read(_outputPipe[0], buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the string
        strMove = std::string(buffer);
        return true;
    }
    return false;
}