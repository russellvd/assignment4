#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/GLScenegraphTextMapRenderer.h"
#include "VertexAttrib.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

View::View() : isDragging(false), rotationMatrix(glm::mat4(1.0f))
{
}

View::~View()
{
}

void View::init(Callbacks *callbacks, map<string, util::PolygonMesh<VertexAttrib>> &meshes)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(1600, 1600, "Hello GLFW: Per-vertex coloring", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowUserPointer(window, (void *)this);

    // using C++ functions as callbacks to a C-style library
    // using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window,
                       [](GLFWwindow *window, int key, int scancode, int action, int mods)
                       {
                           reinterpret_cast<View *>(glfwGetWindowUserPointer(window))->onkey(key, scancode, action, mods);
                       });

    glfwSetMouseButtonCallback(window,
                               [](GLFWwindow *window, int button, int action, int mods)
                               {
                                   reinterpret_cast<View *>(glfwGetWindowUserPointer(window))->onMousePress(button, action, mods);
                               });

    // Call our mouse movement function
    glfwSetCursorPosCallback(window,
                             [](GLFWwindow *window, double posX, double posY)
                             {
                                 reinterpret_cast<View *>(glfwGetWindowUserPointer(window))->onMouseMove(posX, posY);
                             });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // create the shader program
    program.createProgram(string("shaders/default.vert"),
                          string("shaders/default.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    /* In the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.

       We create such a shader variable -> vertex attribute mapping now
     */
    map<string, string> shaderVarsToVertexAttribs;

    shaderVarsToVertexAttribs["vPosition"] = "position";

    for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
         it != meshes.end();
         it++)
    {
        util::ObjectInstance *obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs, it->second);
        objects[it->first] = obj;
    }

    int window_width, window_height;
    glfwGetFramebufferSize(window, &window_width, &window_height);

    // prepare the projection matrix for perspective projection
    projection = glm::perspective(glm::radians(60.0f), (float)window_width / window_height, 0.1f, 10000.0f);
    glViewport(0, 0, window_width, window_height);

    frames = 0;
    time = glfwGetTime();

    renderer = new sgraph::GLScenegraphRenderer(modelview, objects, shaderLocations);
    renderer_text = new sgraph::GLScenegraphTextRenderer(0);

    counter = 0;
    droneSpeed = .12f;
    droneMovementSpeed = 3.0f;
    
}


void View::animateDroneMovement(sgraph::IScenegraph *scenegraph, const std::string &nodeName, float distance, int axis)
{
    sgraph::SGNode *translateBoxNode = scenegraph->getRoot()->getNode(nodeName);
    sgraph::TranslateTransform *translateNode = dynamic_cast<sgraph::TranslateTransform*>(translateBoxNode);
    sgraph::RotateTransform *rotateNode = dynamic_cast<sgraph::RotateTransform*>(scenegraph->getRoot()->getNode("rotate-drone")); 
    Direction currentDirection = rotateNode->getCurrentDirection();
    glm::vec3 newTranslate = translateNode->getTranslate();
    float newDistance = distance * droneSpeed;

    // adjust translation based on the current facing direction
    switch (currentDirection) {
        case Direction::FORWARD:
            newTranslate.z += newDistance;
            break;
        case Direction::BEHIND:
            newTranslate.z -= newDistance;
            break;
        case Direction::LEFT:
            newTranslate.x += newDistance;
            break;
        case Direction::RIGHT:
            newTranslate.x -= newDistance;
            break;
        case Direction::UP:
            newTranslate.y += newDistance;
            break;
        case Direction::DOWN:
            newTranslate.y -= newDistance;
            break;
    }
    translateNode->updateTranslation(newTranslate);
        if(cameraMode==FP){
        }
}


void View::animateTurns(sgraph::IScenegraph *scenegraph, const std::string &nodeName, const std::string &direction)
{
    sgraph::SGNode *rotateBoxNode = scenegraph->getRoot()->getNode(nodeName);
    sgraph::RotateTransform *rotateNode = dynamic_cast<sgraph::RotateTransform*>(rotateBoxNode);
    if (direction == "up") {
        rotateNode->turnUp(90.0f);
        turnUp = false;
    } else if (direction == "down") {
        rotateNode->turnDown(90.0f);
        turnDown = false;
    } else if (direction == "left") {
        rotateNode->turnLeft(90.0f);
        turnLeft = false;
    } else if (direction == "right") {
        rotateNode->turnRight(90.0f);
        turnRight = false;
    } else {
        std::cerr << "Invalid direction: " << direction << std::endl;
    }
    rotateNode->printDirection();
}

