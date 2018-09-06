//
//  IAPrinter.cpp
//
//  Copyright (c) 2013-2018 Matthias Melcher. All rights reserved.
//


#include "IAPrinterFDM.h"

#include "Iota.h"
#include "userinterface/IAGUIMain.h"
#include "toolpath/IAToolpath.h"
#include "opengl/IAFramebuffer.h"


#include <FL/Fl_Native_File_Chooser.H>
#include <FL/filename.H>



void IAPrinterFDM::userSliceAs()
{
    if (queryOutputFilename("Save toolpath as GCode", "*.gcode", ".gcode")) {
        sliceAndWrite();
    }
}


void IAPrinterFDM::userSliceAgain()
{
    if (pFirstWrite) {
        userSliceAs();
    } else {
        sliceAndWrite();
    }
}


void IAPrinterFDM::sliceAndWrite(const char *filename)
{
    if (!filename)
        filename = outputPath();

    if (!Iota.pMachineToolpath)
        Iota.pMachineToolpath = new IAMachineToolpath();
    else
        Iota.pMachineToolpath->clear();
    double hgt = Iota.pMesh->pMax.z() - Iota.pMesh->pMin.z();
    // initial height determines stickiness to bed

    double zMin = 0.2;
    double zLayerHeight = 0.3;
#if 1
    double zMax = hgt;
#else
    double zMax = 25;
#endif

    showProgressDialog();
    char buf[1024];
    strcpy(buf, "Genrating slices");
    wProgressText->copy_label(buf);
    wProgressValue->value(0);
    int i = 0, n = (int)((zMax-zMin)/zLayerHeight);

    for (double z=zMin; z<zMax; z+=zLayerHeight) {
        printf("Slicing at z=%g\n", z);

        sprintf(buf, "Slicing layer %d of %d at %.3fmm (%d%%)", i, n, z, i*100/n);
        wProgressText->copy_label(buf);
        wProgressValue->value(i*100/n);
        bool abort = updateProgressDialog();
        if (abort) break;

        Iota.gMeshSlice.changeZ(z);
        Iota.gMeshSlice.clear();
        Iota.gMeshSlice.generateRim(Iota.pMesh);
        Iota.gMeshSlice.tesselateLidFromRim();
        Iota.gMeshSlice.drawFlat(false, 1, 1, 1);

        uint8_t *rgb = Iota.gMeshSlice.pColorbuffer->getRawImageRGB();

        IAToolpath *tp1 = new IAToolpath(z);
        Iota.gMeshSlice.pFramebuffer->traceOutline(tp1, z);
        //        Iota.gMeshSlice.pFramebuffer->saveAsJpeg("/Users/matt/a1.jpg");

        IAToolpath *tp2 = new IAToolpath(z);
        Iota.gMeshSlice.pFramebuffer->bindForRendering();
        glDisable(GL_DEPTH_TEST);
        glColor3f(0.0, 0.0, 0.0);
        tp1->drawFlat(4);
        //        Iota.gMeshSlice.pFramebuffer->saveAsJpeg("/Users/matt/a2.jpg");
        Iota.gMeshSlice.pFramebuffer->unbindFromRendering();
        Iota.gMeshSlice.pFramebuffer->traceOutline(tp2, z);

        IAToolpath *tp3 = new IAToolpath(z);
        Iota.gMeshSlice.pFramebuffer->bindForRendering();
        glDisable(GL_DEPTH_TEST);
        glColor3f(0.0, 0.0, 0.0);
        tp2->drawFlat(4);
        //        Iota.gMeshSlice.pFramebuffer->saveAsJpeg("/Users/matt/a3.jpg");
        Iota.gMeshSlice.pFramebuffer->unbindFromRendering();
        Iota.gMeshSlice.pFramebuffer->traceOutline(tp3, z);

        IAToolpath *tp = Iota.pMachineToolpath->createLayer(z);
#if 1
        tp->add(*tp3);
        tp->add(*tp2);
        tp->add(*tp1);
#else
        IAToolpath *b1 = new IAToolpath(z);
        IAToolpath *w1 = new IAToolpath(z);
        IAToolpath *b2 = new IAToolpath(z);
        IAToolpath *w2 = new IAToolpath(z);
        tp2->colorize(rgb, b1, w1);
        tp3->colorize(rgb, b2, w2);
        tp->pList.push_back(new IAToolpathExtruder(0));
        tp->add(*w2);
        tp->add(*w1);
        tp->pList.push_back(new IAToolpathExtruder(1));
        tp->add(*b2);
        tp->add(*b1);
        delete b1;
        delete w1;
        delete b2;
        delete w2;
#endif
        delete tp1;
        delete tp2;
        delete tp3;
        free(rgb);
        i++;
    }
    Iota.pMachineToolpath->saveGCode(filename);
    hideProgressDialog();
    zSlider1->value(0.0);
    zSlider1->do_callback();
    gSceneView->redraw();
}