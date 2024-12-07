// Include standard headers
/*
Author: Ben Hall
Class: ECE6122 (section)
Last Date Modified: 12/07/24
Description: a large main application source containing the main() and some helper functions
*/
#include "cmdlinehandler.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>

#include <chesspiece.hpp>
#include <ChessStateManager.h>
#include <ChessBackend.h>

#include <thread>
#include <atomic>
#include <mutex>
#include <optional> // For std::optional

std::atomic<bool> newCommandAvailable(false);
std::optional<CMD> cmd = std::nullopt; // std::optional<CMD> for the shared command
std::mutex cmdMutex;


glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;



// Initial position : on +Z
glm::vec3 position = glm::vec3( 0.0f, 1.6f, 1.4f );

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = -1.0f;
// Initial Field of View
float initialFoV = 70.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

glm::vec3 lightPos = glm::vec3(4, 4, 4);
float lightPower = 75.0f;

// function to be given to thread for getting th user command within a thread to stop the window from dying constantly
void getUserCommandThread()
{
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
        std::optional<CMD> newCmd = getUserCommand(); // Get new command
        
        {
            std::lock_guard<std::mutex> lock(cmdMutex); // Lock for thread-safe access
            cmd = newCmd; // Update shared command
        }
        
        newCommandAvailable = true; // Notify that new command is available
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Adjust sleep duration as needed
    }
}

// get the real position of the light from the desired sperical coordinates
glm::vec3 computeNewLightPosFromCoords(coords c)
{
    // Convert angles from degrees to radians
    float thetaRad = glm::radians(c.theta);
    float phiRad = glm::radians(c.phi);

    // Compute Cartesian coordinates
    float x = c.r * cos(phiRad) * sin(thetaRad);
    float y = c.r * sin(phiRad);
    float z = c.r * cos(phiRad) * cos(thetaRad);

    return glm::vec3(x, y, z); 
}

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// depending on if a command exists or not, compute new viz matrices.

// technically, the theta is horizontal meaning that yes, it should indeed rotate to right right in theory 
// (*if wikipedia is to be understood correctly)
void computeMatricesFromInputs(std::optional<CMD> cmd) {
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();

    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Default radius (distance from origin) and angles
    static float radius = 2.0f;
    static float horizontalAngle = glm::radians(180.0f); // Default: facing forward
    static float verticalAngle = glm::radians(120.0f);    // Default: no elevation

    // Update from command input
    if (cmd) {
        if ((*cmd).type == cmdType::CAMERA) {
            coords c = std::get<coords>((*cmd).cmdVerb.value()); // Get spherical coordinates
            horizontalAngle = glm::radians(c.theta + 180.0f);
            verticalAngle = glm::radians((-c.phi) + 120.0f);
            radius = c.r; // Update radius
        }
    }

    // Ensure vertical angle stays within bounds to prevent flipping

    // Calculate the camera's position in Cartesian coordinates
    glm::vec3 position = glm::vec3(
        radius * cos(verticalAngle) * sin(horizontalAngle),
        radius * sin(verticalAngle),
        radius * cos(verticalAngle) * cos(horizontalAngle)
    );

    // Direction vector (always pointing to the origin)
    glm::vec3 direction = -position;

    // Right vector (perpendicular to the direction, in the horizontal plane)
    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - glm::pi<float>() / 2.0f),
        0,
        cos(horizontalAngle - glm::pi<float>() / 2.0f)
    );

    // Up vector (perpendicular to both direction and right)
    glm::vec3 up = glm::cross(right, direction);

    // Field of View
    float FoV = initialFoV;

    // Projection matrix
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);

    // Camera matrix
    ViewMatrix = glm::lookAt(
        position,  // Camera position
        glm::vec3(0, 0, 0), // Look at the origin
        up         // Head is up
    );

    // Update last time for the next frame
    lastTime = currentTime;
}

// create a simple texture to use for the chess pieces since the bmps do not want to play nice with me
GLuint createSimpleTexture(uint8_t R, uint8_t G, uint8_t B, uint8_t A) {
    GLuint dummyTexture;
    glGenTextures(1, &dummyTexture);
    glBindTexture(GL_TEXTURE_2D, dummyTexture);

    unsigned char whitePixel[4] = {R, G, B, A}; // RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

    // Texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return dummyTexture;
}

