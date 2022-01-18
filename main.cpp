#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/common.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Window.hpp"
#include "GameObj3D.hpp"
#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "parametric-3d/Parametric3DShape.hpp"
#include "CubeData.hpp"
#include "Textures.hpp"
#include "collusion-helpers.hpp"
#include "Scene.hpp"
#include <iostream>
#include <vector>
#include <random>
using namespace std;

// Globals
int u_transform, u_pv, u_frame, u_light_pos, u_light_color, is3D;
int moveFront = 0, moveRight = 0;
float mouseX = 0, mouseY = 0, velocityY = 0;
glm::vec3 targetLocation, hitMeteorLocation, hitTargetLocation;
float velocity = 0.0f;
int velocitySignal = 2, noOfFire = 0;
bool isFired = false, isMeteorHit = false, isTargetHit = false, isGameOver, isWon;
int meteorHitFrameCount = 0, targetHitFrameCount = 0, point, health, gameOverDuration;
float shipHitSpeed = 0.5f;

// helper functions are defined starting from here
void calculateVelocity(const int & num){ // responsible for calculating the velocity according the user input and current velocity
    if (num == 1){
        if (velocity == 0.0f){
            velocity = 0.05f;
        }
        else if (velocity < 1.0f){
            velocity += 0.95f / 240;
        }
    }
    
    else if (num == 2){
        
    }
    
    else if (num == 3){
        if (velocity >= (1.0f / 120))
            velocity -= 1.0f / 120;
        else velocity = 0;
    }
    
    else if (num == 4){
        if (velocity <= (1.5f - (0.5f / 60)))
            velocity += 0.5f / 60;
    }
}

void keyCallback(GLFWwindow *_, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        velocitySignal = 1;
    }
    else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
    {
        velocitySignal = 2;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        velocitySignal = 3;
    }
    else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
    {
        velocitySignal = 2;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        moveRight = 1;
    }
    else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        moveRight = -1;
    }
    else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        moveRight = 0;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS){
        velocitySignal = 4;
    }
    
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        isFired = true;
    }
}

static void cursorPositionCallback(GLFWwindow *_, double x, double y)
{
    mouseX = 2.0 * ((float)x / Window::width) - 1;
    mouseY = 2.0 * (1 - ((float)y / Window::height)) - 1;
}

int randomNum(const int & low, const int & high){ // random integer creator that used all in the program
    random_device rd;
    mt19937 e{rd()};
    uniform_int_distribution<int> dist{low, high};
    return dist(e);
}

float returnPosition(){ // returns a position based on the coordinates that randomNum function returns
    float sign = 0.01f;
    if ((randomNum(1, 8) % 2) == 1) sign = -0.01f;
    return  350.0f * randomNum(1, 99) * sign;
}

bool isLocationAvailable(vector<glm::vec3> locationList, const glm::vec3 & location){ // chechks all the location and return if its available for a given location
    for (int i=0 ; i<locationList.size() ; i++){
        glm::vec3 distance = location - locationList[i];
        if (glm::length(distance) < 40.0f)
            return false;
    }
    return true;
}

void resetShipPosition(GameObj3D & ship, const glm::vec3 & startingPoint, const glm::vec3 & startingRotation){ // when it's necessary this function resets the ship's location to its starting point
    ship.translate(startingPoint.x, startingPoint.y, startingPoint.z);
    ship.rotate(startingRotation.x, startingRotation.y, startingRotation.z);
    velocity = 0;
}

float returnAbs(float num){ // returns absolute value of a given value
    if (num < 0){
        return -1 * num;
    }
    return num;
}

int calculatePanelTextureIdx(){ // returns the panel texture index based on current health and point of player
    return ((5 - health) *  11 - point + 10);
}

void createFire(vector<GameObj3D *> * scene, GameObj3D & ship){ // creates the 3D model that we see as the fire
    Model3D cubeModel(CubeData::positions, CubeData::normals, CubeData::uvs, CubeData::indices);
    GameObj3D * fire = new GameObj3D(cubeModel);
    glm::vec3 position = ship.position() + 2.0f * ship.front();
    float side = ship.scale().x * 0.70f;
    if (noOfFire % 2 == 1)
        side = -1.0f * side;
    fire->translate(position.x + side, position.y, position.z);
    fire->scale(0.05, 0.05, 4);
    glm::vec3 rotation = ship.rotation();
    fire->rotate(rotation.x, rotation.y, rotation.z);
    fire->textureId = 2;
    scene->push_back(fire);
}
// end of the helper function definitions


