/*
 *  I claim no ownership of the following code. It is used in this project for the sole purpose of
 *  parsing a gcode command for text. The following code is a HEAVILY trimmed down version of
 *  the gcode parsing and transfer protocol for the Repetier 3D Printer Firmware. The original code
 *  in its entirety can be found at the web page listed below and it licensed under
 *  GNU General Public License version 3 or later.
 *
 *  https://github.com/repetier/Repetier-Firmware/blob/master/src/ArduinoAVR/Repetier/gcode.h
 */

#ifndef _GCODE_H
#define _GCODE_H

#include "defs.h" /// ADDED

class GCode   // 52 uint8_ts per command needed
{
    uint16_t params;
    uint16_t params2;
public:
    uint16_t N; ///< Line number reduced to 16 bit
    uint16_t M; ///< G-code M value if set
    uint16_t G; ///< G-code G value if set
    float X; ///< G-code X value if set
    float Y; ///< G-code Y value if set
    float Z; ///< G-code Z value if set
    float E; ///< G-code E value if set
    float F; ///< G-code F value if set
    int32_t S; ///< G-code S value if set
    int32_t P; ///< G-code P value if set
    float I; ///< G-code I value if set
    float J; ///< G-code J value if set
    float R; ///< G-code R value if set
    float D; ///< G-code D value if set
    float C; ///< G-code C value if set
    float H; ///< G-code H value if set
    float A; ///< G-code A value if set
    float B; ///< G-code B value if set
    float K; ///< G-code K value if set
    float L; ///< G-code L value if set
    float O; ///< G-code O value if set

    char *text; ///< Text message of g-code if present.
    //moved the byte to the end and aligned ints on short boundary
    // Old habit from PC, which require alignments for data types such as int and long to be on 2 or 4 byte boundary
    // Otherwise, the compiler adds padding, wasted space.
    uint8_t T; // This may not matter on any of these controllers, but it can't hurt
    // True if origin did not come from serial console. That way we can send status messages to
    // a host only if he would normally not know about the mode switch.
    bool internalCommand;
    inline bool hasM()
    {
        return ((params & 2)!=0);
    }
    inline bool hasN()
    {
        return ((params & 1)!=0);
    }
    inline bool hasG()
    {
        return ((params & 4)!=0);
    }
    inline bool hasX()
    {
        return ((params & 8)!=0);
    }
    inline void unsetX() {
        params &= ~8;
    }
    inline bool hasY()
    {
        return ((params & 16)!=0);
    }
    inline void unsetY() {
        params &= ~16;
    }
    inline bool hasZ()
    {
        return ((params & 32)!=0);
    }
    inline void unsetZ() {
        params &= ~32;
    }
    inline bool hasNoXYZ()
    {
        return ((params & 56)==0);
    }
    inline bool hasE()
    {
        return ((params & 64)!=0);
    }
    inline bool hasF()
    {
        return ((params & 256)!=0);
    }
    inline bool hasT()
    {
        return ((params & 512)!=0);
    }
    inline bool hasS()
    {
        return ((params & 1024)!=0);
    }
    inline bool hasP()
    {
        return ((params & 2048)!=0);
    }
    inline bool isV2()
    {
        return ((params & 4096)!=0);
    }
    inline bool hasString()
    {
        return ((params & 32768)!=0);
    }
    inline bool hasI()
    {
        return ((params2 & 1)!=0);
    }
    inline bool hasJ()
    {
        return ((params2 & 2)!=0);
    }
    inline bool hasR()
    {
        return ((params2 & 4)!=0);
    }
    inline bool hasD()
    {
        return ((params2 & 8)!=0);
    }
    inline bool hasC()
    {
        return ((params2 & 16)!=0);
    }
    inline bool hasH()
    {
        return ((params2 & 32)!=0);
    }
    inline bool hasA()
    {
        return ((params2 & 64)!=0);
    }
    inline bool hasB()
    {
        return ((params2 & 128)!=0);
    }
    inline bool hasK()
    {
        return ((params2 & 256)!=0);
    }
    inline bool hasL()
    {
        return ((params2 & 512)!=0);
    }
    inline bool hasO()
    {
        return ((params2 & 1024)!=0);
    }
    inline long getS(long def)
    {
        return (hasS() ? S : def);
    }
    inline long getP(long def)
    {
        return (hasP() ? P : def);
    }
    inline void setFormatError() {
        params2 |= 32768;
    }
    inline bool hasFormatError() {
        return ((params2 & 32768)!=0);
    }
    static void executeFString(char *cmds);     // modified to take char pointer
    bool parseAscii(char *line,bool fromSerial);
protected:
    inline float parseFloatValue(char *s)
    {
        char *endPtr;
        while(*s == 32) s++; // skip spaces
        float f = (strtod(s, &endPtr));
        if(s == endPtr) f=0.0; // treat empty string "x " as "x0"
        return f;
    }
    inline long parseLongValue(char *s)
    {
        char *endPtr;
        while(*s == 32) s++; // skip spaces
        long l = (strtol(s, &endPtr, 10));
        if(s == endPtr) l=0; // treat empty string argument "p " as "p0"
        return l;
    }

    static uint32_t actLineNumber; ///< Line number of current command.
    static uint8_t formatErrors; ///< Number of sequential format errors
};

#endif
