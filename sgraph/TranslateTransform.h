#ifndef _TRANSLATETRANSFORM_H_
#define _TRANSLATETRANSFORM_H_


#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>

namespace sgraph {
    /**
     * This node represents a specific kind of transform: translation
     * 
     */
    class TranslateTransform: public TransformNode {
        protected:
            float tx,ty,tz;

        ParentSGNode *copyNode() {
            return new TranslateTransform(tx,ty,tz,name,scenegraph);
        }

        public:
            TranslateTransform(float tx,float ty,float tz,const string& name,sgraph::IScenegraph *graph) 
                :TransformNode(name,graph) {
                    this->tx = tx;
                    this->ty = ty;
                    this->tz = tz;
                    glm::mat4 transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz));
                    setTransform(transform);
            }

            
            /**
             * Visit this node.
             * 
             */
            void accept(SGNodeVisitor* visitor) {
                visitor->visitTranslateTransform(this);
            }

            glm::vec3 getTranslate() {
            return glm::vec3(tx,ty,tz);
        }

        /**
         * Method to update the translation values.
         * Updates tx, ty, tz and recalculates the transformation matrix.
         */
        void updateTranslation(const glm::vec3 &newTranslation) {
            this->tx = newTranslation.x;
            this->ty = newTranslation.y;
            this->tz = newTranslation.z;

            glm::mat4 transform = glm::translate(glm::mat4(1.0), glm::vec3(tx, ty, tz));
            setTransform(transform); 
        }

    };
}

#endif