// draw all of the chess pieces. will also elevate the knight if it is moving
void drawChessPiece(GLuint matID, GLuint mmID, GLuint vmID, glm::mat4 projMat, glm::mat4 viewMat, GLuint textureID, ChessPiece& piece)
{
    if(piece.isVisible())
    {
        auto ri = piece.getRenderInfo();
        // first, bind texture

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ri.texture);
        glUniform1i(textureID, 2);

        // next, handle scaling and translation w.r.t board position

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        // scale
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
        // translate.
        /////
        // scale, translate and rotate the board coordinates into render coords: //
        /////
        auto pos = piece.getCurrentPosition();


    /// where the handling of the elevation of the knight is handled
        float z = 0.0f;
        if(piece.isMoving() && piece.isKnight())
        {
            z = 0.7f;
        }
        glm::vec3 originalVector(pos.x, z, pos.y);
        // Translation vector
        glm::vec3 translation(6.0f, 0.0f, 6.0f);
        // Scaling factor
        glm::vec3 scaleFactors(1.7f);
        // Rotation angle (in radians) and axis
        float rotationAngle = glm::radians(180.0f); // 45 degrees
        glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);  // Rotate around the Y-axis

        // Translation matrix
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation);

        // Scaling matrix
        glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), scaleFactors);

        // Rotation matrix
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, rotationAxis);

        // Combined transformation matrix (order matters: scale -> rotate -> translate)
        glm::mat4 transformationMatrix = translationMatrix * rotationMatrix * scalingMatrix;
        glm::vec4 transformedVector = transformationMatrix * glm::vec4(originalVector, 1.0f);

        // Back to vec3 (ignore w component)
        glm::vec3 result(transformedVector);
        /////
        /////
        /////

        modelMatrix = glm::translate(modelMatrix, result);
        glm::mat4 modelMVP = projMat * viewMat * modelMatrix;

        glUniformMatrix4fv(matID, 1, GL_FALSE, &modelMVP[0][0]);
        glUniformMatrix4fv(mmID, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(vmID, 1, GL_FALSE, &viewMat[0][0]);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, ri.vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, ri.uvbuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, ri.normalbuffer);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ri.elementbuffer);
        glDrawElements(GL_TRIANGLES, ri.indicesSize, GL_UNSIGNED_SHORT, (void*)0);

        // Disable vertex arrays after drawing
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

}

