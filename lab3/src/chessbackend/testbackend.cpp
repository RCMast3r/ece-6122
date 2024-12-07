/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 12/07/24
Description: a test application
*/
#include <ChessBackend.h>

int main() {

    ECE_ChessEngine backend;
    if(!backend.InitializeEngine()){
        std::cout <<"ahhhh" <<std::endl;
    }

    // SendToEngine("uci");
    backend.sendMove("uci");
    std::string out;
    // std::cout << "Engine Response: " << backend.getResponseMove(out) << std::endl;

    // SendToEngine("isready");
    backend.sendMove("isready");
    // std::cout << "Engine Response: " << backend.getResponseMove(out) << std::endl;

    // Example: position, followed by best move
    backend.sendMove("position startpos moves e2e4");
    backend.sendMove("go depth 10");

    while (out.find("bestmove") == std::string::npos) {
        (void)backend.getResponseMove(out);
        // std::cout << "Engine Response: " << out << std::endl;
    }

    std::cout << "Engine best move 1: " << std::endl;
    // std::string line;
    auto move1 = out.substr(out.find("bestmove")+9, 4);
    std::cout << move1 << std::endl;

    (void)backend.sendMove("position startpos moves d2d3");
    (void)backend.sendMove("go depth 10");

    
    while (out.find("bestmove") == std::string::npos) {
        (void)backend.getResponseMove(out);
        // std::cout << "Engine Response: " << out << std::endl;
    }
    std::cout << "Engine best move 2: " << std::endl;
    // std::string line;
    auto move2 = out.substr(out.find("bestmove")+9, 4);
    std::cout << move2 << std::endl;


    return 0;
}
