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


View::View() : isDragging(false), rotationMatrix(glm::mat4(1.0f)) {

}

View::~View(){

}

void View::init(Callbacks *callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes) 
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

    //using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window, 
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onkey(key,scancode,action,mods);
    });

    glfwSetMouseButtonCallback(window,
    [](GLFWwindow *window, int button, int action, int mods)
    {
        reinterpret_cast<View *>(glfwGetWindowUserPointer(window))->onMousePress(button, action, mods);
        });
        
    //Call our mouse movement function
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
    
    
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
    
	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    frames = 0;
    time = glfwGetTime();

    renderer = new sgraph::GLScenegraphRenderer(modelview,objects,shaderLocations);
    renderer_text = new sgraph::GLScenegraphTextRenderer(0);

    counter = 0;
    
}




void View::display(sgraph::IScenegraph *scenegraph) {
    
    program.enable();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT_FACE);

    float rotation_time = 25.0 * glfwGetTime();
    
    modelview.push(glm::mat4(1.0));
    glm::mat4 viewMatrix = glm::lookAt(glm::vec3(200.0f,250.0f,250.0f),glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
    modelview.top() = viewMatrix * rotationMatrix;
    
    
    // ROTATION
    if (isRotating) {
        modelview.top() = modelview.top() * glm::rotate(glm::mat4(1.0f), glm::radians(rotation_time), glm::vec3(0.0f, 1.0f, 0.0f));
    }
    
    
    //send projection matrix to GPU    
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    

    //draw scene graph here
    scenegraph->getRoot()->accept(renderer);

    // only output once text renderer to fix infinite loop
    if (counter < 1) {
        scenegraph->getRoot()->accept(renderer_text);
        counter ++;
    }
    
    modelview.pop();
    glFlush();
    program.disable();
    
    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime-time)>1.0) {
        printf("Framerate: %2.0f\r",frames/(currenttime-time));
        frames = 0;
        time = currenttime;
    }
    

}
void View::resetRotation() {
    rotationMatrix = glm::mat4(1.0f);
}

//Keeps track of mouse clicking for storing click positions
void View::onMousePress(int button, int action, int mods) {
    if(button==GLFW_MOUSE_BUTTON_LEFT){
        if(action==GLFW_PRESS){
            isDragging=true;
            double x;
            double y;
            glfwGetCursorPos(window, &x, &y);
            lastPos=glm::vec2(x,y);
        }else if(action==GLFW_RELEASE){
            isDragging=false;
        }
    }
}

//Updates the rotated matrix while the Trackball is being used
void View::onMouseMove(double posX, double posY){
    if(isDragging){
        glm::vec2 currPos(posX, posY);
        glm::vec2 delta=currPos-lastPos;
        lastPos =currPos;
        float rotX = glm::radians(delta.y*0.2f);
        float rotY = glm::radians(delta.x*0.2f);
        glm::mat4 rotationX=glm::rotate(glm::mat4(1.0f),rotX,glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationY=glm::rotate(glm::mat4(1.0f),rotY, glm::vec3(0.0f, 1.0f, 0.0f));
        rotationMatrix=rotationY*rotationX*rotationMatrix;
    }
}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

void View::rotate() {
    isRotating = true;
}

void View::stopRotate() {
    isRotating = false;
}

void View::closeWindow() {
    for (map<string,util::ObjectInstance *>::iterator it=objects.begin();
           it!=objects.end();
           it++) {
          it->second->cleanup();
          delete it->second;
    } 
    glfwDestroyWindow(window);

    glfwTerminate();
}




