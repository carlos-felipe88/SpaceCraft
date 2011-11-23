#include "main.h"

double SubPlanetSektor::Wurzel_3 = sqrtf(3.0f);

SubPlanetSektor::SubPlanetSektor(Vector3Unit position, Unit radius, SektorID id, Sektor* parent)
: Sektor(position, radius, id, parent)
{
    mType = SUB_PLANET;    
    PlanetSektor* parentPlanet = (PlanetSektor*)parent;
    assert(parentPlanet->getType() == PLANET);        
    RenderGridBoxSide boxSide;    
    if(id.z == -1) boxSide = BOX_FRONT;
    else if(id.x == 1) boxSide = BOX_RIGHT;
    else if(id.z == 1) boxSide = BOX_BACK;
    else if(id.x == -1) boxSide = BOX_LEFT;
    else if(id.y == 1) boxSide = BOX_TOP;
    else boxSide = BOX_BOTTOM;
    mRenderer = new RenderSubPlanet(parentPlanet->getNoiseGenerator(), boxSide);
}

DRReturn SubPlanetSektor::move(float fTime, Camera* cam)
{
    if(mParent)
    {
        if(!mParent->isObjectInSektor(cam->getSektorPosition()))
            mIdleSeconds += fTime;
        else
            mIdleSeconds = 0.0f;
    }
    return DR_OK;
}

DRReturn SubPlanetSektor::render(float fTime, Camera* cam)
{
    double faktor = 1.0/Wurzel_3;
    //glScalef(faktor, faktor, faktor);
    //glTranslatef(mID.x, mID.y, mID.z);
    mRenderer->render(fTime, cam);
    return DR_OK;
}