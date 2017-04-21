#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include <SDL_opengl.h>

class Entity {
private:
    float height;
    float width;
    float x_value;
    float y_value;
    float x_direction;
    float y_direction;

    Matrix* modelMatrix = new Matrix;

public:
    Entity();
    Entity(float height, float width, float x_value, float y_value, float newXdir, float newYDir);
    
    float getX ();
    float getY ();
    float getHeight ();
    float getWidth ();
    void setDirectionX (float x);
    void setDirectionY (float y);
    Matrix* getModel ();
    void setModel(Matrix* );
    
    void draw (ShaderProgram* program);
    bool detectCollision(Entity * collisionWith);
    void update(float elapsed, float velocity);
    
};

#endif /* Entity_hpp */
