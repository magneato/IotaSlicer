//
//  IACamera.cpp
//
//  Copyright (c) 2013-2018 Matthias Melcher. All rights reserved.
//


#include "IACamera.h"

#include "Iota.h"
#include "IASceneView.h"
#include "../geometry/IAMath.h"

#include <FL/gl.h>
#include <FL/glu.h>

#include <math.h>

// FIXME: where does MS define M_PI?
#ifndef M_PI
#define M_PI 3.141592654
#endif


/**
 * Create a camera superclass.
 */
IACamera::IACamera(IASceneView *view)
:   pView( view )
{
}


/**
 * Create a simple perspective camera.
 */
IAPerspectiveCamera::IAPerspectiveCamera(IASceneView *view)
:   super( view )
{
}


/**
 * User wants to rotate the camera.
 */
void IAPerspectiveCamera::rotate(double dx, double dy)
{
    pZRotation += dx/100.0;
    pXRotation += dy/100.0;

    while (pZRotation>=M_PI*2.0) pZRotation -= M_PI*2.0;
    while (pZRotation<0.0) pZRotation += M_PI*2.0;

    if (pXRotation>M_PI*0.48) pXRotation = M_PI*0.48;
    if (pXRotation<-M_PI*0.48) pXRotation = -M_PI*0.48;
}


/**
 * User wants to drag the camera around.
 */
void IAPerspectiveCamera::drag(double dx, double dy)
{
    IAVector3d position = IAVector3d(0.0, -pDistance, 0.0);
    position.xRotate(pXRotation);
    position.zRotate(pZRotation);
    position += pInterest;

    IAVector3d printer = Iota.gPrinter.pBuildVolume;
    printer *= 0.5;
    printer -= position;
    double dist = printer.length();

    IAVector3d offset(dx, 0, -dy);
    // using pDistance works as well, but is not as nice
    offset *= 1.8*dist/(pView->w()+pView->h());
    offset.xRotate(pXRotation);
    offset.zRotate(pZRotation);
    pInterest += offset;
}


/**
 * User wants to get closer to the point of interest.
 */
void IAPerspectiveCamera::dolly(double dx, double dy)
{
    pDistance = pDistance * (1.0+0.01*dy);
    if (pDistance<5.0) pDistance = 5.0;
}


/**
 * Emit OpenGL commands to load the viewing and model matrices.
 */
void IAPerspectiveCamera::draw()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    IAVector3d position = IAVector3d(0.0, -pDistance, 0.0);
    position.xRotate(pXRotation);
    position.zRotate(pZRotation);
    position += pInterest;

    IAVector3d printer = Iota.gPrinter.pBuildVolume;
    printer *= 0.5;
    printer -= position;
    double dist = printer.length();
    double aspect = (double(pView->pixel_w()))/(double(pView->pixel_h()));
    double nearPlane = max(dist-2.0*Iota.gPrinter.pBuildVolumeRadius, 1.0);
    double farPlane = dist+2.0*Iota.gPrinter.pBuildVolumeRadius;
    gluPerspective(50.0, aspect, nearPlane, farPlane);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(position.x(), position.z(), -position.y(),
              pInterest.x(), pInterest.z(), -pInterest.y(),
              0.0, 1.0, 0.0);
    glRotated(-90, 1.0, 0.0, 0.0);
}


void IAPerspectiveCamera::setInterest(IAVector3d &v)
{
    pInterest = v;
}



/**
 * Create a simple perspective camera.
 */
IAOrthoCamera::IAOrthoCamera(IASceneView *view, int direction)
:   super( view )
{
}


/**
 * User wants to rotate the camera.
 */
void IAOrthoCamera::rotate(double dx, double dy)
{
    // Do we want to allow rotation?
}


/**
 * User wants to drag the camera around.
 */
void IAOrthoCamera::drag(double dx, double dy)
{
    IAVector3d offset(dx, -dy, 0);
    offset *= 4.0*pZoom/(pView->w()+pView->h());
    pInterest += offset;
}


/**
 * User wants to get closer to the point of interest.
 */
void IAOrthoCamera::dolly(double dx, double dy)
{
    pZoom = pZoom * (1.0+0.01*dy);
    if (pZoom<1.0) pZoom = 1.0;
    if (pZoom>4.0*Iota.gPrinter.pBuildVolumeRadius) pZoom = 4.0*Iota.gPrinter.pBuildVolumeRadius;
}


/**
 * Emit OpenGL commands to load the viewing and model matrices.
 */
void IAOrthoCamera::draw()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double aspect = (double(pView->pixel_w()))/(double(pView->pixel_h()));
    glOrtho(-pZoom*aspect, pZoom*aspect, -pZoom, pZoom, -Iota.gPrinter.pBuildVolumeRadius, Iota.gPrinter.pBuildVolumeRadius);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(-pInterest.x(), -pInterest.y(), -pInterest.z());
}


void IAOrthoCamera::setInterest(IAVector3d &v)
{
    pInterest = v;
}


