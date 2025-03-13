#ifndef _ROTATETRANSFORM_H_
#define _ROTATETRANSFORM_H_

#include <iostream>
#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>

enum class Direction {
    FORWARD,
    LEFT,
    RIGHT,
    BEHIND,
    UP,
    DOWN
};

namespace sgraph {

/**
 * This class represents a specific type of transformation: rotation
 * 
 */
    class RotateTransform: public TransformNode {
        protected:
            float angleInRadians;
            glm::vec3 axis;
            Direction currentDirection;

            ParentSGNode *copyNode() {
                return new RotateTransform(angleInRadians,axis[0],axis[1],axis[2],name,scenegraph);
            }

        public:
            RotateTransform(float angleInRadians,float ax,float ay,float az,const string& name,sgraph::IScenegraph *graph) 
                :TransformNode(name,graph) {
                    this->angleInRadians = angleInRadians;
                    this->axis = glm::vec3(ax,ay,az);
                    glm::mat4 transform = glm::rotate(glm::mat4(1.0),this->angleInRadians,this->axis);
                    setTransform(transform);
            }


            /**
             * Visit this node.
             * 
             */
            void accept(SGNodeVisitor* visitor) {
                return visitor->visitRotateTransform(this);
            }

            glm::vec3 getRotationAxis() {
                return axis;
            }

            float getAngleInRadians() {
                return angleInRadians;
            }
            
            void updateRotation(float newAngleInRadians) {
                this->angleInRadians = newAngleInRadians;
                glm::mat4 transform = glm::rotate(glm::mat4(1.0), this->angleInRadians, this->axis);
                setTransform(transform); 
            }

            void turnLeft(float angleStep) {
                glm::mat4 currentTransform = getTransform();
                glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(angleStep), glm::vec3(0, 1, 0));
                glm::mat4 newTransform = rotation * currentTransform;
                setTransform(newTransform);
                
                // Update direction
                if (currentDirection == Direction::FORWARD)
                    currentDirection = Direction::LEFT;
                else if (currentDirection == Direction::LEFT)
                    currentDirection = Direction::BEHIND;
                else if (currentDirection == Direction::BEHIND)
                    currentDirection = Direction::RIGHT;
                else if (currentDirection == Direction::RIGHT)
                    currentDirection = Direction::FORWARD;
            }
    
            void turnRight(float angleStep) {
                glm::mat4 currentTransform = getTransform();
                glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(-angleStep), glm::vec3(0, 1, 0));
                glm::mat4 newTransform = rotation * currentTransform;
                setTransform(newTransform);
    
                // Update direction
                if (currentDirection == Direction::FORWARD)
                    currentDirection = Direction::RIGHT;
                else if (currentDirection == Direction::RIGHT)
                    currentDirection = Direction::BEHIND;
                else if (currentDirection == Direction::BEHIND)
                    currentDirection = Direction::LEFT;
                else if (currentDirection == Direction::LEFT)
                    currentDirection = Direction::FORWARD;
            }
    
            void turnUp(float angleStep) {
                glm::mat4 currentTransform = getTransform();
                glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(-angleStep), glm::vec3(1, 0, 0));
                glm::mat4 newTransform = rotation * currentTransform;
                setTransform(newTransform);
            
                // Update direction based on current state
                switch (currentDirection) {
                    case Direction::FORWARD:
                        currentDirection = Direction::UP;
                        break;
                    case Direction::UP:
                        currentDirection = Direction::BEHIND;
                        break;
                    case Direction::BEHIND:
                        currentDirection = Direction::DOWN;
                        break;
                    case Direction::DOWN:
                        currentDirection = Direction::FORWARD;
                        break;
                    default:
                        break;
                }
            }
            
            void turnDown(float angleStep) {
                glm::mat4 currentTransform = getTransform();
                glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(angleStep), glm::vec3(1, 0, 0));
                glm::mat4 newTransform = rotation * currentTransform;
                setTransform(newTransform);
            
                // Update direction based on current state
                switch (currentDirection) {
                    case Direction::FORWARD:
                        currentDirection = Direction::DOWN;
                        break;
                    case Direction::DOWN:
                        currentDirection = Direction::BEHIND;
                        break;
                    case Direction::BEHIND:
                        currentDirection = Direction::UP;
                        break;
                    case Direction::UP:
                        currentDirection = Direction::FORWARD;
                        break;
                    default:
                        break;
                }
            }
            
    
            Direction getCurrentDirection() const {
                return currentDirection;
            }

            void printDirection() {
                switch (currentDirection) {
                    case Direction::FORWARD:
                        cout << "Current direction: Forward" << endl;
                        break;
                    case Direction::LEFT:
                        cout << "Current direction: Left" << endl;
                        break;
                    case Direction::RIGHT:
                        cout << "Current direction: Right" << endl;
                        break;
                    case Direction::BEHIND:
                        cout << "Current direction: Behind" << endl;
                        break;
                    case Direction::UP:
                        cout << "Current direction: Up" << endl;
                        break;
                    case Direction::DOWN:
                        cout << "Current direction: Down" << endl;
                        break;
                    default:
                        cout << "direction!" << endl;
                        break;
                }
            }
    };
}

#endif