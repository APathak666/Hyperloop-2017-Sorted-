
############# Target type (Debug/Release) ##################
############################################################
CPPCompileDebug= /Zi /Od /D "_DEBUG" $(LIBCRT_FLAG)d  /Fd"$(TARGET_NAME)" 
CPPCompileRelease= /Ox /D"NDEBUG" $(LIBCRT_FLAG) /Fd"$(TARGET_NAME)" 
LinkDebug=
LinkRelease=
BuildSet=Debug
SUBSYSTEM=/SUBSYSTEM:console
RPFrameWorkDll=False
SimulinkLibName=

ConfigurationCPPCompileSwitches=   /I . /I . /I $(OMROOT)\LangCpp /I $(OMROOT)\LangCpp\oxf /nologo /W3 $(ENABLE_EH) $(CRT_FLAGS) $(CPPCompileDebug) /D "_AFXDLL" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "_WINDOWS" $(INST_FLAGS) $(INCLUDE_PATH) $(INST_INCLUDES) /c   

SIMULINK_CONFIG=False
!IF "$(SIMULINK_CONFIG)" == "True"
ConfigurationCPPCompileSwitches=$(ConfigurationCPPCompileSwitches) /D "OM_WITH_CLEANUP"
!ENDIF


!IF "$(RPFrameWorkDll)" == "True"
ConfigurationCPPCompileSwitches=$(ConfigurationCPPCompileSwitches) /D "FRAMEWORK_DLL"
!ENDIF

################### Compilation flags ######################
############################################################
INCLUDE_QUALIFIER=/I
DEFINE_QUALIFIER=/D

!IF "False" == "True"
MT_PREFIX=MT
LIBCRT_FLAG=/MT
!ELSE
MT_PREFIX=
LIBCRT_FLAG=/MD
!ENDIF

LIB_PREFIX=MSVC10x86$(MT_PREFIX)

CRT_FLAGS=$(DEFINE_QUALIFIER)_CRT_SECURE_NO_DEPRECATE $(DEFINE_QUALIFIER)_CRT_SECURE_NO_WARNINGS
ENABLE_EH=/EHa

WINMM_LIB=winmm.lib

################### Commands definition #########################
#################################################################
RMDIR = rmdir
LIB_CMD=link.exe -lib
LINK_CMD=link.exe
LIB_FLAGS=$(LinkDebug)  /NOLOGO   
LINK_FLAGS=$(LinkDebug)  /NOLOGO    $(SUBSYSTEM) /MACHINE:x86 

############### Generated macros #################
##################################################

FLAGSFILE=
RULESFILE=
OMROOT="C:\ProgramData\IBM\Rational\Rhapsody\8.2\Share"
RHPROOT="C:\Program Files (x86)\IBM\Rational\Rhapsody\8.2"

CPP_EXT=.cpp
H_EXT=.h
OBJ_EXT=.obj
EXE_EXT=.exe
LIB_EXT=.lib

INSTRUMENTATION=None

TIME_MODEL=RealTime

TARGET_TYPE=Executable

TARGET_NAME=DefaultComponent

all : $(TARGET_NAME)$(EXE_EXT) DefaultComponent.mak

TARGET_MAIN=MainDefaultComponent

LIBS=

INCLUDE_PATH= \
  $(INCLUDE_QUALIFIER)$(OMROOT)/LangCpp/osconfig/WIN32

ADDITIONAL_OBJS=

OBJS= \
  VehicleController_Driver.obj \
  Sensor.obj \
  BrakingController.obj \
  UnitTesting.obj \
  LevitationController.obj \
  IntegrationTesting.obj \
  VehicleController_CanReceive.obj \
  VehicleController_CANSend.obj \
  VehicleController_UnitTesting.obj \
  VehicleController_IntegrationTesting.obj \
  IMUSensor.obj \
  RetroreflectiveSensor.obj \
  TouchSensor.obj \
  TemperatureSensor.obj \
  PressureSensor.obj \
  Navigation_Driver.obj \
  Navigation_CANReceive.obj \
  Navigation_CANSend.obj \
  Navigation_UnitTesting.obj \
  Navigation_IntegrationTesting.obj \
  Communication_CANSend.obj \
  Communication_CANReceive.obj \
  Channel.obj \
  Telemtry.obj \
  Telecommand.obj \
  Communication_Driver.obj \
  Communciation.obj




