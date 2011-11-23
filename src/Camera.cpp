/* 
 * File:   Camera.cpp
 * Author: dario
 * 
 * Created on 20. August 2011, 21:17
 */

#include "main.h"

Camera::Camera()
: mSektorPosition(Unit(0, AE)), mCurrentSektor(NULL)
{
}

Camera::Camera(const DRVector3& position, Sektor* sektor) 
: DRObjekt(position), mSektorPosition(Unit(0, NONE)), mCurrentSektor(sektor)
{
}

Camera::~Camera() 
{
    mCurrentSektor = NULL;    
}

void Camera::setKameraMatrix()
{
    glMultMatrixf(mMatrix);
}

void Camera::setKameraMatrixRotation()
{
    glMultMatrixf(DRMatrix::axis(mXAxis, mYAxis, mZAxis));
}

void Camera::translateRel_SektorPosition(const DRVector3& translate, const UnitTypes& type)
{
    DRMatrix m1 = DRMatrix::axis(mXAxis, mYAxis, mZAxis);
    DRMatrix m2 = DRMatrix::translation(mSektorPosition.getVector3());
    DRMatrix m = DRMatrix(m1) * DRMatrix(m2);
    mSektorPosition += Vector3Unit(translate.transformNormal(m), type);
    //mAbsPosition.print();
  /*  printf("\r x-axis: %f %f %f, y-axis: %f %f %f, z-axis: %f %f %f, abs: %s %s %s", mXAxis.x, mXAxis.y, mXAxis.z,
                                                    mYAxis.x, mYAxis.y, mYAxis.z,
                                                    mZAxis.x, mZAxis.y, mZAxis.z,       
                                                    mAbsPosition.x.print().data(), mAbsPosition.y.print().data(),
                                                    mAbsPosition.z.print().data());
 // * */
}
void Camera::translateRel(const DRVector3& translate)
{
    DRObjekt::translateRel(translate);
    //printf("\r camera: %f %f %f l:%f", mPosition.x, mPosition.y, mPosition.z, mPosition.length());
    
    if(mPosition.lengthSq() > 1000.0f*1000.0f)
    {
        mSektorPosition += Vector3Unit(mPosition/1000.0f, KM);
        mPosition = 0.0f;
        update();
        printf("update sektor\n");
    }
   
}

void Camera::update()
{
 //   printf("\r camera: %s %s %s", mSektorPosition.x.print().data(), mSektorPosition.y.print().data(),
   //                                                 mSektorPosition.z.print().data());
    DRMatrix m1 = DRMatrix::axis(mXAxis, mYAxis, mZAxis);
    DRMatrix m2 = DRMatrix::translation(-mPosition);
    //mMatrix = DRMatrix(m1) * DRMatrix(m2);
    mMatrix = DRMatrix(m2) * DRMatrix(m1);
   // mMatrix.print();
}