void View::animateDroneRoll(sgraph::IScenegraph *scenegraph, const std::string &nodeName, float deltaAngle)
{
    sgraph::SGNode *rotateBoxNode = scenegraph->getRoot()->getNode(nodeName);
    sgraph::RotateTransform *rotateNode = dynamic_cast<sgraph::RotateTransform*>(rotateBoxNode);
    float currentAngle = rotateNode->getAngleInRadians();
    float newAngle = currentAngle + deltaAngle;
    if (newAngle >= 2 * M_PI) {
        newAngle = fmod(newAngle, 2 * M_PI);  // reset the angle after full rotation
        isRolling = false; 
    }
    rotateNode->updateRotation(newAngle);
}

void View::animatePropellerRotation(sgraph::IScenegraph *scenegraph, const std::string &nodeName, float deltaAngle)
{
    sgraph::SGNode *rotateBoxNode = scenegraph->getRoot()->getNode(nodeName);
    sgraph::RotateTransform *rotateNode = dynamic_cast<sgraph::RotateTransform *>(rotateBoxNode);
    float newAngle = rotateNode->getAngleInRadians() + deltaAngle;
    rotateNode->updateRotation(newAngle); 
}

void View::resetDrone(sgraph::IScenegraph *scenegraph)
{
    sgraph::SGNode *translateBoxNode = scenegraph->getRoot()->getNode("translate-drone");
    sgraph::TranslateTransform *translateNode = dynamic_cast<sgraph::TranslateTransform*>(translateBoxNode);
    glm::vec3 originalPosition = glm::vec3(0.0f, 0.0f, 0.0f); 
    translateNode->updateTranslation(originalPosition);
    sgraph::SGNode *rotateBoxNode = scenegraph->getRoot()->getNode("rotate-drone");
    sgraph::RotateTransform *rotateNode = dynamic_cast<sgraph::RotateTransform*>(rotateBoxNode);
    float originalRotationAngle = 0.0f;  
    glm::vec3 originalRotationAxis = glm::vec3(0.0f, 1.0f, 0.0f); 
    rotateNode->updateRotation(originalRotationAngle);
    reset = false;
}


// animate all the propellers (doing this to remove too much mess in the render function) 
void View:: animateDronePropellers(sgraph::IScenegraph *scenegraph, float deltaAngle) 
{
    // left propellers
    animatePropellerRotation(scenegraph, "rotate-lp1", deltaAngle);
    animatePropellerRotation(scenegraph, "rotate-lp2", deltaAngle);
    animatePropellerRotation(scenegraph, "rotate-lp3", deltaAngle);
    animatePropellerRotation(scenegraph, "rotate-lp4", deltaAngle);

    // right propellers
    animatePropellerRotation(scenegraph, "rotate-rp1", deltaAngle);
    animatePropellerRotation(scenegraph, "rotate-rp2", deltaAngle);
    animatePropellerRotation(scenegraph, "rotate-rp3", deltaAngle);
    animatePropellerRotation(scenegraph, "rotate-rp4", deltaAngle);
}


