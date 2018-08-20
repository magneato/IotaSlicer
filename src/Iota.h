//
//  Iota.h
//
//  Copyright (c) 2013-2018 Matthias Melcher. All rights reserved.
//

#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <memory>

#include "geometry/IAMesh.h"
#include "geometry/IASlice.h"
#include "printer/IAPrinter.h"

class IAGeometryReader;
class IAMachineToolpath;
class IAToolpath;


// TODO globals that we want to get rid of.

const int IA_PROJECTION_FRONT       = 0;
const int IA_PROJECTION_CYLINDER    = 1;
const int IA_PROJECTION_SPHERE      = 2;


/**
 * List of errors that the user may encounter.
 */
enum class Error {
    NoError = 0,
    CantOpenFile_STR_BSD,
    UnknownFileType_STR,
    FileContentCorrupt_STR,
	OpenGLFeatureNotSupported_STR,
};


/**
 * The main class that manages this app.
 */
class IAIota
{
public:
    IAIota();
    ~IAIota();

    void menuWriteSlice();
    void menuQuit();

    void loadAnyFile(const char *list);

    void clearError();
    void setError(const char *loc, Error err, const char *str=nullptr);
    bool hadError();
    Error lastError() { return pError; }
    void showError();

private:
    bool addGeometry(const char *name, uint8_t *data, size_t size);
    bool addGeometry(const char *filename);
    bool addGeometry(std::shared_ptr<IAGeometryReader> reader);

public:
    /// the main UI window
    /// \todo the UI must be managed in a UI class (Fluid can do that!)
    class Fl_Window *gMainWindow = nullptr;
    /// the one and only texture we currently support
    /// \todo move this into a class and attach it to models
    class Fl_RGB_Image *texture = nullptr;
    /// the one and only mesh we currently support
    /// \todo move meshes into a model class, and models into a modelList
    IAMesh *pMesh = nullptr;
    /// the one slice that we generate
    /// \todo create a current slice and hashed slices for other z-layers
    IASlice gMeshSlice;
    /// the current 3d printwer
    IAPrinter gPrinter;
    /// the toolpath for the entire scene
    IAMachineToolpath *pMachineToolpath = nullptr;
    /// the current toolpath
    IAToolpath *pCurrentToolpath = nullptr;
    /// show the slice in the 3d view
    /// \todo move to UI class
    bool gShowSlice;
    /// show the texture in the 3d view
    /// \todo move to UI class
    bool gShowTexture;

private:
    /// user definable string explaining the details of an error
    const char *pErrorString = nullptr;
    /// user definable string explaining the function that caused an error
    const char *pErrorLocation = nullptr;
    /// current error condition
    Error pError = Error::NoError;
    /// system specific error number of the call that was markes by an error
    int pErrorBSD = 0;
    /// list of error messages
    static const char *kErrorMessage[];
};


extern IAIota Iota;


#endif /* MAIN_H */
