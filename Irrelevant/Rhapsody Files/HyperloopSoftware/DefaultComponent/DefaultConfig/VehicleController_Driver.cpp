/********************************************************************
	Rhapsody	: 8.2 
	Login		: vinam
	Component	: DefaultComponent 
	Configuration 	: DefaultConfig
	Model Element	: VehicleController_Driver
//!	Generated Date	: Wed, 24, May 2017  
	File Path	: DefaultComponent\DefaultConfig\VehicleController_Driver.cpp
*********************************************************************/

//## auto_generated
#include "VehicleController_Driver.h"
//## package VehicleController

//## class VehicleController_Driver
VehicleController_Driver::VehicleController_Driver() {
    {
         %s;
    }
    {
         %s;
    }
}

VehicleController_Driver::~VehicleController_Driver() {
}

BrakingController* VehicleController_Driver::getBrakingController () const {
    return (BrakingController* %s) &BrakingController;
}

Sensor* VehicleController_Driver::getBrakingSensorBL() const {
    return (Sensor*) &BrakingSensorBL;
}

Sensor* VehicleController_Driver::getBrakingSensorBR() const {
    return (Sensor*) &BrakingSensorBR;
}

Sensor* VehicleController_Driver::getBrakingSensorFL() const {
    return (Sensor*) &BrakingSensorFL;
}

Sensor* VehicleController_Driver::getBrakingSensorFR() const {
    return (Sensor*) &BrakingSensorFR;
}

LevitationController* VehicleController_Driver::getLevitationController () const {
    return (LevitationController* %s) &LevitationController;
}

Sensor* VehicleController_Driver::getLevitationSensorBL() const {
    return (Sensor*) &LevitationSensorBL;
}

Sensor* VehicleController_Driver::getLevitationSensorBR() const {
    return (Sensor*) &LevitationSensorBR;
}

Sensor* VehicleController_Driver::getLevitationSensorFL() const {
    return (Sensor*) &LevitationSensorFL;
}

Sensor* VehicleController_Driver::getLevitationSensorFR() const {
    return (Sensor*) &LevitationSensorFR;
}

/*********************************************************************
	File Path	: DefaultComponent\DefaultConfig\VehicleController_Driver.cpp
*********************************************************************/
