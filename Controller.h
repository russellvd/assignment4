#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "View.h"
#include "Model.h"
#include "Callbacks.h"

class Controller: public Callbacks
{
public:
    Controller(Model& m,View& v, const std::string& filePath);
    ~Controller();
    void run();

    virtual void reshape(int width, int height);
    virtual void dispose();
    virtual void onkey(int key, int scancode, int action, int mods);
    virtual void error_callback(int error, const char* description);
    virtual void onMouse(int button, int action, int mods);

private:
    void initScenegraph();

    View view;
    Model model;

    void initScenegraph(const std::string& filePath);
    bool mouseReleased = true;
    bool rotateFaster = false;
};

#endif