void View::display(sgraph::IScenegraph *scenegraph)
{

    program.enable();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT_FACE);

    float rotation_time = 25.0 * glfwGetTime();

    modelview.push(glm::mat4(1.0));
    glm::mat4 viewMatrix;
    switch (cameraMode){
        case GLOBAL:
            //Global view of the entire scene
            viewMatrix = glm::lookAt(glm::vec3(200.0f, 250.0f, 250.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        case CHOPPER:{
            //Chopper cam circling around the scene
            float time=glfwGetTime();
            float radius=300.0f;
            float camX=radius *cos(time*0.5f);
            float camZ = radius * sin(time*0.5f);
            viewMatrix = glm::lookAt(glm::vec3(camX, 250.0f, camZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        }
        case FP:{
            sgraph::SGNode *droneNode=scenegraph->getRoot()->getNode("translate-drone");
            sgraph::TranslateTransform *droneTransform = dynamic_cast<sgraph::TranslateTransform*>(droneNode);
            glm::vec3 dronePos=droneTransform->getTranslate();
            sgraph::SGNode *droneRotNode =scenegraph->getRoot()->getNode("rotate-drone");
            sgraph::RotateTransform *droneRot=dynamic_cast<sgraph::RotateTransform*>(droneRotNode);

            glm::vec3 cameraPosition=dronePos + glm::vec3(0.0f, 5.0f, 0.0f);
            // viewMatrix = glm::lookAt(cameraPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));
            break;
        }
    }
    modelview.top() = viewMatrix * rotationMatrix;

    // ROTATION
    if (isRotating)
    {
        modelview.top() = modelview.top() * glm::rotate(glm::mat4(1.0f), glm::radians(rotation_time), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // send projection matrix to GPU
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Propeller Animation
    animateDronePropellers(scenegraph, droneSpeed);
    
    // Drone Roll Animation 
    if (isRolling) {
        animateDroneRoll(scenegraph, "rotate-drone", .06f);
    }

    // Drone Movement Animation
    if (isMovingForward) {
        animateDroneMovement(scenegraph, "translate-drone", droneMovementSpeed, 2);
    }

    // Drone Movement Animation
    if (isMovingBackward) {
        animateDroneMovement(scenegraph, "translate-drone", -droneMovementSpeed, 2);
    }

    // Turning Left
    if (turnLeft) {
        animateTurns(scenegraph, "rotate-drone", "left");
    }

    // Turning Right
    if (turnRight) {
        animateTurns(scenegraph, "rotate-drone", "right");
    }

    // Turning Up
    if (turnUp) {
        animateTurns(scenegraph, "rotate-drone", "up");
    }

    // Turning Down
    if (turnDown) {
        animateTurns(scenegraph, "rotate-drone", "down");
    }

    // Reset Drone
    if (reset) {
        resetDrone(scenegraph);
    }


    // draw scene graph here
    scenegraph->getRoot()->accept(renderer);

    // only output once text renderer to fix infinite loop
    if (counter < 1)
    {
        scenegraph->getRoot()->accept(renderer_text);
        counter++;
    }

    modelview.pop();
    glFlush();
    program.disable();

    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime - time) > 1.0)
    {
        printf("Framerate: %2.0f\r", frames / (currenttime - time));
        frames = 0;
        time = currenttime;
    }
}



void View::resetRotation()
{
    rotationMatrix = glm::mat4(1.0f);
}

void View::onkey(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_R:
            cout << "r pressed" << endl;
            resetRotation();
            break;
        case GLFW_KEY_F:
            cout << "f pressed" << endl;
            droneSpeed += 0.04f; // increase the speed
            droneMovementSpeed += 1.0f;
            break;
        case GLFW_KEY_S:
            cout << "s pressed" << endl;
            if (droneSpeed == 0.04f) { // if the speed is at minimum, do not decrease
                droneSpeed = .04f;
                droneMovementSpeed = 1.0f;
                break;
            }
            droneSpeed -= 0.04f; // decrease the speed
            droneMovementSpeed -= 1.0f; 
            break;
        case GLFW_KEY_J:
            isRolling = true;
            break;
        case GLFW_KEY_MINUS:
            isMovingBackward = true;
            isMovingForward = false;
            break;
        case GLFW_KEY_LEFT:
            cout << "LEFT pressed" << endl;
            turnLeft = true;
            break;
        case GLFW_KEY_RIGHT:
            cout << "RIGHT pressed" << endl;
            turnRight = true;
            break;
        case GLFW_KEY_DOWN:
            cout << "DOWN pressed" << endl;
            turnDown = true;
            break;
        case GLFW_KEY_UP:
            cout << "UP pressed" << endl;
            turnUp = true;
            break;
        case GLFW_KEY_D:
            reset = true;
            break;
        case GLFW_KEY_1:
            cout << "Switching to Global Camera" << endl;
            cameraMode =GLOBAL;
            break;
        case GLFW_KEY_2:
            cout << "Switching to Chopper Camera" << endl;
            cameraMode =CHOPPER;
            break;
        case GLFW_KEY_3:
            cout << "Switching to First-Person Camera" << endl;
            cameraMode =FP;
            break;
        default:
            cout << "Unhandled key press: " << key << endl;
            break;
        }
        // because '+' requires two inputs 
        if ((key == GLFW_KEY_EQUAL) && ((mods & GLFW_MOD_SHIFT) != 0)) {
            isMovingForward = true;
            isMovingBackward = false;
        }
    }

    cout << (char)key << " pressed" << endl;
}

// Keeps track of mouse clicking for storing click positions
void View::onMousePress(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            isDragging = true;
            double x;
            double y;
            glfwGetCursorPos(window, &x, &y);
            lastPos = glm::vec2(x, y);
        }
        else if (action == GLFW_RELEASE)
        {
            isDragging = false;
        }
    }
}

// Updates the rotated matrix while the Trackball is being used
void View::onMouseMove(double posX, double posY)
{
    if (isDragging)
    {
        glm::vec2 currPos(posX, posY);
        glm::vec2 delta = currPos - lastPos;
        lastPos = currPos;
        float rotX = glm::radians(delta.y * 0.2f);
        float rotY = glm::radians(delta.x * 0.2f);
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), rotX, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix = rotationY * rotationX * rotationMatrix;
    }
}

bool View::shouldWindowClose()
{
    return glfwWindowShouldClose(window);
}

void View::rotate()
{
    isRotating = true;
}

void View::stopRotate()
{
    isRotating = false;
}

void View::closeWindow()
{
    for (map<string, util::ObjectInstance *>::iterator it = objects.begin();
         it != objects.end();
         it++)
    {
        it->second->cleanup();
        delete it->second;
    }
    glfwDestroyWindow(window);

    glfwTerminate();
}