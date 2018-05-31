/*
 * extruder.h
 *
 * Created: 5/27/2018 3:34:01 PM
 *  Author: Justin
 */

#ifndef EXTRUDER_H_
#define EXTRUDER_H_

namespace Extruder
{
    void  init();
    void  setTemp(unsigned char);
    float getTemp();
    void checkTemp();
    void onTickHeater(_task *);
    void onTickSensor(_task *);
}

#endif /* EXTRUDER_H_ */