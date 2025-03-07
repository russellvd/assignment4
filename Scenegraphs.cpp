#include <iostream>
#include <string>
#include "View.h"
#include "Model.h"
#include "Controller.h"

int main(int argc, char* argv[]) {
    // Check if the user provided a file path as a command-line argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path to scenegraph file>" << std::endl;
        return EXIT_FAILURE;
    }

    // the first argument (argv[1]) will be the path to the scenegraph file
    std::string filePath = argv[1];

    Model model;
    View view;
    Controller controller(model, view, filePath);  // pass file path to the controller
    controller.run();

    return 0;
}
