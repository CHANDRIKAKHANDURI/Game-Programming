//For some reason different things are happening when I run this code different times
//Sometimes the collission resolution looks very smooth, other times it takes one cube along with the other etc
//If you run a more than once you will see (hopefully) what I am talking about

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "Matrix.h"
#include "ShaderProgram.h"
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

class Vector {
public:
    Vector(){};
    Vector(float setX, float setY, float setZ){
        x = setX;
        y = setY;
        z = setZ;
    };
    
    float length() const {
        return sqrtf(pow(x, 2) + pow(y, 2) + pow(z, 2));
    }
    void normalize(){
        if (length() != 0) {
            x = x/length();
            y = y/length();
            z = z/length();
        }
    }
    
    Vector operator * (Matrix &matrix){
        Vector vec;
        vec.x = matrix.m[0][0]*x + matrix.m[1][0]*y + matrix.m[2][0]*z + matrix.m[3][0];
        vec.y = matrix.m[0][1]*x + matrix.m[1][1]*y + matrix.m[2][1]*z + matrix.m[3][1];
        vec.z = matrix.m[0][2]*x + matrix.m[1][2]*y + matrix.m[2][2]*z + matrix.m[3][2];
        return vec;
    }
    
    float x;
    float y;
    float z;
    
};
bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vector> &points1, const std::vector<Vector> &points2, Vector &penetration) {
    float normalX = -edgeY;
    float normalY = edgeX;
    float len = sqrtf(normalX*normalX + normalY*normalY);
    normalX /= len;
    normalY /= len;
    
    std::vector<float> e1Projected;
    std::vector<float> e2Projected;
    
    for(int i=0; i < points1.size(); i++) {
        e1Projected.push_back(points1[i].x * normalX + points1[i].y * normalY);
    }
    for(int i=0; i < points2.size(); i++) {
        e2Projected.push_back(points2[i].x * normalX + points2[i].y * normalY);
    }
    
    std::sort(e1Projected.begin(), e1Projected.end());
    std::sort(e2Projected.begin(), e2Projected.end());
    
    float e1Min = e1Projected[0];
    float e1Max = e1Projected[e1Projected.size()-1];
    float e2Min = e2Projected[0];
    float e2Max = e2Projected[e2Projected.size()-1];
    
    float e1Width = fabs(e1Max-e1Min);
    float e2Width = fabs(e2Max-e2Min);
    float e1Center = e1Min + (e1Width/2.0);
    float e2Center = e2Min + (e2Width/2.0);
    float dist = fabs(e1Center-e2Center);
    float p = dist - ((e1Width+e2Width)/2.0);
    
    if(p >= 0) {
        return false;
    }
    
    float penetrationMin1 = e1Max - e2Min;
    float penetrationMin2 = e2Max - e1Min;
    
    float penetrationAmount = penetrationMin1;
    if(penetrationMin2 < penetrationAmount) {
        penetrationAmount = penetrationMin2;
    }
    
    penetration.x = normalX * penetrationAmount;
    penetration.y = normalY * penetrationAmount;
    
    return true;
}

bool penetrationSort(const Vector &p1, const Vector &p2) {
    return p1.length() < p2.length();
}

