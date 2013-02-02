#ifndef __SC_RENDER_IN_STEPS_TO_TEXTURE__
#define __SC_RENDER_IN_STEPS_TO_TEXTURE__

#include "RenderToTexture.h"

class RenderInStepsToTexture: public RenderToTexture
{
public:
    // frame buffer, will be called in init and in step
    RenderInStepsToTexture();
    virtual ~RenderInStepsToTexture();
    
    // reset values, set step size
    virtual DRReturn init(int stepSize, DRTexturePtr texture);
    //! render with same setup, but other texture as render-target
    virtual DRReturn reinit(DRTexturePtr texture);
    // render with current step, add step
    //! save image to file after finish render, if filname was set
    virtual DRReturn step();
    
  //  __inline__ void setFilenameToSave(DRString path) {mFileNameToSave = path;}
    
    // called every step
   // virtual DRReturn renderStuff() = 0;        
    
    virtual bool isFinished() {return mFinished == 1;}
    //__inline__ DRTexturePtr getTextur() {return mTexture;}
    
    virtual const char* getResourceType() const {return "RenderInStepsToTexture";}
    
    __inline__ void setCurrentDistance(DRReal distance) {mDistance = distance;}
    
    virtual bool less_than(DRIResource& b) const
    {
        //printf("[RenderInStepsToTexture::operator <]\n");
        return this->mDistance < dynamic_cast<RenderInStepsToTexture&>(b).mDistance;
    }
    
protected:
    //DRVector2i  mTextureSize;    
    //DRTexturePtr  mTexture;
    //DRMatrix	mProjectionMatrix;
    
private:
    DRVector2i  mCursorIndex;
    int         mStepSize;
    u8          mIndexStepMode; 
    short       mCursorMaxCount;
    short       mCursorCurrentCount;
    //bool        mFinished;
    
    DRReal       mDistance; //distance to viewer, for priority list
  //  DRString     mFileNameToSave;
};

//typedef DRResourcePtr<RenderInStepsToTexture> RenderInStepsToTexturePtr;

#endif //__SC_RENDER_IN_STEPS_TO_TEXTURE__