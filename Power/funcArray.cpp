#include <iostream>
using namespace std;

typedef void (*IntFunctionWithOneParameter) ();

void LinActUp(){
	cout<<"Linear Actuator moving up"<<endl;
}	

void LinActDown(){
	cout<<"Linear Actuator moving down"<<endl;
}

void LSD(){
	cout<<"Low Speed drive enabled"<<endl;
}

void Braking(){
	cout<<"Braking Actuation"<<endl;
}

void ClutchEng(){
	cout<<"Clutch Engaged"<<endl;
}

void ClutchDiseng(){
	cout<<"Clutch Disengaged"<<endl;
}

void EmBrake(){
	cout<<"Emergency Braking"<<endl;
}

void GoOnline(){
	cout<<"Going online"<<endl;
}

int main()
{
    IntFunctionWithOneParameter functions[] = 
    {
        LinActUp, 
        LinActDown, 
        LSD,
        Braking,
        ClutchEng,
        ClutchDiseng,
        EmBrake,
        GoOnline
    };

    for(int i = 0; i < 8; ++i)
    {
        functions[i]() ;
    }

    return 0;
}

	