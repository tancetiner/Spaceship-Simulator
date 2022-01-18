// from the private repo of Alp Cihan (https://github.com/alpcihan)
#pragma once
#include "GameObj3D.hpp"

vector<GameObj3D *> scene;

void DeleteFromScene(int id)
{
    int index = -1;
    for (int i = 0; i < scene.size(); i++)
    {
        if (scene[i]->id == id)
        {
            index = i;
            break;
        }
    }
    if (index > -1)
    {
        delete scene[index];
        scene.erase(scene.begin() + index);
    }
}

bool CollidesWithSth(GameObj3D &check, const int & num, int & idx)
{
    for (int i = 0; i < scene.size() - num; i++)
    {
        GameObj3D *obj = scene[i];
        if (check.id != obj->id)
        {
            if (intersect(check, *obj))
            {
                idx = i;
                return true;
            }
        }
    }
    return false;
}

bool CollidesWithTarget(GameObj3D &check, unsigned idx){
    return intersect(check, *scene[idx]);
}


