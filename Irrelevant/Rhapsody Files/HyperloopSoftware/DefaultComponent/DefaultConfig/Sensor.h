/*********************************************************************
	Rhapsody	: 8.2 
	Login		: vinam
	Component	: DefaultComponent 
	Configuration 	: DefaultConfig
	Model Element	: Sensor
//!	Generated Date	: Thu, 25, May 2017  
	File Path	: DefaultComponent\DefaultConfig\Sensor.h
*********************************************************************/

#ifndef Sensor_H
#define Sensor_H

//## auto_generated
#include <oxf\oxf.h>
//## package VehicleController

//## class Sensor
class Sensor {
    ////    Constructors and destructors    ////
    
public :

    //## auto_generated
    Sensor();
    
    //## auto_generated
    ~Sensor();
    
    ////    Operations    ////
    
    //## operation dataAcquisition()
    void dataAcquisition();
    
    //## operation dataLogging()
    void dataLogging();
    
    //## operation dataValidation()
    void dataValidation();
    
    //## operation getData()
    void getData();
    
    ////    Additional operations    ////
    
    //## auto_generated
    int getSensorData() const;
    
    //## auto_generated
    void setSensorData(int p_sensorData);
    
    ////    Attributes    ////

protected :

    int sensorData;		//## attribute sensorData
};

#endif
/*********************************************************************
	File Path	: DefaultComponent\DefaultConfig\Sensor.h
*********************************************************************/
