#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stack>
using namespace std;


class View
{
public:
    void onMousePress(int button, int action, int mods);
    void onMouseMove(double posX, double posY);
    void onkey(int key, int scancode, int action, int mods);
    void resetRotation();
    View();
    ~View();
    void init(Callbacks* callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes);
    void display(sgraph::IScenegraph *scenegraph);
    bool shouldWindowClose();
    void rotate();
    void stopRotate();
    void closeWindow();
    void animatePropellerRotation(sgraph::IScenegraph *scenegraph, const std::string &nodeName, float deltaAngle);
    void animateDronePropellers(sgraph::IScenegraph *scenegraph, float deltaAngle);
    void animateDroneRoll(sgraph::IScenegraph *scenegraph, const std::string &nodeName, float deltaAngle);
    void animateDroneMovement(sgraph::IScenegraph *scenegraph, const std::string &nodeName, float distance, int axis);
    void animateTurns(sgraph::IScenegraph *scenegraph, const std::string &nodeName, const std::string &direction);
    void resetDrone(sgraph::IScenegraph *scenegraph);

private: 

    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string,util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    sgraph::SGNodeVisitor *renderer_text;
    int frames;
    double time;
    double deltaTime;
    bool isRotating = false;
    int counter;
    glm::vec2 lastPos;
    bool isDragging= false;
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    float droneSpeed;
    float droneMovementSpeed;
    enum CameraMode{GLOBAL,CHOPPER,FP};
    CameraMode cameraMode =GLOBAL;
    float chopperAngle = 0.0f;
    float chopperRadius = 400.0f;
    

    bool isRolling = false;
    bool isMovingForward = false;
    bool isMovingBackward = false;
    bool turnLeft = false;
    bool turnRight = false;
    bool turnUp = false;
    bool turnDown = false;
    bool reset = false;

};

#endif