bool checkSATCollision(const std::vector<Vector> &e1Points, const std::vector<Vector> &e2Points, Vector &penetration) {
    std::vector<Vector> penetrations;
    for(int i=0; i < e1Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e1Points.size()-1) {
            edgeX = e1Points[0].x - e1Points[i].x;
            edgeY = e1Points[0].y - e1Points[i].y;
        } else {
            edgeX = e1Points[i+1].x - e1Points[i].x;
            edgeY = e1Points[i+1].y - e1Points[i].y;
        }
        Vector penetration;
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);
        if(!result) {
            return false;
        }
        penetrations.push_back(penetration);
    }
    for(int i=0; i < e2Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e2Points.size()-1) {
            edgeX = e2Points[0].x - e2Points[i].x;
            edgeY = e2Points[0].y - e2Points[i].y;
        } else {
            edgeX = e2Points[i+1].x - e2Points[i].x;
            edgeY = e2Points[i+1].y - e2Points[i].y;
        }
        Vector penetration;
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points, penetration);
        
        if(!result) {
            return false;
        }
        penetrations.push_back(penetration);
    }
    
    std::sort(penetrations.begin(), penetrations.end(), penetrationSort);
    penetration = penetrations[0];
    
    Vector e1Center;
    for(int i=0; i < e1Points.size(); i++) {
        e1Center.x += e1Points[i].x;
        e1Center.y += e1Points[i].y;
    }
    e1Center.x /= (float)e1Points.size();
    e1Center.y /= (float)e1Points.size();
    
    Vector e2Center;
    for(int i=0; i < e2Points.size(); i++) {
        e2Center.x += e2Points[i].x;
        e2Center.y += e2Points[i].y;
    }
    e2Center.x /= (float)e2Points.size();
    e2Center.y /= (float)e2Points.size();
    
    Vector ba;
    ba.x = e1Center.x - e2Center.x;
    ba.y = e1Center.y - e2Center.y;
    
    if( (penetration.x * ba.x) + (penetration.y * ba.y) < 0.0f) {
        penetration.x *= -1.0f;
        penetration.y *= -1.0f;
    }
    
    return true;
}
class Entity {
public:
    Entity(float x1, float y1,
           float x2, float y2,
           float x3, float y3,
           float x4, float y4,
           float x5, float y5,
           float x6, float y6) {
        vertices = {
            Vector(x1, y1, 0),
            Vector(x2, y2, 0),
            Vector(x3, y3, 0),
            Vector(x4, y4, 0),
            Vector(x5, y5, 0),
            Vector(x6, y6, 0) //don't care about 3D
        };
        position.x = x1 + x2 / 2;
        position.y = y2 + y3 / 2;
        position.z = 0; //in 3D
        
        //calculating world space coordinates - we only have rectablges so only 4 vertices for each corner
        Vector vertex1;
        vertex1.x = x1;
        vertex1.y = y1;
        vertex1.z = 0;
        worldSpaceCoordinates.push_back(vertex1 * matrix);
        
        Vector vertex2;
        vertex2.x = x2;
        vertex2.y = y2;
        vertex2.z = 0;
        worldSpaceCoordinates.push_back(vertex2 * matrix);
        
        Vector vertex3;
        vertex3.x = x3;
        vertex3.y = y3;
        vertex3.z = 0;
        worldSpaceCoordinates.push_back(vertex3 * matrix);
        
        Vector vertex4;
        vertex4.x = x5;
        vertex4.y = y5;
        vertex4.z = 0;
        worldSpaceCoordinates.push_back(vertex4 * matrix);
    }
    Matrix matrix;
    Vector position;
    Vector scale;
    Vector penetration;
    std::vector<Vector> worldSpaceCoordinates;
    float rotation;
    float velocity_x;
    float velocity_y;
    std::vector<Vector> vertices;
    
    
    std::vector<Vector> getCurrentWorldSpaceCoordinates(){
        matrix.identity();
        matrix.Translate(position.x, position.y, position.z);
        matrix.Rotate(rotation);
        matrix.Scale(scale.x, scale.y, scale.z);
        
        //calculating world space coordinates - we only have rectablges so only 4 vertices for each corner
        Vector vertex1;
        vertex1.x = vertices[0].x;
        vertex1.y = vertices[0].y;
        vertex1.z = 0;
        
        worldSpaceCoordinates[0] = vertex1 * matrix;
        
        Vector vertex2;
        vertex2.x = vertices[1].x;
        vertex2.y = vertices[1].y;
        vertex2.z = 0;
        
        worldSpaceCoordinates[1] = vertex2 * matrix;
        
        Vector vertex3;
        vertex3.x = vertices[2].x;
        vertex3.y = vertices[2].y;
        vertex3.z = 0;
        
        worldSpaceCoordinates[2] = vertex3 * matrix;
        
        Vector vertex4;
        vertex4.x = vertices[4].x;
        vertex4.y = vertices[4].y;
        vertex4.z = 0;
        
        worldSpaceCoordinates[3] = vertex4 * matrix;
        
        return worldSpaceCoordinates;
    }
    
    
    void update(ShaderProgram* program, float elapsedTime, Entity enemy1, Entity enemy2){
        Vector penetration;
        //change position every frame
        position.x += velocity_x*elapsedTime;
        position.y += velocity_y*elapsedTime;

        //func that calcs world spcae
        //check collisions with enemy1
        if (checkSATCollision(this->getCurrentWorldSpaceCoordinates(), enemy1.getCurrentWorldSpaceCoordinates(), penetration)){
            //true then update based on the penetration vector
            position.x += penetration.x / 2.0f;
            position.y += penetration.y / 2.0f;
            enemy1.position.x -= penetration.x / 2.0f;
            enemy1.position.y -= penetration.y / 2.0f;
            velocity_x *= -1;
            velocity_y *= -1;
            enemy1.velocity_x *= -1;
            enemy1.velocity_y *= -1;

        }
        //check collisions with enemy2
        if (checkSATCollision(this->getCurrentWorldSpaceCoordinates(), enemy2.getCurrentWorldSpaceCoordinates(), penetration)){
            position.x += penetration.x / 2.0f;
            position.y += penetration.y / 2.0f;
            enemy2.position.x -= penetration.x / 2.0f;
            enemy2.position.y -= penetration.y / 2.0f;
            velocity_x *= -1;
            velocity_y *= -1;
            enemy2.velocity_x *= -1;
            enemy2.velocity_y *= -1;
        }
        
    }
    
