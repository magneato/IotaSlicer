//
//  IAVector3d.h
//
//  Copyright (c) 2013-2018 Matthias Melcher. All rights reserved.
//


#ifndef IA_VECTOR3D_H
#define IA_VECTOR3D_H


/**
 A small class to handle vectors in 3D space.
 */
class IAVector3d
{
public:
    IAVector3d();
    IAVector3d(const IAVector3d&);
    IAVector3d(double*);
    IAVector3d(double, double, double);
    IAVector3d& operator-=(const IAVector3d&);
    IAVector3d& operator+=(const IAVector3d&);
    IAVector3d& operator*=(double);
    double *dataPointer() { return pV; }
    IAVector3d& cross(const IAVector3d&);
    double normalize();
    void zero();
    void write(double*) const;
    void read(float*);
    void read(double*);
    double length() const;
    double x() const { return pV[0]; }
    double y() const { return pV[1]; }
    double z() const { return pV[2]; }
    void set(double, double, double);
    void xRotate(double);
    void yRotate(double);
    void zRotate(double);

    double pV[3] = { 0.0, 0.0, 0.0 };
};


#endif /* IA_VECTOR3D_H */