OBJ_DIR=

!IF "$(OBJ_DIR)"!=""
CREATE_OBJ_DIR=if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
CLEAN_OBJ_DIR= if exist $(OBJ_DIR) $(RMDIR) $(OBJ_DIR)
!ELSE
CREATE_OBJ_DIR=
CLEAN_OBJ_DIR=
!ENDIF

######################## Predefined macros ############################
#######################################################################
!IF "$(OBJS)" != ""
$(OBJS) : $(INST_LIBS) $(OXF_LIBS)
!ENDIF

LIB_POSTFIX=
!IF "$(BuildSet)"=="Release"
LIB_POSTFIX=R
!ENDIF

!IF "$(TARGET_TYPE)" == "Executable"
LinkDebug=$(LinkDebug) /DEBUG
LinkRelease=$(LinkRelease) /OPT:NOREF
!ELSEIF "$(TARGET_TYPE)" == "Library"
LinkDebug=$(LinkDebug)
!ENDIF


!IF "$(INSTRUMENTATION)" == "Animation"
INST_FLAGS=/D "OMANIMATOR"
INST_INCLUDES=/I $(OMROOT)\LangCpp\aom /I $(OMROOT)\LangCpp\tom
!IF "$(RPFrameWorkDll)" == "True"
INST_LIBS= 
OXF_LIBS=$(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxfanimdll$(LIB_POSTFIX)$(LIB_EXT) 
!ELSE
INST_LIBS= $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)aomanim$(LIB_POSTFIX)$(LIB_EXT) $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxsiminst$(LIB_POSTFIX)$(LIB_EXT)
OXF_LIBS=$(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxfinst$(LIB_POSTFIX)$(LIB_EXT) $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)omComAppl$(LIB_POSTFIX)$(LIB_EXT) $(SimulinkLibName)
!ENDIF
SOCK_LIB=wsock32.lib

!ELSEIF "$(INSTRUMENTATION)" == "Tracing"
INST_FLAGS=/D "OMTRACER"
INST_INCLUDES=/I $(OMROOT)\LangCpp\aom /I $(OMROOT)\LangCpp\tom
!IF "$(RPFrameWorkDll)" == "True"
INST_LIBS=
OXF_LIBS= $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxftracedll$(LIB_POSTFIX)$(LIB_EXT) 
!ELSE
INST_LIBS=$(OMROOT)\LangCpp\lib\$(LIB_PREFIX)tomtrace$(LIB_POSTFIX)$(LIB_EXT) $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)aomtrace$(LIB_POSTFIX)$(LIB_EXT) $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxsiminst$(LIB_POSTFIX)$(LIB_EXT)
OXF_LIBS= $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxfinst$(LIB_POSTFIX)$(LIB_EXT) $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)omComAppl$(LIB_POSTFIX)$(LIB_EXT) $(SimulinkLibName)
!ENDIF
SOCK_LIB=wsock32.lib

