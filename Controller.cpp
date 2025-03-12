#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"

Controller::Controller(Model &m, View &v, const std::string &filePath)
{
    model = m;
    view = v;
    initScenegraph(filePath);
}

void Controller::initScenegraph(const std::string &sceneFile)
{
    ifstream inFile(sceneFile);
    if (!inFile)
    {
        cerr << "Error: Could not open scene file: " << sceneFile << endl;
        exit(EXIT_FAILURE);
    }

    sgraph::ScenegraphImporter importer;
    IScenegraph *scenegraph = importer.parse(inFile);

    if (!scenegraph)
    {
        cerr << "Error: Scenegraph is NULL after parsing!" << endl;
        exit(EXIT_FAILURE);
    }

    if (!scenegraph->getRoot())
    {
        cerr << "Error: Scenegraph root is NULL!" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Scenegraph loaded successfully. Root: " << scenegraph->getRoot()->getName() << endl;
    model.setScenegraph(scenegraph);
}

Controller::~Controller()
{
}

void Controller::run()
{
    sgraph::IScenegraph *scenegraph = model.getScenegraph();
    map<string, util::PolygonMesh<VertexAttrib>> meshes = scenegraph->getMeshes();
    view.init(this, meshes);
    while (!view.shouldWindowClose())
    {
        view.display(scenegraph);
        // view rotate
        if (!mouseReleased)
        {
            view.rotate();
        }
        else
        {
            view.stopRotate();
        }
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onMouse(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        cout << "left mouse pressed" << endl;
        mouseReleased = false;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        cout << "left mouse released" << endl;
        mouseReleased = true;
    }
}

void Controller::onkey(int key, int scancode, int action, int mods)
{
   cout << (char)key << " pressed" << endl;

}

void Controller::reshape(int width, int height)
{
    cout << "Window reshaped to width=" << width << " and height=" << height << endl;
    glViewport(0, 0, width, height);
}

void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}