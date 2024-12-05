#include <baseoglmanaging.hpp>
#include <shader.hpp>

std::optional<OpenGLRuntimeParams> setupScene()
{
    OpenGLRuntimeParams params;
    // init GLFW library for some reason
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return std::nullopt;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    params.current_window = glfwCreateWindow( 1024, 768, "Chess 3D", NULL, NULL);

    if( params.current_window == nullptr ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version.\n" );
        getchar();
        glfwTerminate();
        return std::nullopt;
    }

    glfwMakeContextCurrent(params.current_window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return std::nullopt;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(params.current_window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(params.current_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwPollEvents();
    glfwSetCursorPos(params.current_window, 1024/2, 768/2);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS); 
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    params.programID = LoadShaders( "data/StandardShading.vertexshader", "data/StandardShading.fragmentshader" );

    // Get a handle for our "MVP" uniform
    params.matrixID = glGetUniformLocation(params.programID, "MVP");
    params.viewMatrixID = glGetUniformLocation(params.programID, "V");
    params.modelMatrixID = glGetUniformLocation(params.programID, "M");

    // Get a handle for our "myTextureSampler" uniform
    params.textureID  = glGetUniformLocation(params.programID, "myTextureSampler");

    // Get a handle for our "lightToggleSwitch" uniform
    params.lightSwitchID = glGetUniformLocation(params.programID, "lightSwitch");


    return params;
}

void destroyScene(OpenGLRuntimeParams &curParams)
{

}