// load the chess piceces and append to the vector of meshes that exist
std::vector<ChessPiece> loadChessPieces(std::vector<Mesh>& meshes, GLuint whiteTexture, GLuint blackTexture)
{
    loadobjfile("data/chess-mod.obj", meshes);

    std::vector<ChessPiece> chessPieces;
    for (size_t i = 0; i < meshes.size(); i++) {

        switch (i)
        {
            case 0: // bishop
            {
                ChessPiece::RenderInfo ri;
                ri.vertexBuffer = meshes[i].vertexbuffer;
                ri.uvbuffer = meshes[i].uvbuffer;
                ri.normalbuffer = meshes[i].normalbuffer;
                ri.elementbuffer = meshes[i].elementbuffer;
                ri.indicesSize = meshes[i].indices.size();
                ri.texture= whiteTexture;

                chessPieces.emplace_back(ChessPiece::ChessPieceType::BISHOP, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(2, 0), "f1");
                chessPieces.emplace_back(ChessPiece::ChessPieceType::BISHOP, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(5, 0), "c1");
                ri.texture = blackTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::BISHOP, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(2, 7), "f8");
                chessPieces.emplace_back(ChessPiece::ChessPieceType::BISHOP, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(5, 7), "c8");
                break;
            }
            case 2: // knight
            {
                ChessPiece::RenderInfo ri;
                ri.vertexBuffer = meshes[i].vertexbuffer;
                ri.uvbuffer = meshes[i].uvbuffer;
                ri.normalbuffer = meshes[i].normalbuffer;
                ri.elementbuffer = meshes[i].elementbuffer;
                ri.indicesSize = meshes[i].indices.size();
                ri.texture= whiteTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::KNIGHT, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(1, 0), "g1");
                chessPieces.emplace_back(ChessPiece::ChessPieceType::KNIGHT, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(6, 0), "b1");
                ri.texture = blackTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::KNIGHT, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(1, 7), "g8");
                chessPieces.emplace_back(ChessPiece::ChessPieceType::KNIGHT, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(6, 7), "b8");
                break;
            }
            case 4: // pawn
            {
                ChessPiece::RenderInfo ri;
                ri.vertexBuffer = meshes[i].vertexbuffer;
                ri.uvbuffer = meshes[i].uvbuffer;
                ri.normalbuffer = meshes[i].normalbuffer;
                ri.elementbuffer = meshes[i].elementbuffer;
                ri.indicesSize = meshes[i].indices.size();
                char mapping[] = {'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
                for(int i =0; i<8; i++)
                {
                    ri.texture= whiteTexture;
                    chessPieces.emplace_back(ChessPiece::ChessPieceType::PAWN, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(i, 1), std::string(1, mapping[i])+"2");
                    ri.texture= blackTexture;
                    chessPieces.emplace_back(ChessPiece::ChessPieceType::PAWN, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(i, 6), std::string(1, mapping[i])+"7");
                }
                break;
            }
            case 6: // king
            {
                ChessPiece::RenderInfo ri;
                ri.vertexBuffer = meshes[i].vertexbuffer;
                ri.uvbuffer = meshes[i].uvbuffer;
                ri.normalbuffer = meshes[i].normalbuffer;
                ri.elementbuffer = meshes[i].elementbuffer;
                ri.indicesSize = meshes[i].indices.size();
                ri.texture= whiteTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::KING, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(3, 0), "e1");
                ri.texture= blackTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::KING, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(3, 7), "e8");
                break;
            }
            case 8: // queen
            {
                ChessPiece::RenderInfo ri;
                ri.vertexBuffer = meshes[i].vertexbuffer;
                ri.uvbuffer = meshes[i].uvbuffer;
                ri.normalbuffer = meshes[i].normalbuffer;
                ri.elementbuffer = meshes[i].elementbuffer;
                ri.indicesSize = meshes[i].indices.size();
                ri.texture= whiteTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::QUEEN, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(4, 0), "d1");
                ri.texture= blackTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::QUEEN, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(4, 7), "d8");
                break;
            }
            case 10: // rook
            {
                ChessPiece::RenderInfo ri;
                ri.vertexBuffer = meshes[i].vertexbuffer;
                ri.uvbuffer = meshes[i].uvbuffer;
                ri.normalbuffer = meshes[i].normalbuffer;
                ri.elementbuffer = meshes[i].elementbuffer;
                ri.indicesSize = meshes[i].indices.size();
                ri.texture= whiteTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::ROOK, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(0, 0), "h1");
                chessPieces.emplace_back(ChessPiece::ChessPieceType::ROOK, ChessPiece::Color::WHITE, ri, ChessPiece::BoardPos(7, 0), "a1");
                ri.texture= blackTexture;
                chessPieces.emplace_back(ChessPiece::ChessPieceType::ROOK, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(0, 7), "h8");
                chessPieces.emplace_back(ChessPiece::ChessPieceType::ROOK, ChessPiece::Color::BLACK, ri, ChessPiece::BoardPos(7, 7), "a8");
                break;
            } 
        }
    }
    return chessPieces;
}

