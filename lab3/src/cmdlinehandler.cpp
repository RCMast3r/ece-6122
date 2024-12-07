#include <cmdlinehandler.hpp>

std::vector<std::string> split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;

    while ((end = s.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(s.substr(start, end - start)); // Extract token
        start = end + delimiter.length();              // Move past the delimiter
    }

    // Add the last token
    tokens.push_back(s.substr(start));

    return tokens;
}

std::optional<CMD> getUserCommand()
{
    CMD out;
    std::string input;
    std::cout << "Please enter a command: ";
    std::getline(std::cin, input);

    auto verbs = split(input, " ");
    if (verbs.empty()) {
        std::cout << "Invalid command or move!!" << std::endl;
        return std::nullopt;
    }

    if (verbs[0] == "quit") {
        out.type = cmdType::QUIT;
        out.cmdVerb = std::monostate{}; // No additional data
        return out;
    } else if (verbs[0] == "camera" || verbs[0] == "light") {
        std::cout << input <<std::endl;
        if (verbs.size() != 4) {
            std::cout << "Invalid command or move!! not enough args" << std::endl;
            return std::nullopt;
        }

        try {
            coords c;
            c.theta = std::stof(verbs[1]);
            c.phi = std::stof(verbs[2]);
            c.r = std::stof(verbs[3]);

            // Validate ranges
            // if (c.theta < 10.0f || c.theta > 80.0f || c.phi < 0.0f || c.phi > 360.0f || c.r <= 0.0f) {
                // std::cout << "Invalid command or move!! invalid range" << std::endl;
                // return std::nullopt;
            // }

            out.type = (verbs[0] == "camera") ? cmdType::CAMERA : cmdType::LIGHT;
            out.cmdVerb = c;
            return out;

        } catch (const std::invalid_argument& e) {
            std::cout << e.what() <<std::endl;
            // std::cout << verbs[1] <<std::endl;
            // std::cout << verbs[2] <<std::endl;
            // std::cout << verbs[3] <<std::endl;
            std::cout << "Invalid command or move!! catch" << std::endl;
            return std::nullopt;
        }
    } else if (verbs[0] == "power") {
        if (verbs.size() != 2) {
            std::cout << "Invalid command or move!!" << std::endl;
            return std::nullopt;
        }

        try {
            float powerLevel = std::stof(verbs[1]);
            if (powerLevel < 0.0f) {
                std::cout << "Invalid power level!!" << std::endl;
                return std::nullopt;
            }

            out.type = cmdType::POWER;
            out.cmdVerb = powerLevel;
            return out;

        } catch (const std::invalid_argument& e) {
            std::cout << "Invalid command or move!!" << std::endl;
            return std::nullopt;
        }
    } else if (verbs[0] == "move") {
        if (verbs.size() != 2) {
            std::cout << "Invalid command or move!!" << std::endl;
            return std::nullopt;
        }
        out.type = cmdType::MOVE;
        out.cmdVerb = std::monostate{}; // No additional data for simplicity
        return out;
    }

    std::cout << "Invalid command or move!!" << std::endl;
    return std::nullopt;
}