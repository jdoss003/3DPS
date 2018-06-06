/*
 *  I claim no ownership of the following code. It is used in this project for the sole purpose of
 *  parsing a gcode command for text. The following code is a HEAVILY trimmed down version of
 *  the gcode parsing and transfer protocol for the Repetier 3D Printer Firmware. The original code
 *  in its entirity can be found at the web page listed below and it licensed under
 *  GNU General Public License version 3 or later.
 *
 *  https://github.com/repetier/Repetier-Firmware/blob/master/src/ArduinoAVR/Repetier/gcode.cpp
 */
#include "defs.h" // ADDED

uint32_t GCode::actLineNumber; ///< Line number of current command.
uint8_t  GCode::formatErrors = 0;
/** \brief Computes size of binary data from bitfield.

In the repetier-protocol in binary mode, the first 2 uint8_ts define the
data. From this bitfield, this function computes the size of the command
including the 2 uint8_ts of the bitfield and the 2 uint8_ts for the checksum.

Gcode Letter to Bit and Datatype:

- N : Bit 0 : 16-Bit Integer
- M : Bit 1 :  8-Bit unsigned uint8_t
- G : Bit 2 :  8-Bit unsigned uint8_t
- X : Bit 3 :  32-Bit Float
- Y : Bit 4 :  32-Bit Float
- Z : Bit 5 :  32-Bit Float
- E : Bit 6 :  32-Bit Float
-  : Bit 7 :  always set to distinguish binary from ASCII line.
- F : Bit 8 :  32-Bit Float
- T : Bit 9 :  8 Bit Integer
- S : Bit 10 : 32 Bit Value
- P : Bit 11 : 32 Bit Integer
- V2 : Bit 12 : Version 2 command for additional commands/sizes
- Ext : Bit 13 : There are 2 more uint8_ts following with Bits, only for future versions
- Int :Bit 14 : Marks it as internal command,
- Text : Bit 15 : 16 Byte ASCII String terminated with 0
Second word if V2:
- I : Bit 0 : 32-Bit float
- J : Bit 1 : 32-Bit float
- R : Bit 2 : 32-Bit float
- D : Bit 3 : 32-Bit float
- C : Bit 4 : 32-Bit float
- H : Bit 5 : 32-Bit float
- A : Bit 6 : 32-Bit float
- B : Bit 7 : 32-Bit float
- K : Bit 8 : 32-Bit float
- L : Bit 9 : 32-Bit float
- O : Bit 0 : 32-Bit float
*/
/** \brief Execute commands in progmem stored string. Multiple commands are separated by \n 
Used to execute memory stored parts called from gcodes. For new commands use the
flash sender instead.
*/
void GCode::executeFString(char *cmds)
{
    char buf[80];
    uint8_t buflen;
    char c = 0;
    do
    {
        GCode code;

        // Wait for a free place in command buffer
        // Scan next command from string
        uint8_t comment = 0;
        buflen = 0;
        do
        {
            c = *cmds;
            ++cmds;
            if(c == 0 || c == '\n') break;
            if(c == ';') comment = 1;
            if(comment) continue;
            buf[buflen++] = c;
        }
        while(buflen < 79);
        if(buflen == 0) {  // empty line ignore
			if(!c) return; // Special case \n0
            continue;
		}
        buf[buflen] = 0;
        // Send command into command buffer
        if(code.parseAscii((char *)buf,false) && (code.params & 518))   // Success
        {
			LCD_DisplayString(17, (const unsigned char*)buf);
            proccess_command(code); // ADDED
        }
    }
    while(c);
}


