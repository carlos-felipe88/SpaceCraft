#ifndef __SC_RENDER_NOISE_PLANET_TO_TEXTURE__
#define __SC_RENDER_NOISE_PLANET_TO_TEXTURE__

#include "RenderInStepsToTexture.h"
#include "ShaderProgram.h"
class DRGeometrieHeightfield;

class RenderNoisePlanetToTexture : public RenderInStepsToTexture
{
public:
    RenderNoisePlanetToTexture(const char* vertexShaderName, const char* fragmentShaderName);
    ~RenderNoisePlanetToTexture();
    
    //__inline__ void update(float patchScaling) {mPatchScaling = patchScaling;}
    DRReturn init(float stepSize, DRVector3 translate, float patchScaling, Texture* texture, const DRMatrix& rotation = DRMatrix::identity());
    
    virtual DRReturn renderStuff();
    
    __inline__ const DRMatrix& getRotationsMatrix() {return mRotation;}
    __inline__ float getPatchScaling() {return mPatchScaling;}

    
protected:
private:
    ShaderProgram* mShader;
    DRGeometrieHeightfield* mRenderSphere;
    float         mPatchScaling;
    DRVector3     mTranslate;
    DRMatrix      mRotation;
};

#endif //__SC_RENDER_NOISE_PLANET_TO_TEXTURE__