int main(){
    Window::init(1000, 1000, "Spaceship Game");
    glfwSetKeyCallback(Window::window, keyCallback);
    glfwSetCursorPosCallback(Window::window, cursorPositionCallback);

    // initializing models and necessary data structures
    Model3D sphereModel = Parametric3DShape::generate(ParametricLine::halfCircle, 50, 50);
    Model3D cubeModel(CubeData::positions, CubeData::normals, CubeData::uvs, CubeData::indices);
    Model3D targetModel = Parametric3DShape::generate(ParametricLine::circle, 50, 50);
    vector<glm::vec3> objectLocationList;

    // initializing the ship
    GameObj3D ship(targetModel);
    glm::vec3 startingPoint(-3, 9, -22);
    ship.translate(startingPoint.x, startingPoint.y, startingPoint.z);
    objectLocationList.push_back(startingPoint);
    ship.scale(0.4, 0.4, 0.4);
    glm::vec3 startingRotation(2, -150, 0);
    ship.rotate(startingRotation.x, startingRotation.y, startingRotation.z);
    ship.textureId = 0;
    scene.push_back(&ship);

    // initializing planets
    int numberOfSpheres = 250;
    float rotationValueX;
    float rotationValueY;
    float rotationValueZ;
    vector<glm::vec3> planetRotations;
    for (int i=0 ; i<numberOfSpheres; i++){
        GameObj3D * sphere = new GameObj3D(sphereModel, false);
        glm::vec3 translationVec(returnPosition(), returnPosition(), returnPosition());
        while (!isLocationAvailable(objectLocationList, translationVec))
            translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
        sphere->translate(translationVec.x, translationVec.y, translationVec.z);
        objectLocationList.push_back(translationVec);
        float scaleValue = 1.5f + randomNum(0, 35) * 0.1f;
        sphere->scale(scaleValue, scaleValue, scaleValue);
        rotationValueX = randomNum(10, 30) * 0.01f;
        rotationValueY = randomNum(10, 30) * 0.01f;
        rotationValueZ = randomNum(10, 30) * 0.01f;
        planetRotations.push_back(glm::vec3(rotationValueX, rotationValueY, rotationValueZ));
        sphere->rotate(0,0,0);
        sphere->textureId = (i%16)+4;
        scene.push_back(sphere);
    }
    
    // initializing target
    GameObj3D * target = new GameObj3D(cubeModel);
    glm::vec3 translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
    while (!isLocationAvailable(objectLocationList, translationVec))
        translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
    target->translate(translationVec.x, translationVec.y, translationVec.z);
    objectLocationList.push_back(translationVec);
    target->scale(10,10,10);
    target->textureId = 0;
    int targetIdx = numberOfSpheres + 1;
    rotationValueX = randomNum(1, 90);
    rotationValueY = randomNum(1, 90);
    rotationValueZ = randomNum(1, 90);
    target->rotate(rotationValueX, rotationValueY, rotationValueZ);
    scene.push_back(target);
    targetLocation = translationVec;
    
    // initializing meteors
    int numOfMeteors = 250;
    vector<glm::vec3> meteorDirections;
    Model3D spikeModel = Parametric3DShape::generate(ParametricLine::spikes, 200, 10);
    for (int i=0 ; i<numOfMeteors; i++){
        GameObj3D * meteor = new GameObj3D(spikeModel);
        translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
        while (!isLocationAvailable(objectLocationList, translationVec))
            translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
        meteor->translate(translationVec.x, translationVec.y, translationVec.z);
        objectLocationList.push_back(translationVec);
        meteor->scale(0.8, 0.8, 0.8);
        meteor->rotate(0,0,0);
        meteor->textureId = 1;
        rotationValueX = randomNum(10, 39) * 0.002f;
        rotationValueY = randomNum(10, 39) * 0.002f;
        rotationValueZ = randomNum(10, 39) * 0.002f;
        meteorDirections.push_back(glm::vec3(rotationValueX, rotationValueY, rotationValueZ));
        scene.push_back(meteor);
    }
    
    // initializing the box
    GameObj3D * box = new GameObj3D(cubeModel);
    box->translate(0, 0, 0);
    box->rotate(0,0,0);
    int boxScale = 600;
    box->scale(boxScale, boxScale, boxScale);
    box->textureId = 3;
    
    //initializing control panel
    GameObj3D * panel = new GameObj3D(cubeModel);
    panel->translate(0.8, -0.8, 0);
    panel->rotate(0,0,0);
    panel->scale(0.2,0.2,0.01);
    
    // a scene for meteor effects
    vector<GameObj3D *> effectScene;
    
    // light
    glm::vec3 lightPos = glm::vec3(0.0, 0.0, 1.0);
    glm::vec3 lightColor = glm::vec3(1.0, 1.0, 1.0);

    // main textures
    const vector<string> texture_files{
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/red-concrete-background.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/mar1.png",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/fire2.jpeg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/8k_stars.jpeg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_ceres_fictional.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_earth_clouds.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_earth_daymap.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_earth_nightmap.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_earth_normal_map.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_eris_fictional.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_haumea_fictional.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_jupiter.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_makemake_fictional.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_mars.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_moon.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_neptune.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_uranus.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_venus_atmosphere.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/2k_venus_surface.jpg",
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/textures/mercuryTexture.jpg"
    };
    
    // load main textures
    vector<unsigned int> textures = Textures::loadTextures(texture_files);
    
    // storing panel texture data
    const vector<string> panel_textures{
        "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-8.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-9.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-10.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-11.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-12.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-13.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-14.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-15.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-16.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-17.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-18.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-19.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-20.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-21.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-22.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-23.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-24.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-25.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-26.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-27.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-28.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-29.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-30.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-31.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-32.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-33.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-34.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-35.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-36.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-37.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-38.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-39.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-40.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-41.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-42.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-43.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-44.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-45.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-46.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-47.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-48.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-49.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-50.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-51.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-52.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-53.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-54.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-55.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-56.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-57.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-58.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-59.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-60.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-61.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/control panel.drawio-62.png","/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/gameOver.png", "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/panel_textures/youWin.png"
    };
    
    // load panel textures
    vector<unsigned int> panelTextures = Textures::loadTextures(panel_textures);
    
    // create shader
    ShaderProgram sp("/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/shader/vertex.vert", "/Users/tancetiner/Desktop/Developer/cs405/3d project template 3/3d project template 3/shader/frag.frag");
    u_transform = glGetUniformLocation(sp.id, "u_transform");
    u_pv = glGetUniformLocation(sp.id, "u_pv");
    u_frame = glGetUniformLocation(sp.id, "u_frame");
    u_light_pos = glGetUniformLocation(sp.id, "u_light_pos");
    u_light_color = glGetUniformLocation(sp.id, "u_light_color");
    is3D = glGetUniformLocation(sp.id, "is3D"); // added for control panel drawing
    auto u_texture = glGetUniformLocation(sp.id, "u_texture");
    glUniform1i(u_texture, 0);    // 0th unit
    glActiveTexture(GL_TEXTURE0); // active 0th unit
    sp.use();
    
    // initializing variables that'll be used in the game loop
    health = 5;
    point = 0;
    panel->textureId = 10;
    gameOverDuration = 0;
    isGameOver = false;
    isWon = false;
    
    // game loop
    while (!Window::isClosed()){
        
        // character movement
        calculateVelocity(velocitySignal);
        ship.moveFront(velocity * 1.5f);
        ship.rotate(
            mouseY*70.0f,
            ship.rotation().y - moveRight * 1.0f,
            ship.rotation().z
        );
                
        // camera
        // first calculate respect to hero
        Camera::position = ship.position() - ship.front() * 5.0f + ship.up() * 1.0f;
        Camera::front = ship.front();
        Camera::up = ship.up();
        
        // then rotate
        glm::vec3 rotation(1,0,0);
        Camera::up = glm::vec3(glm::toMat4(glm::quat(glm::radians(rotation))) * glm::vec4(Camera::up, 1));
        Camera::front = glm::vec3(glm::toMat4(glm::quat(glm::radians(rotation))) * glm::vec4(Camera::front, 1));
        
        // check if character is inside borders
        if (returnAbs(ship.position().x) >= boxScale || returnAbs(ship.position().y) >= boxScale || returnAbs(ship.position().z) >= boxScale){
            resetShipPosition(ship, startingPoint, startingRotation);
        }
        
        // calculate meteor movements
        int idx = 0;
        for (int i=0 ; i<numOfMeteors ; i++){
            GameObj3D * obj = scene[targetIdx + i + 1];
            glm::vec3 dir = meteorDirections[i] + obj->position();
            obj->translate(dir.x, dir.y, dir.z);
            if (CollidesWithSth(*obj, 0, idx) || returnAbs(obj->position().x) >= boxScale || returnAbs(obj->position().y) >= boxScale || returnAbs(obj->position().z) >= boxScale){
                if (CollidesWithTarget(ship, targetIdx + i + 1)){
                    health -= 1;
                    panel->textureId = calculatePanelTextureIdx();
                    resetShipPosition(ship, startingPoint, startingRotation);
                }
                if (idx >= scene.size() - noOfFire){
                    hitMeteorLocation = obj->position();
                    isMeteorHit = true;
                    meteorHitFrameCount = 0;
                    scene.erase(scene.begin() + idx);
                }
                translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
                while (!isLocationAvailable(objectLocationList, translationVec))
                    translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
                obj->translate(translationVec.x, translationVec.y, translationVec.z);
                objectLocationList[targetIdx + i] = translationVec;
                rotationValueX = randomNum(10, 39) * 0.01f;
                rotationValueY = randomNum(10, 39) * 0.01f;
                rotationValueZ = randomNum(10, 39) * 0.01f;
                meteorDirections[i] = glm::vec3(rotationValueX, rotationValueY, rotationValueZ);
            }
        }
        
        // check collision between ship and target
        bool collisionWithTarget = false;
        if (CollidesWithTarget(ship, targetIdx)){
            hitTargetLocation = targetLocation;
            isTargetHit = true;
            targetHitFrameCount = 0;
            shipHitSpeed = velocity;
            translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
            while (!isLocationAvailable(objectLocationList, translationVec))
                translationVec = glm::vec3(returnPosition(), returnPosition(), returnPosition());
            target->translate(translationVec.x, translationVec.y, translationVec.z);
            objectLocationList[targetIdx] = translationVec;
            rotationValueX = randomNum(1, 90);
            rotationValueY = randomNum(1, 90);
            rotationValueZ = randomNum(1, 90);
            target->rotate(rotationValueX, rotationValueY, rotationValueZ);
            targetLocation = translationVec;
            collisionWithTarget = true;
            point += 2;
            panel->textureId = calculatePanelTextureIdx();
        }
    
        // check collisions between ship and the other planets
        if ((!collisionWithTarget) && CollidesWithSth(ship, noOfFire, idx)){
            health = 5;
            point = 0;
            panel->textureId = panelTextures.size() - 1;
            isGameOver = true;
            gameOverDuration = 0;
            resetShipPosition(ship, startingPoint, startingRotation);
        }
        
        // handle fire operation
        if (isFired){
            createFire(&scene, ship);
            isFired = false;
            noOfFire += 1;
        }
        
        // calculate fire movement
        for (unsigned i = scene.size() - noOfFire ; i<scene.size() ; i++){
            GameObj3D * obj = scene[i];
            if (CollidesWithSth(*obj, noOfFire, idx) ||returnAbs(obj->position().x) >= boxScale || returnAbs(obj->position().y) >= boxScale || returnAbs(obj->position().z) >= boxScale){
                scene.erase(scene.begin() + i);
            }
            else obj->moveFront(2.5f);
        }
        
        // handle planet rotations
        for (int i=1 ; i<numberOfSpheres+1 ; i++){
            GameObj3D * obj = scene[i];
            glm::vec3 rotationValue = obj->rotation() + planetRotations[i-1];
            obj->rotate(rotationValue.x, rotationValue.y, rotationValue.z);
        }
        
        // handle meteor effect
        if (isMeteorHit){
            if (meteorHitFrameCount == 0){
                point += 1;
                panel->textureId = calculatePanelTextureIdx();
                for (int i=0 ; i<400 ; i++){
                    GameObj3D * particle = new GameObj3D(sphereModel);
                    particle->translate(hitMeteorLocation.x, hitMeteorLocation.y, hitMeteorLocation.z);
                    glm::vec3 scale(0.01f * randomNum(1, 5), 0.01f * randomNum(1, 5), 0.01f * randomNum(1, 5));
                    particle->scale(scale.x, scale.y, scale.z);
                    float rotationValue = 359.0f * i /100.0f;
                    particle->rotate(randomNum(0,359), randomNum(0,359), randomNum(0,359));
                    particle->textureId = 1;
                    effectScene.push_back(particle);
                }
            }
            meteorHitFrameCount++;
            if (meteorHitFrameCount == 60){
                isMeteorHit = false;
                meteorHitFrameCount = 0;
                effectScene.clear();
            }
            else{
                for (std::vector<GameObj3D*>::iterator t = effectScene.begin(); t != effectScene.end(); ++t){
                    const int i = t - effectScene.begin();
                    GameObj3D* obj = effectScene[i];
                    obj->moveFront(0.5f);
                }
            }
        }
        
        // handle target hit effect
        if (isTargetHit){
            if (targetHitFrameCount == 0){
                for (int i=0 ; i<800 ; i++){
                    GameObj3D * particle = new GameObj3D(cubeModel);
                    glm::vec3 loc = hitTargetLocation + ship.front() * 3.0f;
                    particle->translate(loc.x, loc.y, loc.z);
                    glm::vec3 scale(0.02f * randomNum(1, 10), 0.02f * randomNum(1, 10), 0.02f * randomNum(1, 10));
                    particle->scale(scale.x, scale.y, scale.z);
                    float rotationValue = 359.0f * i /100.0f;
                    int radius = 150;
                    particle->rotate(randomNum(0,359), randomNum(0,359), randomNum(0,359));
                    particle->textureId = 0;
                    effectScene.push_back(particle);
                }
            }
            targetHitFrameCount++;
            if (targetHitFrameCount == 120){
                isTargetHit = false;
                targetHitFrameCount = 0;
                effectScene.clear();
            }
            else{
                for (std::vector<GameObj3D*>::iterator t = effectScene.begin(); t != effectScene.end(); ++t){
                    const int i = t - effectScene.begin();
                    GameObj3D* obj = effectScene[i];
                    obj->moveFront(shipHitSpeed * 2.0f);
                }
            }
        }
        
        // displaying 'game over' texture
        if (isGameOver){
            if (gameOverDuration < 120){
                panel->textureId = panelTextures.size() - 2;
                gameOverDuration++;
            }
            else{
                isGameOver = false;
                gameOverDuration = 0;
                panel->textureId = calculatePanelTextureIdx();
            }
        }
        
        // checking if we won
        if (point >= 11){
            isWon = true;
            gameOverDuration = 0;
        }
        
        // displaying 'you win' texture
        if (isWon){
            if (gameOverDuration == 0){
                resetShipPosition(ship, startingPoint, startingRotation);
                panel->textureId = panelTextures.size() - 1;
                point = 0;
                health = 5;
                gameOverDuration++;
            }
            else if (gameOverDuration < 120){
                panel->textureId = panelTextures.size() - 1;
                gameOverDuration++;
            }
            else{
                gameOverDuration = 0;
                panel->textureId = calculatePanelTextureIdx();
                isWon = false;
            }
        }
 
        // update uniforms
        glUniformMatrix4fv(u_pv, 1, GL_FALSE, glm::value_ptr(Camera::getProjMatrix() * Camera::getViewMatrix()));
        glUniform1i(u_frame, 1);
        glUniform3fv(u_light_pos, 1, glm::value_ptr(lightPos));
        glUniform3fv(u_light_color, 1, glm::value_ptr(lightColor));
        glUniform1i(is3D, 5);

        // main scene draw
        for (std::vector<GameObj3D*>::iterator t = scene.begin(); t != scene.end(); ++t){
            const int i = t - scene.begin();
            GameObj3D* object = scene[i];
            glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(object -> getTransform()));
            glBindTexture(GL_TEXTURE_2D, textures[object -> textureId]);
            object -> draw();
        }
        
        // drawing the box independent from the main scene (because we don't want to check collision)
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(box -> getTransform()));
        glBindTexture(GL_TEXTURE_2D, textures[box -> textureId]);
        box -> draw();
        
        // drawing meteor particle effects (same logic as floor drawing, we don't want collision check)
        for (std::vector<GameObj3D*>::iterator t = effectScene.begin(); t != effectScene.end(); ++t){
            const int i = t - effectScene.begin();
            GameObj3D * object = effectScene[i];
            glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(object -> getTransform()));
            glBindTexture(GL_TEXTURE_2D, textures[object -> textureId]);
            object -> draw();
        }
        
        // drawing 2D control panel
        glUniform1i(is3D, 6);
        glUniformMatrix4fv(u_transform, 1, GL_FALSE, glm::value_ptr(panel -> getTransform()));
        glBindTexture(GL_TEXTURE_2D, panelTextures[panel->textureId]);
        panel->draw();

        // update the scene
        Window::refresh();
    }

    Window::terminate();

    return 0;
}
