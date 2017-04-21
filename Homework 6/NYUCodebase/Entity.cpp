#include "Entity.hpp"


Entity::Entity (float newHeight, float newWidth, float newX, float newY, float newXdir, float newYdir) {
    height = newHeight;
    width = newWidth;
    x_value = newX;
    y_value = newY;
    x_direction = newYdir;
    y_direction = newXdir;
};

void Entity::setDirectionX(float newX) {
    x_direction = newX;
};

void Entity::setDirectionY(float newY) {
    y_direction = newY;
};

float Entity::getX () {
    return x_value;
};

float Entity::getY () {
    return y_value;
};

float Entity::getHeight () {
    return height;
};

float Entity::getWidth () {
    return width;
};

void Entity::draw (ShaderProgram* program) {
    float halfHeight = height/2;
    float halfWidth = width/2;
    program->setModelMatrix(*modelMatrix);
    float vertices [] = {
        x_value-halfWidth, y_value-halfHeight,
        x_value + halfWidth, y_value - halfHeight,
        x_value + halfWidth, y_value + halfHeight,
        x_value - halfWidth, y_value - halfHeight,
        x_value + halfWidth, y_value + halfHeight,
        x_value - halfWidth, y_value + halfHeight
    };
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray (program->positionAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 8);
    glDisableVertexAttribArray(program->positionAttribute);
};

void Entity::update(float elapsed, float velocity)
{
    x_value += (x_direction * elapsed * velocity);
    y_value += (y_direction * elapsed * velocity);
}

bool Entity::detectCollision(Entity * collideWith){
    float entityBottom = y_value - (height/2);
    float entityTop = y_value + (height/2);
    float entityLeft = x_value - (width/2);
    float entityRight = x_value + (width/2);
    
    float entity2Top = collideWith->getY() + (collideWith->getHeight()/2);
    float entity2Bottom = collideWith->getY() - (collideWith->getHeight()/2);
    float entity2Left = collideWith->getX() - (collideWith->getWidth()/2);
    float entity2Right = collideWith->getX() + (collideWith->getWidth()/2);
    
    return (
    entityBottom >= entity2Top ||
    entityTop <= entity2Bottom ||
    entityLeft >= entity2Right ||
    entityRight <= entity2Left);
            
}