// main function 
int main(void) {

    ECE_ChessEngine backend;
    backend.InitializeEngine();
    backend.sendMove("uci");
    backend.sendMove("isready");
    std::string moves = "";  // This will hold the list of moves made so far

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "Chess 3d", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders("data/StandardShading.vertexshader", "data/StandardShading.fragmentshader");

    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    chessComponent gchessComponent;
    bool cBoard = loadAssImpLab3("data/12951_Stone_Chess_Board_v1_L3.obj", gchessComponent);

    gchessComponent.setupGLBuffers();
    gchessComponent.setupTextureBuffers();

    GLuint whiteTexture = createSimpleTexture(255, 255, 255, 255); // Use dummy texture
    GLuint blackTexture = createSimpleTexture(0, 0, 0, 100); // Use dummy texture
    GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

    std::vector<Mesh> meshes;
    auto chessPieces = loadChessPieces(meshes, whiteTexture, blackTexture);
    
    ChessStateManager chessManager(chessPieces);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    float lightPower = 75.0f;
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    // Set the light color
    GLint LightColorID = glGetUniformLocation(programID, "LightColor");
    
    // Set the light power
    GLint LightPowerID = glGetUniformLocation(programID, "LightPower");
    
    std::thread commandThread(getUserCommandThread);

    // this holds the calculated move from the chess engine while the user pieces are moving
    std::string engine_move_in_waiting = "";
    computeMatricesFromInputs(std::nullopt);
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);
        
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);


        glUniform3f(LightColorID, lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
        glUniform1f(LightPowerID, lightPower); 

        // chessPieces.at(0).setGoalPosition({3,3});
        for (auto & chesspiece : chessPieces)
        {
            // the draw command has the chess piece ref pass through and it calls the getCurrentPosition function, 
            // during which the new position for the chess piece is calculated
            drawChessPiece(MatrixID, ModelMatrixID, ViewMatrixID, ProjectionMatrix, ViewMatrix, TextureID, chesspiece);
        }
        
        // handle chess board manipulation and draw it
        tPosition cTPositionMorph;
        cTPositionMorph.rDis = 0.0;
        cTPositionMorph.rAngle = -90.0f;
        cTPositionMorph.rAxis = {1, 0, 0};
        cTPositionMorph.cScale = glm::vec3(0.03);
        cTPositionMorph.tPos = {0.f,     -0.05f, 0.f}; 
        // Pass it for Model matrix generation
        glm::mat4 mm = gchessComponent.genModelMatrix(cTPositionMorph);
        // Genrate the MVP matrix
        glm::mat4 mvpCb = ProjectionMatrix * ViewMatrix * mm;

        // Send our transformation to the currently bound shader
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvpCb[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &mm[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        gchessComponent.setupTexture(TextureID);
        gchessComponent.renderMesh();
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        if (newCommandAvailable) {
            std::lock_guard<std::mutex> lock(cmdMutex); // Lock for thread-safe access
            if (cmd) {
                if (cmd->type == cmdType::LIGHT) {
                    auto coords = std::get<0>(*(cmd->cmdVerb));
                    lightPos = computeNewLightPosFromCoords(coords);
                }
                if (cmd->type == cmdType::POWER) {
                    lightPower = std::get<1>(*(cmd->cmdVerb));
                }
                if(cmd->type == cmdType::MOVE)
                {
                    auto move = std::get<2>(*(cmd->cmdVerb));
                    if(chessManager.validateMove(move))
                    {
                        std::string out = "";
                        bool engineError = false;
                        moves += " " + move;
                        std::cout << moves <<std::endl;
                        backend.sendMove(std::string("position startpos moves") + moves);
                        // Request engine's best move
                        backend.sendMove("go depth 1");
                        while ( (out.find("bestmove")) == std::string::npos) {
                            (void)backend.getResponseMove(out);
                            std::cout << out <<std::endl;
                            if (out.find("mate") != std::string::npos) {
                                if (out.find("mate") != std::string::npos) {
                                    size_t matePos = out.find("mate");
                                    int mateValue = std::stoi(out.substr(matePos + 5));
                                    if (mateValue > 0) {
                                        std::cout << "Checkmate you lose!!" << std::endl;
                                        return 0;
                                    } else if (mateValue < 0) {
                                        std::cout << "Checkmate you win!!" << std::endl;
                                        return 0;
                                    }
                                }
                            }

                            if(out.find("Cannot execute move") != std::string::npos)
                            {
                                engineError = true;
                                moves.erase(moves.size() - 5); 
                                break;
                            }
                        } 

                        if((!engineError) && chessManager.applyMove(move))
                        {
                            // Extract the best move from the engine's response
                            auto best_move = out.substr(out.find("bestmove") + 9, 4);
                            std::cout << "Engine's best move: " << best_move << std::endl;
                            moves += " " + best_move;
                            // engine_move_in_waiting = best_move;
                            chessManager.applyMove(best_move);

                        } else {
                            std::cout << "erm" <<std::endl;
                        }
                    } else {
                        std::cout << "Invalid command or move!!" <<std::endl;
                    }
                    
                }
                
                if(cmd->type == cmdType::QUIT)
                {
                    std::cout << "Thanks for playing!!" <<std::endl;
                    return 0;
                }
            }


            computeMatricesFromInputs(cmd);        
            
            newCommandAvailable = false; // Reset the flag after processing

        }
    
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    glDeleteProgram(programID);
    glDeleteTextures(1, &whiteTexture);
    glDeleteTextures(1, &blackTexture);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
    return 0;
}