!ELSEIF "$(INSTRUMENTATION)" == "None" 
INST_FLAGS=
INST_INCLUDES=
INST_LIBS=
!IF "$(RPFrameWorkDll)" == "True"
OXF_LIBS=$(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxfdll$(LIB_POSTFIX)$(LIB_EXT) $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxsim$(LIB_POSTFIX)$(LIB_EXT)
!ELSE
OXF_LIBS=$(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxf$(LIB_POSTFIX)$(LIB_EXT) $(SimulinkLibName) $(OMROOT)\LangCpp\lib\$(LIB_PREFIX)oxsim$(LIB_POSTFIX)$(LIB_EXT)
!ENDIF
SOCK_LIB=

!ELSE
!ERROR An invalid Instrumentation $(INSTRUMENTATION) is specified.
!ENDIF



################## Generated dependencies ########################
##################################################################






VehicleController_Driver.obj : VehicleController_Driver.cpp VehicleController_Driver.h    Sensor.h BrakingController.h LevitationController.h 
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"VehicleController_Driver.obj" "VehicleController_Driver.cpp" 



Sensor.obj : Sensor.cpp Sensor.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Sensor.obj" "Sensor.cpp" 



BrakingController.obj : BrakingController.cpp BrakingController.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"BrakingController.obj" "BrakingController.cpp" 



UnitTesting.obj : UnitTesting.cpp UnitTesting.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"UnitTesting.obj" "UnitTesting.cpp" 



LevitationController.obj : LevitationController.cpp LevitationController.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"LevitationController.obj" "LevitationController.cpp" 



IntegrationTesting.obj : IntegrationTesting.cpp IntegrationTesting.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"IntegrationTesting.obj" "IntegrationTesting.cpp" 



VehicleController_CanReceive.obj : VehicleController_CanReceive.cpp VehicleController_CanReceive.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"VehicleController_CanReceive.obj" "VehicleController_CanReceive.cpp" 



VehicleController_CANSend.obj : VehicleController_CANSend.cpp VehicleController_CANSend.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"VehicleController_CANSend.obj" "VehicleController_CANSend.cpp" 



VehicleController_UnitTesting.obj : VehicleController_UnitTesting.cpp VehicleController_UnitTesting.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"VehicleController_UnitTesting.obj" "VehicleController_UnitTesting.cpp" 



VehicleController_IntegrationTesting.obj : VehicleController_IntegrationTesting.cpp VehicleController_IntegrationTesting.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"VehicleController_IntegrationTesting.obj" "VehicleController_IntegrationTesting.cpp" 



IMUSensor.obj : IMUSensor.cpp IMUSensor.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"IMUSensor.obj" "IMUSensor.cpp" 



RetroreflectiveSensor.obj : RetroreflectiveSensor.cpp RetroreflectiveSensor.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"RetroreflectiveSensor.obj" "RetroreflectiveSensor.cpp" 



TouchSensor.obj : TouchSensor.cpp TouchSensor.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"TouchSensor.obj" "TouchSensor.cpp" 



TemperatureSensor.obj : TemperatureSensor.cpp TemperatureSensor.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"TemperatureSensor.obj" "TemperatureSensor.cpp" 



PressureSensor.obj : PressureSensor.cpp PressureSensor.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"PressureSensor.obj" "PressureSensor.cpp" 



Navigation_Driver.obj : Navigation_Driver.cpp Navigation_Driver.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Navigation_Driver.obj" "Navigation_Driver.cpp" 



Navigation_CANReceive.obj : Navigation_CANReceive.cpp Navigation_CANReceive.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Navigation_CANReceive.obj" "Navigation_CANReceive.cpp" 



Navigation_CANSend.obj : Navigation_CANSend.cpp Navigation_CANSend.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Navigation_CANSend.obj" "Navigation_CANSend.cpp" 



Navigation_UnitTesting.obj : Navigation_UnitTesting.cpp Navigation_UnitTesting.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Navigation_UnitTesting.obj" "Navigation_UnitTesting.cpp" 



Navigation_IntegrationTesting.obj : Navigation_IntegrationTesting.cpp Navigation_IntegrationTesting.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Navigation_IntegrationTesting.obj" "Navigation_IntegrationTesting.cpp" 



Communication_CANSend.obj : Communication_CANSend.cpp Communication_CANSend.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Communication_CANSend.obj" "Communication_CANSend.cpp" 



Communication_CANReceive.obj : Communication_CANReceive.cpp Communication_CANReceive.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Communication_CANReceive.obj" "Communication_CANReceive.cpp" 



Channel.obj : Channel.cpp Channel.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Channel.obj" "Channel.cpp" 



Telemtry.obj : Telemtry.cpp Telemtry.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Telemtry.obj" "Telemtry.cpp" 



Telecommand.obj : Telecommand.cpp Telecommand.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Telecommand.obj" "Telecommand.cpp" 



Communication_Driver.obj : Communication_Driver.cpp Communication_Driver.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Communication_Driver.obj" "Communication_Driver.cpp" 



Communciation.obj : Communciation.cpp Communciation.h    
	$(CREATE_OBJ_DIR)
	$(CPP) $(ConfigurationCPPCompileSwitches)  /Fo"Communciation.obj" "Communciation.cpp" 






$(TARGET_MAIN)$(OBJ_EXT) : $(TARGET_MAIN)$(CPP_EXT) $(OBJS) 
	$(CPP) $(ConfigurationCPPCompileSwitches) /Fo"$(TARGET_MAIN)$(OBJ_EXT)" $(TARGET_MAIN)$(CPP_EXT)

########################## Linking instructions ###############################
###############################################################################
$(TARGET_NAME)$(EXE_EXT): $(OBJS) $(ADDITIONAL_OBJS) $(TARGET_MAIN)$(OBJ_EXT) DefaultComponent.mak 
	@echo Linking $(TARGET_NAME)$(EXE_EXT)
	$(LINK_CMD)  $(TARGET_MAIN)$(OBJ_EXT) $(OBJS) $(ADDITIONAL_OBJS) \
	$(LIBS) \
	$(INST_LIBS) \
	$(OXF_LIBS) \
	$(SOCK_LIB) \
	$(WINMM_LIB) \
	$(LINK_FLAGS) /out:$(TARGET_NAME)$(EXE_EXT)
	if exist $(TARGET_NAME)$(EXE_EXT).manifest mt.exe -manifest $@.manifest -outputresource:$(TARGET_NAME)$(EXE_EXT);1



$(TARGET_NAME)$(LIB_EXT) : $(OBJS) $(ADDITIONAL_OBJS) DefaultComponent.mak
	@echo Building library $@
	$(LIB_CMD) $(LIB_FLAGS) /out:$(TARGET_NAME)$(LIB_EXT) $(OBJS) $(ADDITIONAL_OBJS) $(LIBS)

clean:
	@echo Cleanup
	if exist VehicleController_Driver.obj erase VehicleController_Driver.obj
	if exist Sensor.obj erase Sensor.obj
	if exist BrakingController.obj erase BrakingController.obj
	if exist UnitTesting.obj erase UnitTesting.obj
	if exist LevitationController.obj erase LevitationController.obj
	if exist IntegrationTesting.obj erase IntegrationTesting.obj
	if exist VehicleController_CanReceive.obj erase VehicleController_CanReceive.obj
	if exist VehicleController_CANSend.obj erase VehicleController_CANSend.obj
	if exist VehicleController_UnitTesting.obj erase VehicleController_UnitTesting.obj
	if exist VehicleController_IntegrationTesting.obj erase VehicleController_IntegrationTesting.obj
	if exist IMUSensor.obj erase IMUSensor.obj
	if exist RetroreflectiveSensor.obj erase RetroreflectiveSensor.obj
	if exist TouchSensor.obj erase TouchSensor.obj
	if exist TemperatureSensor.obj erase TemperatureSensor.obj
	if exist PressureSensor.obj erase PressureSensor.obj
	if exist Navigation_Driver.obj erase Navigation_Driver.obj
	if exist Navigation_CANReceive.obj erase Navigation_CANReceive.obj
	if exist Navigation_CANSend.obj erase Navigation_CANSend.obj
	if exist Navigation_UnitTesting.obj erase Navigation_UnitTesting.obj
	if exist Navigation_IntegrationTesting.obj erase Navigation_IntegrationTesting.obj
	if exist Communication_CANSend.obj erase Communication_CANSend.obj
	if exist Communication_CANReceive.obj erase Communication_CANReceive.obj
	if exist Channel.obj erase Channel.obj
	if exist Telemtry.obj erase Telemtry.obj
	if exist Telecommand.obj erase Telecommand.obj
	if exist Communication_Driver.obj erase Communication_Driver.obj
	if exist Communciation.obj erase Communciation.obj
	if exist $(TARGET_MAIN)$(OBJ_EXT) erase $(TARGET_MAIN)$(OBJ_EXT)
	if exist *$(OBJ_EXT) erase *$(OBJ_EXT)
	if exist $(TARGET_NAME).pdb erase $(TARGET_NAME).pdb
	if exist $(TARGET_NAME)$(LIB_EXT) erase $(TARGET_NAME)$(LIB_EXT)
	if exist $(TARGET_NAME).ilk erase $(TARGET_NAME).ilk
	if exist $(TARGET_NAME)$(EXE_EXT) erase $(TARGET_NAME)$(EXE_EXT)
	if exist $(TARGET_NAME)$(EXE_EXT).manifest erase $(TARGET_NAME)$(EXE_EXT).manifest
	$(CLEAN_OBJ_DIR)
	
