//
//  IAPrinterFDM.h
//
//  Copyright (c) 2013-2018 Matthias Melcher. All rights reserved.
//

#ifndef IA_PRINTER_FDM_H
#define IA_PRINTER_FDM_H


#include "printer/IAPrinter.h"


class Fl_Widget;
class Fl_Choice;
class Fl_Input_Choice;


/**
 * FDM, Fused Deposition Modeling, describes printers that extrude filament
 * to create 3D models.
 *
 * This printer driver writes GCode files.
 */
class IAPrinterFDM : public IAPrinter
{
    typedef IAPrinter super;
public:
    // ---- constructor and destructor
    IAPrinterFDM();
    IAPrinterFDM(IAPrinterFDM const& src);
    virtual ~IAPrinterFDM() override;
    IAPrinterFDM &operator=(IAPrinterFDM&) = delete;
    virtual IAPrinter *clone() const override;
    virtual const char *type() const override { return "IAPrinterFDM"; }

    // ---- direct user interaction
    virtual void userSliceSave() override;
    virtual void userSliceSaveAs() override;
    virtual void userSliceGenerateAll() override;

    virtual void purgeSlicesAndCaches() override;
    virtual void drawPreview(double lo, double hi) override;

    // ---- controllers
    virtual void createPropertiesControls() override;

    // ---- properties
    virtual void readProperties(Fl_Preferences &p) override;
    virtual void writeProperties(Fl_Preferences &p) override;

    IAIntProperty numExtruders { "numExtruders", 1 };
    // ex 0 type
    // ex 0 nozzle diameter
    // ex 0 feeds
    // ex 0 feed 0 material

    // ---- scene settings
    virtual void initializeSceneSettings() override;


    // printer
    IAFloatProperty nozzleDiameter { "nozzleDiameter", 0.4 };
    // construction
    IAIntProperty numShells { "numShells", 3 };
    IAIntProperty numLids { "numLids", 2 };
    IAIntProperty lidType { "lidType", 0 }; // 0=zigzag, 1=concentric
    IAFloatProperty infillDensity { "infillDensity", 20.0 }; // %
    IAIntProperty hasSkirt { "hasSkirt",  1 }; // prime line around perimeter
    IAFloatProperty minimumLayerTime { "minimumLayerTime", 15.0 };
    IAExtruderProperty modelExtruder { "modelExtruder", 0 };
    // support
    IAIntProperty hasSupport { "hasSupport", 1 };
    IAFloatProperty supportAngle { "supportAngle", 50.0 };
    IAFloatProperty supportDensity { "supportDensity", 40.0 };
    IAFloatProperty supportTopGap { "supportTopGap", 1.0 };
    IAFloatProperty supportSideGap { "supportSideGap", 0.2 };
    IAFloatProperty supportBottomGap { "supportBottomGap", 1.0 };
    IAExtruderProperty supportExtruder { "supportExtruder", 0 };
    // models and meshes

    // ----
    void sliceAll();
    void saveToolpath(const char *filename = nullptr);

    double filamentDiameter() { return 1.75; }

protected:
    void userChangedColorMode();
    void userChangedNumLids();
    void userChangedLidType();
    void userChangedNumShells();
    void userChangedInfillDensity();
    void userChangedSkirt();
    void userChangedNozzleDiameter();

private:

    /// the toolpath for the entire scene for vector based machines
    IAMachineToolpath pMachineToolpath = IAMachineToolpath(this);
};


class IALayerMapFDM;

/**
 * Layers foor FDM printers contain all data from slice to toolpath.
 */
class IALayerFDM
{
public:
    IALayerFDM(IALayerMapFDM*, double z, double height);
    ~IALayerFDM();
    void createToolpath();
    void createInfill();
    void createLid();
    void createBottom();
    void createShells();
    void createSLice();
private:
    IASlice *pSlice;
    IAFramebuffer *pFBSlice;
    IAFramebuffer *pFBCore; // slice without shell
};


class IALayerMapFDM
{
    IALayerMapFDM(IAPrinterFDM*);
    ~IALayerMapFDM();
    std::map<IALayerFDM, double> pMap;
};



#endif /* IA_PRINTER_FDM_H */


