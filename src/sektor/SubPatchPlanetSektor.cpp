#include "SubPatchPlanetSektor.h"
#include "GlobalRenderer.h"

SubPatchPlanetSektor::SubPatchPlanetSektor(Vector3Unit position, Unit radius, 
                                           SektorID id, Sektor* parent, PlanetSektor* planet,
                                           float patchScaling/* = 1.0f*/, int subLevel/* = 7*/)
: SubPlanetSektor(position, radius, id, parent, planet, patchScaling, subLevel), mVectorToPlanetCenter(DRVector3(0.0f, 0.0f, -1.0f))
{
    mType = SUB_PATCH_PLANET;  
    DRVector3 childPos(mID.x, mID.y, mID.z);
    childPos /= 1000.0f;
             
    DRMatrix rotation = dynamic_cast<SubPlanetSektor*>(mParent)->getRotation();
    //mRotation = mRotation * rotation;
    mVectorToPlanetCenter = (mParent->getPosition()+mSektorPosition).getVector3().normalize();
    printf("[SubPatchPlanetSektor::SubPatchPlanetSektor] vector to planet center: %f %f %f\n",
            mVectorToPlanetCenter.x, mVectorToPlanetCenter.y, mVectorToPlanetCenter.z);
    mRenderer = new RenderSubPatchPlanet(id, childPos, patchScaling, rotation, getSektorPathName(), mPlanet->getPlanetNoiseParameters());
}

 SubPatchPlanetSektor::~SubPatchPlanetSektor()
 {
     //DRLog.writeToLog("deconstruct SubPatchPlanetSektor");
 }

 DRReturn SubPatchPlanetSektor::move(float fTime, Camera* cam)
 {
    if(mSubLevel != 7) return DR_OK;
    //teilen bei Camera Distance von 1.5 radius
    mLastRelativeCameraPosition = cam->getSektorPositionAtSektor(this).convertTo(KM);
  //  mLastRelativeCameraPosition.convertTo(KM).print("[SubPatchPlanetSektor::move] camera Position");
    DRVector3 patchPosition = mSektorPosition.getVector3().normalize();
    DRVector3 cameraPosition = mLastRelativeCameraPosition.getVector3().normalize();
    if(EnIsButtonPressed(SDLK_y))
        cam->setSektorPosition(DRVector3(0.0f));
    mHorizontCulling = acos(cameraPosition.dot(patchPosition))*RADTOGRAD;  
    if(mParent)
    {
        if(!isObjectInSektor(mLastRelativeCameraPosition))    
            mIdleSeconds += fTime;
        else
            mIdleSeconds = 0.0f;
    }
    
    if(mHorizontCulling <= 50.0)
    {
        /*///sub sektoren erstellen
        getChild(SektorID(-500, -500, 0)); // rechts unten
        getChild(SektorID( 500, -500, 0)); // links unten
        getChild(SektorID(-500,  500, 0)); //rechts oben
        getChild(SektorID(-500, -500, 0)); //rechts unten
    //* */
    }
    else
    {
        removeInactiveChilds(60.0f);
    }
    return DR_OK;
 }
    
   
 DRReturn SubPatchPlanetSektor::render(float fTime, Camera* cam)
 {
     if(mIdleSeconds > 0.0f) return DR_OK;
    //DRVector3 pos = mSektorPosition.getVector3().normalize();
    
    //glMultMatrixf(mRotation);
    //glTranslatef(0.0f, 0.0f, 1.0f-mEbeneDistanceToCenter);
     DRVector3 childPos(mID.x, mID.y, mID.z);
     childPos /= 1000.0f;
     
    mMatrix = /*DRMatrix::translation(childPos) */ mParent->getMatrix();//  mParent->getMatrix();
    ShaderProgram* shader = mRenderer->getShaderProgram();
    if(!shader) LOG_ERROR("renderer shader isn't valid", DR_ERROR);
    shader->bind();
    shader->setUniformMatrix("projection", GlobalRenderer::Instance().getProjectionMatrix());
    shader->setUniformMatrix("modelview", mMatrix);
    shader->setUniform3fv("translate", childPos);
    
    //if(mPlanet->getHorizontAngle() > 50.0)
    {
        //shader->setUniform1f("theta", mTheta);
        shader->setUniform1f("patchScaling", mPatchScaling);
        mRenderer->render(fTime, cam);
        shader->unbind();
        //GlobalRenderer::getSingleton().getPlanetShaderPtr()->unbind();
        // childs didn't need to render
        return DR_NOT_ERROR;
    }
    return DR_OK;
 }
 
 bool SubPatchPlanetSektor::isObjectInSektor(Vector3Unit positionInSektor)
{    
    //double angle = acos(positionInSektor.normalize().dot(mSektorPosition.getVector3().normalize()));   
    DRVector3 posInSektorNorm = positionInSektor.getVector3().normalize();
    DRVector3 sektorPosNorm = mSektorPosition.getVector3().normalize();
    double angle = acos(posInSektorNorm.dot(mVectorToPlanetCenter));   
    double d = posInSektorNorm.dot(mVectorToPlanetCenter);
    //positionInSektor.print("sektorpos");
    //printf("\r angle: %f (dot: %f)", angle*RADTOGRAD, d);
    //*/
    if(angle*RADTOGRAD > 110)
        return false;
    return true;   
}