#include "RenderSubPlanet.h"
#include "DRGeometrieManager.h"
#include "ShaderManager.h"

RenderSubPlanet::RenderSubPlanet(SektorID seed, DRVector3 translate, float patchScaling, const DRMatrix& rotation, DRString texturePath, const PlanetNoiseParameter* planetNoiseParameter)
: RenderPlanet(seed, translate, patchScaling, rotation, texturePath, planetNoiseParameter), mGeometrieGrid(NULL)
{
    mGeometrieGrid = DRGeometrieManager::Instance().getGrid(100, GEO_FULL);
    if(mShader) ShaderManager::Instance().releaseShader(mShader);
	mShader = ShaderManager::Instance().getShader("sphere.vert", "sphere.frag");
}

RenderSubPlanet::~RenderSubPlanet()
{
    //DR_SAVE_DELETE(mGeometrieGrid);
}

DRReturn RenderSubPlanet::render(float fTime, Camera* cam)
{   
    generateAndBindTexture();

    if(mGeometrieGrid->render()) 
        LOG_ERROR("Fehler bei Geometrie Grid", DR_ERROR);
    
    if(DRGrafikError("[RenderSubPlanet::render]")) return DR_ERROR;
    return DR_OK;
}


