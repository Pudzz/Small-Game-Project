#pragma once
#include "Physics.h"

#include "RaycastHit.h"
#include <iostream>
#include "Logger.h"

class RaycastCallback : public rp3d::RaycastCallback
{

public:

    virtual rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info) 
    {
        hitInfo.position = { info.worldPoint.x, info.worldPoint.y, info.worldPoint.z };
        hitInfo.normal = { info.worldNormal.x, info.worldNormal.y, info.worldNormal.z };

        hitInfo.hit = true;
        return rp3d::decimal(1.0);
    }

    RaycastHit hitInfo;
};
