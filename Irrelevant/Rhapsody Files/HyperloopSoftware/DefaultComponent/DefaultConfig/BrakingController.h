/*********************************************************************
	Rhapsody	: 8.2 
	Login		: vinam
	Component	: DefaultComponent 
	Configuration 	: DefaultConfig
	Model Element	: BrakingController
//!	Generated Date	: Thu, 25, May 2017  
	File Path	: DefaultComponent\DefaultConfig\BrakingController.h
*********************************************************************/

#ifndef BrakingController_H
#define BrakingController_H

//## auto_generated
#include <oxf\oxf.h>
//## package VehicleController

//## class BrakingController
class BrakingController {
    ////    Constructors and destructors    ////
    
public :

    //## auto_generated
    BrakingController();
    
    //## auto_generated
    ~BrakingController();
    
    ////    Operations    ////
    
    //## operation controller()
    float controller();
    
    //## operation retriveCurrentTrajectory()
    void retriveCurrentTrajectory();
    
    //## operation retriveExpectedTrajectory()
    void retriveExpectedTrajectory();
    
    //## operation validation()
    bool validation();
    
    ////    Additional operations    ////
    
    //## auto_generated
    int getAttribute_10() const;
    
    //## auto_generated
    void setAttribute_10(int p_attribute_10);
    
    ////    Attributes    ////

protected :

    int attribute_10;		//## attribute attribute_10
};

#endif
/*********************************************************************
	File Path	: DefaultComponent\DefaultConfig\BrakingController.h
*********************************************************************/
