// from the private repo of Alp Cihan (https://github.com/alpcihan)
#ifndef GAMEOBJ3D_HPP
#define GAMEOBJ3D_HPP

#include "Model3D.hpp"
#include "glm/gtx/quaternion.hpp"
#include <algorithm>
#include <initializer_list>
#include <limits>

struct TransformInfo
{
public:
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation; // degrees

    glm::mat4 getTransformMatrix()
    {
        return glm::translate(glm::mat4(1.0f), this->position) * glm::toMat4(glm::quat(rotation)) * glm::scale(glm::mat4(1.0f), this->scale);
    }
};

struct Collider
{
public:
    float getMaxX() { return maxX; }
    float getMinX() { return minX; }
    float getMaxY() { return maxY; }
    float getMinY() { return minY; }
    float getMaxZ() { return maxZ; }
    float getMinZ() { return minZ; }

private:
    bool isBox = true; // true = box collider, false = sphere collider
    glm::vec3 center;

    // for box
    glm::vec3 initToX, initToY, initToZ;
    glm::vec3 toX, toY, toZ;
    float maxX, minX, maxY, minY, maxZ, minZ;

    void updateMinMax(){
        float maxXD = (toX.x>=0?toX.x:-toX.x) + (toY.x>=0?toY.x:-toY.x) + (toZ.x>=0?toZ.x:-toZ.x);
        float maxYD = (toX.y>=0?toX.y:-toX.y) + (toY.y>=0?toY.y:-toY.y) + (toZ.y>=0?toZ.y:-toZ.y);
        float maxZD = (toX.z>=0?toX.z:-toX.z) + (toY.z>=0?toY.z:-toY.z) + (toZ.z>=0?toZ.z:-toZ.z);

        maxX = center.x + maxXD;
        minX = center.x - maxXD;
        maxY = center.y + maxYD;
        minY = center.y - maxYD;
        maxZ = center.z + maxZD;
        minZ = center.z - maxZD;

        //cout << "M: " << maxX << " " << minX << " " << maxY << " " << minY << " " << maxZ << " " << minZ << " " << endl;
    };
    
    void makeSphereCollider(){
        isBox = false;
    };
    
    // for sphere
    float radius;

friend class GameObj3D;
};

class GameObj3D
{
public:
    GameObj3D(Model3D &model, bool isBoxCollider = true);
    ~GameObj3D();

    void draw();
    void load(Model3D *model);

    void translate(float x, float y, float z);
    void scale(float x, float y, float z);
    void rotate(float x, float y, float z); // degrees

    void moveFront(float num);
    void moveRight(float num);
    void moveUp(float num);

    glm::vec3 front();
    glm::vec3 up();
    glm::vec3 right();

    glm::vec3 position();
    glm::vec3 scale();
    glm::vec3 rotation();
    glm::mat4 getTransform();

    Collider collider;
    unsigned textureId = 0;
    unsigned id;

    bool hasGravity = true;
    float velocity = 0;
    
    // functions i added
    void scaleCollider(float x, float y, float z);
    void makeFront(float x, float y, float z);
    glm::vec3 getFront();

private:
    glm::vec3 _front = glm::vec3(0, 0, -1);
    glm::vec3 _up = glm::vec3(0, 1, 0);
    glm::vec3 _right = glm::vec3(-1, 0, 0);

private:
    void setCollider(bool isBoxCollider);
    static unsigned uuid;

private:
    Model3D *model3d = nullptr;
    TransformInfo transformInfo;
};

GameObj3D::GameObj3D(Model3D &model, bool isBoxCollider)
{
    this -> id = GameObj3D::uuid;
    GameObj3D::uuid++;

    this->transformInfo.position = glm::vec3(0);
    this->transformInfo.scale = glm::vec3(1);
    this->transformInfo.rotation = glm::vec3(0);

    this->model3d = &model;

    // set collider
    setCollider(isBoxCollider);
}

GameObj3D::~GameObj3D()
{
    cout << "Object " << id << " deleted." << endl;
}

void GameObj3D::draw()
{
    this->model3d->bind();
    glDrawElements(GL_TRIANGLES, model3d->getIndicesCount(), GL_UNSIGNED_INT, NULL);
}

void GameObj3D::load(Model3D *model)
{
    delete (this->model3d);
    this->model3d = model3d;
    collider.updateMinMax();
};