/**
  Converts a ASCII GCode line into a GCode structure.
*/
bool GCode::parseAscii(char *line,bool fromSerial)
{
    char *pos = line;
    params = 0;
    params2 = 0;
    internalCommand = !fromSerial;
	bool hasChecksum = false;
    char c;
    while ( (c = *(pos++)) )
    {
        if(c == '(' || c == '%') break; // alternative comment or program block
        switch(c)
        {
        case 'N':
        case 'n':
        {
            actLineNumber = parseLongValue(pos);
            params |=1;
            N = actLineNumber;
            break;
        }
        case 'G':
        case 'g':
        {
            G = parseLongValue(pos) & 0xffff;
            params |= 4;
            if(G > 255) params |= 4096;
            break;
        }
        case 'M':
        case 'm':
        {
            M = parseLongValue(pos) & 0xffff;
            params |= 2;
            if(M > 255) params |= 4096;
            // handle non standard text arguments that some M codes have
            if (M == 20 || M == 23 || M == 28 || M == 29 || M == 30 || M == 32 || M == 36 || M == 117 || M == 531)
            {
                // after M command we got a filename or text
                char digit;
                while( (digit = *pos) )
                {
                    if (digit < '0' || digit > '9') break;
                    pos++;
                }
                while( (digit = *pos) )
                {
                    if (digit != ' ') break;
                    pos++;
                    // skip leading white spaces (may be no white space)
                }
                text = pos;
                while (*pos)
                {
                    if((M != 117 && M != 20 && M != 531 && *pos==' ') || *pos=='*') break;
                    pos++; // find a space as file name end
                }
                *pos = 0; // truncate filename by erasing space with null, also skips checksum
                //waitUntilAllCommandsAreParsed = true; // don't risk string be deleted
                params |= 32768;
            }
            break;
        }
        case 'X':
        case 'x':
        {
            X = parseFloatValue(pos);
            params |= 8;
            break;
        }
        case 'Y':
        case 'y':
        {
            Y = parseFloatValue(pos);
            params |= 16;
            break;
        }
        case 'Z':
        case 'z':
        {
            Z = parseFloatValue(pos);
            params |= 32;
            break;
        }
        case 'E':
        case 'e':
        {
            E = parseFloatValue(pos);
            params |= 64;
            break;
        }
        case 'F':
        case 'f':
        {
            F = parseFloatValue(pos);
            params |= 256;
            break;
        }
        case 'T':
        case 't':
        {
            T = parseLongValue(pos) & 0xff;
            params |= 512;
            break;
        }
        case 'S':
        case 's':
        {
            S = parseLongValue(pos);
            params |= 1024;
            break;
        }
        case 'P':
        case 'p':
        {
            P = parseLongValue(pos);
            params |= 2048;
            break;
        }
        case 'I':
        case 'i':
        {
            I = parseFloatValue(pos);
            params2 |= 1;
            params |= 4096; // Needs V2 for saving
            break;
        }
        case 'J':
        case 'j':
        {
            J = parseFloatValue(pos);
            params2 |= 2;
            params |= 4096; // Needs V2 for saving
            break;
        }
        case 'R':
        case 'r':
        {
            R = parseFloatValue(pos);
            params2 |= 4;
            params |= 4096; // Needs V2 for saving
            break;
        }
        case 'D':
        case 'd':
        {
            D = parseFloatValue(pos);
            params2 |= 8;
            params |= 4096; // Needs V2 for saving
            break;
        }
        case 'C':
        case 'c':
        {
	        C = parseFloatValue(pos);
	        params2 |= 16;
	        params |= 4096; // Needs V2 for saving
	        break;
        }
        case 'H':
        case 'h':
        {
	        H = parseFloatValue(pos);
	        params2 |= 32;
	        params |= 4096; // Needs V2 for saving
	        break;
        }
        case 'A':
        case 'a':
        {
	        A = parseFloatValue(pos);
	        params2 |= 64;
	        params |= 4096; // Needs V2 for saving
	        break;
        }
        case 'B':
        case 'b':
        {
	        B = parseFloatValue(pos);
	        params2 |= 128;
	        params |= 4096; // Needs V2 for saving
	        break;
        }
        case 'K':
        case 'k':
        {
	        K = parseFloatValue(pos);
	        params2 |= 256;
	        params |= 4096; // Needs V2 for saving
	        break;
        }
        case 'L':
        case 'l':
        {
	        L = parseFloatValue(pos);
	        params2 |= 512;
	        params |= 4096; // Needs V2 for saving
	        break;
        }
        case 'O':
        case 'o':
        {
	        O = parseFloatValue(pos);
	        params2 |= 1024;
	        params |= 4096; // Needs V2 for saving
	        break;
        }
        case '*' : //checksum
        {
            uint8_t checksum_given = parseLongValue(pos);
            uint8_t checksum = 0;
            while(line != (pos - 1)) checksum ^= *line++;
            break;
        }
        default:
            break;
        }// end switch
    }// end while
    if(hasFormatError() /*|| (params & 518) == 0*/)   // Must contain G, M or T command and parameter need to have variables!
    {
        formatErrors++;
        if(formatErrors < 3) return false;
    }
    else formatErrors = 0;
    return true;
}