    void render(ShaderProgram* program){
        program->setModelMatrix(matrix);
        float verticesToDraw [] = {
            vertices[0].x, vertices[0].y,
            vertices[1].x, vertices[1].y,
            vertices[2].x, vertices[2].y,
            vertices[3].x, vertices[3].y,
            vertices[4].x, vertices[4].y,
            vertices[5].x, vertices[5].y
        };
        glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, verticesToDraw);
        glEnableVertexAttribArray(program->positionAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
};

int main(int argc, char *argv[])
{
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    float lastFrameTicks = 0.0f;
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 640, 360);
    ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    SDL_Event event;
    bool done = false;
    Entity entity1(2, 1, -2, 1, -2, -1, -2, -1, 2, -1, 2, 1);
    entity1.position = {-2,0,0};
    Entity entity2(2, 1, -2, 1, -2, -1, -2, -1, 2, -1, 2, 1);
    entity2.position = {2, 0, 0};
    Entity entity3(2, 1, -2, 1, -2, -1, -2, -1, 2, -1, 2, 1);
    entity3.position = {0,2,0};
    entity1.velocity_x = 1;
    entity2.velocity_x = -1;
    entity3.velocity_y = -1;
    
    while (!done) {
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);

        //MEDIUM
        entity1.rotation += elapsed*5;
        entity1.scale = {0.2, 0.2, 0.2};
        entity1.update(&program, elapsed, entity2, entity3);
        entity1.render(&program);
        
        //SMALL
        entity2.rotation += elapsed*5;
        entity2.scale = {0.1, 0.1, 0.1};
        entity2.update(&program, elapsed, entity1, entity3);
        entity2.render(&program);
        
        //LARGE
        entity3.rotation += elapsed*1;
        entity3.scale = {0.3, 0.3, 0.3};
        entity3.update(&program, elapsed, entity1, entity2);
        entity3.render(&program);
        
        glDisableVertexAttribArray(program.positionAttribute);
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
