#ifndef _GLSCENEGRAPHTEXTRENDERER_H_
#define _GLSCENEGRAPHTEXTRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <iostream>
#include <string>

namespace sgraph {

    /**
     * This visitor implements a textual rendering of the scene graph as specified in the starter code.
     * 
     */
    class GLScenegraphTextRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new GLScenegraphTextRenderer object
         * 
         * @param indentLevel initial indentation level for rendering
         */
        GLScenegraphTextRenderer(int indentLevel = 0) 
            : indentLevel(indentLevel) {}

        /**
         * @brief Recur to the children for rendering
         * 
         * @param groupNode 
         */
        void visitGroupNode(GroupNode *groupNode) {
            printNodeName(groupNode->getName());
            for (int i = 0; i < groupNode->getChildren().size(); i++) {
                GLScenegraphTextRenderer childRenderer(indentLevel + 1);
                groupNode->getChildren()[i]->accept(&childRenderer);
            }
        }

        /**
         * @brief Print the name of the leaf node to console.
         * 
         * @param leafNode 
         */
        void visitLeafNode(LeafNode *leafNode) {
            printNodeName(leafNode->getName());
        }

        /**
         * @brief Print the name of the transform node and recur to child
         * 
         * @param transformNode 
         */
        void visitTransformNode(TransformNode *transformNode) {
            printNodeName(transformNode->getName());
            if (transformNode->getChildren().size() > 0) {
                GLScenegraphTextRenderer childRenderer(indentLevel + 1);
                transformNode->getChildren()[0]->accept(&childRenderer);
            }
        }

        /**
         * @brief delegate to visitTransformNode above
         * 
         * @param scaleNode 
         */
        void visitScaleTransform(ScaleTransform *scaleNode) {
            visitTransformNode(scaleNode);
        }

        /**
         * @brief delegate to visitTransformNode above
         * 
         * @param translateNode 
         */
        void visitTranslateTransform(TranslateTransform *translateNode) {
            visitTransformNode(translateNode);
        }

        /**
         * @brief delegate to visitTransformNode above
         * 
         * @param rotateNode 
         */
        void visitRotateTransform(RotateTransform *rotateNode) {
            visitTransformNode(rotateNode);
        }

        private:
        int indentLevel;

        /**
         * @brief Helper to print the node name with appropriate indentation
         * 
         * @param name 
         */
        void printNodeName(const std::string& name) {
            for (int i = 0; i < indentLevel; i++) {
                std::cout << "  ";
            }
            std::cout << "- " << name << std::endl;
        }
    };
}

#endif