void GameObj3D::translate(float x, float y, float z)
{   
    // first update collider position
    const glm::vec3 posPrev = this->transformInfo.position;
    collider.center += glm::vec3(x-posPrev.x,y-posPrev.y,z-posPrev.z);

    this->transformInfo.position = glm::vec3(x, y, z);
    
    collider.updateMinMax();
}

void GameObj3D::scale(float x, float y, float z)
{
    this->transformInfo.scale = glm::vec3(x, y, z);
    
    // update collider
    collider.toX = collider.initToX * x;
    collider.toY = collider.initToY * y;
    collider.toZ = collider.initToZ * z;

    collider.updateMinMax();
}

void GameObj3D::rotate(float x, float y, float z)
{
    const glm::vec3 rt = glm::radians(glm::vec3(x,y,z));
    this->transformInfo.rotation = rt;

    glm::mat4 trans = glm::toMat4(glm::quat(rt));
    _up = glm::vec3(trans * glm::vec4(glm::vec3(0, 1, 0), 1));
    _front = glm::vec3(trans * glm::vec4(glm::vec3(0, 0, -1), 1));
    _right = glm::cross(_front, _up);

    collider.updateMinMax();
}

void GameObj3D::moveFront(float num) {
    const glm::vec3 newPos = front() * num + position();
    translate(newPos.x, newPos.y, newPos.z);
}

void GameObj3D::moveRight(float num) {
    const glm::vec3 newPos = right() * num + position();
    translate(newPos.x, newPos.y, newPos.z);
}

void GameObj3D::moveUp(float num) {
    const glm::vec3 newPos = up() * num + position();
    translate(newPos.x, newPos.y, newPos.z);
}

glm::vec3 GameObj3D::front() {
    return glm::normalize(_front);
}

glm::vec3 GameObj3D::up() {
    return glm::normalize(_up);
}

glm::vec3 GameObj3D::right() {
    return glm::normalize(glm::cross(_up, _front));
}

glm::vec3 GameObj3D::position()
{
    return this->transformInfo.position;
}

glm::vec3 GameObj3D::scale()
{
    return this->transformInfo.scale;
}

glm::vec3 GameObj3D::rotation()
{
    return glm::degrees(this->transformInfo.rotation);
}

glm::mat4 GameObj3D::getTransform()
{
    return this->transformInfo.getTransformMatrix();
}

void GameObj3D::setCollider(bool isBoxCollider)
{
    float 
        maxX = std::numeric_limits<float>::min(), minX = std::numeric_limits<float>::max(),
        maxY = std::numeric_limits<float>::min(), minY = std::numeric_limits<float>::max(),
        maxZ = std::numeric_limits<float>::min(), minZ = std::numeric_limits<float>::max();
    

    for (std::vector<glm::vec3>::iterator t = this->model3d->positions.begin(); t != this->model3d->positions.end(); ++t) {
        float x = t->x, y = t->y, z = t->z;
        if(x > maxX) maxX = x;
        else if(x < minX) minX = x;
        if(y > maxY) maxY = y;
        else if(y < minY) minY = y;
        if(z > maxZ) maxZ = z;
        else if(z < minZ) minZ = z;
    }

    float xD = (maxX - minX)*0.5, yD = (maxY - minY)*0.5, zD = (maxY - minY)*0.5;

    collider.center = glm::vec3(minX + xD, minY + yD, minZ + zD);
    collider.toX = glm::vec3(xD,0,0);
    collider.toY = glm::vec3(0,yD,0);
    collider.toZ = glm::vec3(0,0,zD);
    collider.initToX = collider.toX;
    collider.initToY = collider.toY;
    collider.initToZ = collider.toZ;

    // for box
    if(isBoxCollider) {
        collider.updateMinMax();
    } 
}

void GameObj3D::scaleCollider(float x, float y, float z)
{
    // update collider
    collider.toX = collider.initToX * x;
    collider.toY = collider.initToY * y;
    collider.toZ = collider.initToZ * z;

    collider.updateMinMax();
}

void GameObj3D::makeFront(float x, float y, float z){
    _front = glm::vec3(x, y, z);
}

glm::vec3 GameObj3D::getFront(){
    return _front;
}
unsigned GameObj3D::uuid = 0;

#endif
