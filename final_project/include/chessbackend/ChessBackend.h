#ifndef __CHESSBACKEND_H__
#define __CHESSBACKEND_H__
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <vector>

class ECE_ChessEngine
{
    public:

    ECE_ChessEngine() {}

    bool InitializeEngine();
    bool sendMove(const std::string& strMove);
    bool getResponseMove(std::string& strMove);
    private:

    int _inputPipe[2], _outputPipe[2];


};

#endif // __CHESSBACKEND_H__