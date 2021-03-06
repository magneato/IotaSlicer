//
//  IAGcodeWriter.h
//
//  Copyright (c) 2013-2018 Matthias Melcher. All rights reserved.
//

#ifndef IA_GCODE_WRITER_H
#define IA_GCODE_WRITER_H


#include "geometry/IAVector3d.h"
#include "app/IAMacros.h"

#include <math.h>
#include <vector>
#include <map>


class IAFDMPrinter;


/**
 * Helps the toolpath classes to write GCode
 */
class IAGcodeWriter
{
public:
    IAGcodeWriter(IAFDMPrinter *printer);
    ~IAGcodeWriter();

    bool open(const char *filename);
    void close();

//    /** \todo save and update pEFactor */
//    void setFilamentDiameter(double d);
//    /** \todo save and update pEFactor */
//    void setLayerHeight(double d);
    /** Set the default feedrate for rapid moves. */
    void setRapidFeedrate(double feedrate);
    /** Set the default feedrate for printing moves. */
    void setPrintFeedrate(double feedrate);

    void resetTotalTime();
    double getTotalTime();
    void resetLayerTime();
    double getLayerTime();

    /** Position of the extruder head.
     \return the position of the current extruder's tip. */
    IAVector3d &position() { return pPosition; }

    void sendInitSequence(unsigned int toolmap);
    void sendShutdownSequence();

    void requestTool(int i);

    void cmdHome();
    void cmdResetExtruder();
    void cmdExtrude(double distance, double feedrate=-1.0);
    void cmdComment(const char *format, ...);
    void cmdRapidMove(double x, double y);
    void cmdRapidMove(IAVector3d &v);
    void cmdRetractMove(double x, double y);
    void cmdRetractMove(IAVector3d &v);
    void cmdPrintMove(double x, double y);
    void cmdPrintMove(IAVector3d &v);
    void cmdRetract(double d=1.0);
    void cmdUnretract(double d=1.0);
    void cmdDwell(double seconds);

private:
    DEPRECATED("This call does not work yet!")
    void cmdSelectExtruder(int);
    DEPRECATED("This call does not work yet!") 
    void cmdExtrudeRel(double distance, double feedrate=-1.0);
    DEPRECATED("This call does not work yet!")
    void cmdMove(IAVector3d &v, uint32_t color, double feedrate=-1.0);
    DEPRECATED("This call does not work yet!")
    void sendPurgeExtruderSequence(int t);
    void sendMoveTo(const IAVector3d &v);
    void sendRapidMoveTo(IAVector3d &v);
    void sendPosition(const IAVector3d &v);
    void sendFeedrate(double f);
    void sendExtrusionAdd(double e);
    void sendExtrusionRel(uint32_t color, double e);
    void sendNewLine(const char *comment=nullptr);

    IAFDMPrinter *pPrinter = nullptr;
    FILE *pFile = nullptr;
    IAVector3d pPosition;
    int pT = 0;
    double pE = 0.0;
    double pF = 0.0;
    double pRapidFeedrate = 3000.0;
    double pPrintFeedrate = 1000.0; // 1400.0
    double pLayerHeight = 0.3;
    unsigned int pToolmap = 0; // fill this list with bit for every tool used in the process
    int pToolCount = 0; // number of tools used

    int pExtruderStandbyTemp = 205;
    int pExtruderPrintTemp = 230;

    /*
     Calculating the E-Factor:
     When sending a G1 command, X, Y and Z give the distance of the move in mm.
     E is the distance in mm that the filament will be moved.
     If the E factor was 1, and there was no hotend, G1 would extrude exactly
     the length of filament corresponding to the XYZ motion.

     So, the incoming filament is 1.75mm in diameter. That gives an area
     of PI*r^2 = (1.75/2)^2*pi = 2.41 . With a nozzle diameter of 0.4mm and
     a layer heigt of 0.3mm, we want an area of 0.4*0.3 = 0.12 .
     Dividing the surface areas 2.41/0.12 gives us the factor 20.0 .

     For a layer height of 0.4, this would be 2.41/0.16 = 15.0 .

     The lower the factor, the more material is extruded.

     I am assuming that a printer with a 0.4mm nozzle can not generate
     extrusions that are less wide or wider. Height however varies with
     layer height. This may actually not be true because hot filament
     may stretch or squash.
     */

    double pEFactor = ((1.75/2)*(1.75/2)*M_PI) / (0.4*0.3); // ~20.0

    double pLayerStartTime = 0.0;
    double pTotalTime = 0.0;
};


#endif /* IA_GCODE_WRITER_H */


