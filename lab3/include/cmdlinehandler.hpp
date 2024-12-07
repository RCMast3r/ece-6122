/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 12/07/24
Description: 

cmd handler function prototype and associated shared type definitions
*/

#ifndef __CMDLINEHANDLER_H__
#define __CMDLINEHANDLER_H__

#include <string>
#include <optional>
#include <iostream>
#include <variant>
#include <vector>

/**
 * @enum cmdType
 * @brief Enum class representing different command types.
 * 
 * This enum class defines various types of commands that can be used in the system. 
 * It includes types for camera control, light control, movement, power management, 
 * and quitting the application.
 */
enum class cmdType {
    CAMERA = 0, /**< Command for controlling the camera. */
    LIGHT,      /**< Command for controlling the lights. */
    MOVE,       /**< Command for movement. */
    POWER,      /**< Command for power management. */
    QUIT        /**< Command for quitting the application. */
};

/**
 * @struct coords
 * @brief Represents a 3D coordinate system in spherical coordinates.
 * 
 * This struct represents a set of spherical coordinates with three components:
 * theta (azimuthal angle), phi (polar angle), and r (radius or distance).
 */
struct coords {
    float theta; /**< Azimuthal angle (theta) in radians. */
    float phi;   /**< Polar angle (phi) in radians. */
    float r;     /**< Radius or distance from the origin. */
};

/**
 * @struct CMD
 * @brief Represents a command that can be issued with a type and an optional verb.
 * 
 * This struct represents a command with two fields:
 * - `type`: The type of command (e.g., MOVE, CAMERA).
 * - `cmdVerb`: An optional value that specifies additional data related to the command, 
 *   which can be a `coords` struct, a float, a string, or a monostate (no value).
 */
struct CMD {
    cmdType type; /**< The type of command (e.g., MOVE, CAMERA). */
    
    /**
     * @brief Optional additional data associated with the command.
     * 
     * This field is optional and can store various types of data depending on the command:
     * - `coords` for 3D positions (e.g., for movement or camera positioning).
     * - `float` for numeric values (e.g., power levels).
     * - `std::string` for textual data (e.g., command parameters).
     * - `std::monostate` represents an empty or unused state.
     */
    std::optional<std::variant<coords, float, std::string, std::monostate>> cmdVerb;
};

/**
 * @brief Splits a string into substrings based on a delimiter.
 * 
 * This function takes a string and splits it into substrings using a specified delimiter. 
 * The substrings are returned in a vector of strings.
 * 
 * @param s The string to be split.
 * @param delimiter The delimiter used to split the string.
 * 
 * @return A vector of strings, each containing a substring from the original string.
 */
std::vector<std::string> split(const std::string& s, const std::string& delimiter);

/**
 * @brief Retrieves a command from the user input.
 * 
 * This function processes user input to generate a command. If the input is valid, 
 * a `CMD` object is returned encapsulating the type and optional data (verb). 
 * If no valid command is detected, it returns an empty `std::optional`.
 * 
 * @return An optional `CMD` object containing the user command, or an empty optional if no valid command is found.
 */
std::optional<CMD> getUserCommand();

#endif // __CMDLINEHANDLER_H__