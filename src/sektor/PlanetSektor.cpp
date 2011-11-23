#include "main.h"

PlanetSektor::PlanetSektor(Vector3Unit position, Unit radius, SektorID id, Sektor* parent)
: Sektor(position, radius, id, parent)
{
    mType = PLANET;
    
    mNoiseGenerator = new GenerateNoisePlanet();
        
	noise::module::Perlin p;
	if(id.count) p.SetSeed(id.count);
	DRVector3 idVector(id.x, id.y, id.z);
	idVector /= SHRT_MAX;
    mNoiseGenerator->setupGenerator((int)(p.GetValue(idVector.x, idVector.y, idVector.z)*INT_MAX));
    mRenderer = new RenderPlanet(mNoiseGenerator);
}

PlanetSektor::~PlanetSektor()
{
    DR_SAVE_DELETE(mRenderer);
    DR_SAVE_DELETE(mNoiseGenerator);
}

DRReturn PlanetSektor::move(float fTime, Camera* cam)
{
    if(isObjectInSektor(cam->getSektorPosition()))
    {
        SektorID subPlanets[6] = {SektorID(0,0,-1),SektorID(1,0,0),SektorID(0,0,1),
                                  SektorID(-1,0,0),SektorID(0,1,0),SektorID(0,-1,0)};
        // subPlanet seiten eines Würfels mit Kantenlänge 2 (intern)
        // Kantenpunktlänge = Wurzel(3), radius = 1, Quadratmittelpunktlänge = 1
        // Wurzel(3)       1
        //  ------    =  -----   => Wurzel(3) = 1/faktor => faktor = 1/Wurzel(3)
        //     1         faktor
        double faktor = 1.0/SubPlanetSektor::Wurzel_3;// sqrtf(3.0);
        for(int i = 0; i < 6; i++)
        {
            if(mChilds.find(subPlanets[i]) == mChilds.end())
            {
                printf("not ");
                // Position des Quadratmittelpunktes
                Vector3Unit position = Vector3Unit(subPlanets[i].x, subPlanets[i].y, subPlanets[i].z, KM)*(mRadius*faktor);
                // Entfernung von Quadratmittelpunkt halbkreis punkt
                Unit radius = mRadius * (1.0 - faktor);
                SubPlanetSektor* temp = new SubPlanetSektor(position, radius, subPlanets[i], this);
                mChilds.insert(SEKTOR_ENTRY(subPlanets[i], temp));
            }
        }
    }
    else
    {
        removeInactiveChilds(60.0f);
    }
    return DR_OK;
}

DRReturn PlanetSektor::render(float fTime, Camera* cam)
{
	Unit distance1 = Vector3Unit(mSektorPosition - cam->getSektorPosition()).length();
	DRVector3 diff = Vector3Unit(mSektorPosition - cam->getSektorPosition()).convertTo(KM).getVector3();

	distance1 = distance1.convertTo(mRadius.getType());
	double distance2 = 200.0f;
	Unit radius1 = mRadius;
	double radius2 = ((radius1 * distance2) / distance1);
    
	if(radius2 > 160.0f) mRenderer->setCurrentDetail(10);
	else if(radius2 > 140.0f) mRenderer->setCurrentDetail(9);
	else if(radius2 > 120.0f) mRenderer->setCurrentDetail(8);
	else if(radius2 > 90.0f) mRenderer->setCurrentDetail(7);
	else if(radius2 > 70.0f) mRenderer->setCurrentDetail(6);
	else if(radius2 > 30.0f) mRenderer->setCurrentDetail(5);
	else if(radius2 > 25.0f) mRenderer->setCurrentDetail(4);
	else if(radius2 > 15.0f) mRenderer->setCurrentDetail(3);
	else if(radius2 > 5.0f) mRenderer->setCurrentDetail(2);
	else if(radius2 > 1.0f) mRenderer->setCurrentDetail(1);
	else mRenderer->setCurrentDetail(0);

	DRVector3 pos = (mSektorPosition - cam->getSektorPosition()).getVector3().normalize();
	DRVector3 relCameraPos = -pos*distance1/mRadius;
	pos *= distance2;
/*   printf("\r %f %f %f, %.8f, %s  x:%s y:%s z:%s (%f %f %f)", pos.x, pos.y, pos.z, radius2, distance1.print().data(),
									   absCameraPosition.x.print().data(), absCameraPosition.y.print().data(),
									   absCameraPosition.z.print().data(), diff.x, diff.y, diff.z);
										   //*/


	glTranslatef(pos.x, pos.y, pos.z);
    glScalef(radius2, radius2, radius2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(mRenderer && !isObjectInSektor(cam->getSektorPosition()))
	{
//      GlobalRenderer::getSingleton().getPlanetShaderPtr()->bind();
        //glPushMatrix();
        
		DRReturn ret = mRenderer->render(fTime, cam);
		GlobalRenderer::getSingleton().getPlanetShaderPtr()->unbind();
        //glPopMatrix();
		if(ret) LOG_ERROR("Fehler bei call planet renderer", DR_ERROR);
        //child didn't need to render
        return DR_NOT_ERROR;
	}    
    
    return DR_OK;
}

bool PlanetSektor::isObjectInSektor(Vector3Unit positionInParentSektor)
{
    Unit radiusSquare = mRadius.convertTo(AE)*6.0;
    radiusSquare *= radiusSquare;
	Unit distance = Vector3Unit(positionInParentSektor - mSektorPosition).length();
    return Vector3Unit(positionInParentSektor.convertTo(AE) - mSektorPosition).lengthSq() <= radiusSquare;
}