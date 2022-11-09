/*********************************************************************
	Rhapsody	: 8.2 
	Login		: vinam
	Component	: DefaultComponent 
	Configuration 	: DefaultConfig
	Model Element	: VehicleController_Driver
//!	Generated Date	: Wed, 24, May 2017  
	File Path	: DefaultComponent\DefaultConfig\VehicleController_Driver.h
*********************************************************************/

#ifndef VehicleController_Driver_H
#define VehicleController_Driver_H

//## auto_generated
#include <oxf\oxf.h>
//## attribute BrakingController
#include "BrakingController.h"
//## attribute LevitationController
#include "LevitationController.h"
//## attribute BrakingSensorBL
#include "Sensor.h"
//## package VehicleController

//## class VehicleController_Driver
class VehicleController_Driver {
    ////    Constructors and destructors    ////
    
public :

    //## auto_generated
    VehicleController_Driver();
    
    //## auto_generated
    ~VehicleController_Driver();
    
    ////    Additional operations    ////

private :

    //## auto_generated
    BrakingController* getBrakingController () const;
    
    //## auto_generated
    Sensor* getBrakingSensorBL() const;
    
    //## auto_generated
    Sensor* getBrakingSensorBR() const;
    
    //## auto_generated
    Sensor* getBrakingSensorFL() const;
    
    //## auto_generated
    Sensor* getBrakingSensorFR() const;
    
    //## auto_generated
    LevitationController* getLevitationController () const;
    
    //## auto_generated
    Sensor* getLevitationSensorBL() const;
    
    //## auto_generated
    Sensor* getLevitationSensorBR() const;
    
    //## auto_generated
    Sensor* getLevitationSensorFL() const;
    
    //## auto_generated
    Sensor* getLevitationSensorFR() const;
    
    ////    Attributes    ////

protected :

    BrakingController BrakingController;		//## attribute BrakingController
    
    Sensor BrakingSensorBL;		//## attribute BrakingSensorBL
    
    Sensor BrakingSensorBR;		//## attribute BrakingSensorBR
    
    Sensor BrakingSensorFL;		//## attribute BrakingSensorFL
    
    Sensor BrakingSensorFR;		//## attribute BrakingSensorFR
    
    LevitationController LevitationController;		//## attribute LevitationController
    
    Sensor LevitationSensorBL;		//## attribute LevitationSensorBL
    
    Sensor LevitationSensorBR;		//## attribute LevitationSensorBR
    
    Sensor LevitationSensorFL;		//## attribute LevitationSensorFL
    
    Sensor LevitationSensorFR;		//## attribute LevitationSensorFR
};

#endif
/*********************************************************************
	File Path	: DefaultComponent\DefaultConfig\VehicleController_Driver.h
*********************************************************************/
