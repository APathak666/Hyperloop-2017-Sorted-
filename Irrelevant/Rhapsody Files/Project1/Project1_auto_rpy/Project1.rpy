I-Logix-RPY-Archive version 8.13.0 C++ 9794446
{ IProject 
	- _id = GUID 21c31177-0c91-45fd-a60b-825f8323b0aa;
	- _myState = 8192;
	- _name = "Project1";
	- codeUpdateCGTime = 5.24.2017::9:37:4;
	- Dependencies = { IRPYRawContainer 
		- size = 1;
		- value = 
		{ IDependency 
			- _id = GUID 99eb2baf-2d8e-4ec5-aa81-edc6f9270461;
			- _myState = 2048;
			- _name = "CodeCentricCpp";
			- codeUpdateCGTime = 5.24.2017::9:37:4;
			- Stereotypes = { IRPYRawContainer 
				- size = 1;
				- value = 
				{ IHandle 
					- _m2Class = "IStereotype";
					- _filename = "PredefinedTypes.sbs";
					- _subsystem = "PredefinedTypes";
					- _class = "";
					- _name = "AppliedProfile";
					- _id = GUID d2eca2c1-e5a5-4296-9453-29283c4ed8bc;
				}
			}
			- _modifiedTimeWeak = 5.24.2017::9:37:4;
			- _dependsOn = { INObjectHandle 
				- _m2Class = "IProfile";
				- _filename = "$OMROOT\\Settings\\CodeCentric\\\\CodeCentric752Cpp.sbs";
				- _subsystem = "";
				- _class = "";
				- _name = "CodeCentricCpp";
				- _id = GUID 5bc6cd73-d83c-4d02-b530-2cc953376a5f;
			}
		}
	}
	- _modifiedTimeWeak = 5.24.2017::9:37:13;
	- _lastID = 1;
	- _UserColors = { IRPYRawContainer 
		- size = 16;
		- value = 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 16777215; 
	}
	- _defaultSubsystem = { ISubsystemHandle 
		- _m2Class = "ISubsystem";
		- _filename = "Default.sbs";
		- _subsystem = "";
		- _class = "";
		- _name = "Default";
		- _id = GUID 2670be32-609a-4e80-9b93-ad7f5dd5a822;
	}
	- _component = { IHandle 
		- _m2Class = "IComponent";
		- _filename = "DefaultComponent.cmp";
		- _subsystem = "";
		- _class = "";
		- _name = "DefaultComponent";
		- _id = GUID 0dbbce9b-7112-4df4-b6ef-a432c0bb78e2;
	}
	- Multiplicities = { IRPYRawContainer 
		- size = 4;
		- value = 
		{ IMultiplicityItem 
			- _name = "1";
			- _count = -1;
		}
		{ IMultiplicityItem 
			- _name = "*";
			- _count = -1;
		}
		{ IMultiplicityItem 
			- _name = "0,1";
			- _count = -1;
		}
		{ IMultiplicityItem 
			- _name = "1..*";
			- _count = -1;
		}
	}
	- Subsystems = { IRPYRawContainer 
		- size = 5;
		- value = 
		{ ISubsystem 
			- fileName = "Default";
			- _id = GUID 2670be32-609a-4e80-9b93-ad7f5dd5a822;
		}
		{ IProfile 
			- fileName = "UMLPerspectives";
			- _persistAs = "$OMROOT\\Settings\\UMLPerspectives";
			- _id = GUID d24d9192-bc4f-48a2-a4e1-fdaebbdfa6b6;
			- _partOfTheModelKind = referenceunit;
		}
		{ IProfile 
			- fileName = "SoftwareArchitect752Cpp";
			- _persistAs = "$OMROOT\\Settings\\SoftwareArchitect";
			- _id = GUID c4330c76-1cd6-4dbd-bdff-600355b9f34d;
			- _name = "SoftwareArchitectCpp";
			- _partOfTheModelKind = referenceunit;
		}
		{ IProfile 
			- fileName = "CodeCentric752Cpp";
			- _persistAs = "$OMROOT\\Settings\\CodeCentric\\";
			- _id = GUID 5bc6cd73-d83c-4d02-b530-2cc953376a5f;
			- _name = "CodeCentricCpp";
			- _partOfTheModelKind = referenceunit;
		}
		{ ISubsystem 
			- fileName = "VehicleController";
			- _id = GUID fc495911-946d-4c91-a401-a0fa0bc75cf8;
		}
	}
	- Diagrams = { IRPYRawContainer 
		- size = 1;
		- value = 
		{ IDiagram 
			- _id = GUID 74633357-55d5-49c3-9b0a-cb27de231e8e;
			- _myState = 8192;
			- _name = "Model1";
			- _modifiedTimeWeak = 1.2.1990::0:0:0;
			- _lastModifiedTime = "5.24.2017::9:37:5";
			- _graphicChart = { CGIClassChart 
				- _id = GUID c5f63b55-45df-421c-b58c-ffa9a14ac442;
				- m_type = 0;
				- m_pModelObject = { IHandle 
					- _m2Class = "IDiagram";
					- _id = GUID 74633357-55d5-49c3-9b0a-cb27de231e8e;
				}
				- m_pParent = ;
				- m_name = { CGIText 
					- m_str = "";
					- m_style = "Arial" 10 0 0 0 1 ;
					- m_color = { IColor 
						- m_fgColor = 0;
						- m_bgColor = 0;
						- m_bgFlag = 0;
					}
					- m_position = 1 0 0  ;
					- m_nIdent = 0;
					- m_bImplicitSetRectPoints = 0;
					- m_nOrientationCtrlPt = 8;
				}
				- m_drawBehavior = 0;
				- m_bIsPreferencesInitialized = 0;
				- elementList = 1;
				{ CGIClass 
					- _id = GUID 1cd02c84-9a71-43aa-a78e-37875649fbda;
					- m_type = 78;
					- m_pModelObject = { IHandle 
						- _m2Class = "IClass";
						- _filename = "Default.sbs";
						- _subsystem = "Default";
						- _class = "";
						- _name = "TopLevel";
						- _id = GUID 0624b33a-560e-46c5-93b1-5df10ce61f68;
					}
					- m_pParent = ;
					- m_name = { CGIText 
						- m_str = "TopLevel";
						- m_style = "Arial" 10 0 0 0 1 ;
						- m_color = { IColor 
							- m_fgColor = 0;
							- m_bgColor = 0;
							- m_bgFlag = 0;
						}
						- m_position = 1 0 0  ;
						- m_nIdent = 0;
						- m_bImplicitSetRectPoints = 0;
						- m_nOrientationCtrlPt = 5;
					}
					- m_drawBehavior = 0;
					- m_bIsPreferencesInitialized = 0;
					- m_AdditionalLabel = { CGIText 
						- m_str = "";
						- m_style = "Arial" 10 0 0 0 1 ;
						- m_color = { IColor 
							- m_fgColor = 0;
							- m_bgColor = 0;
							- m_bgFlag = 0;
						}
						- m_position = 1 0 0  ;
						- m_nIdent = 0;
						- m_bImplicitSetRectPoints = 0;
						- m_nOrientationCtrlPt = 1;
					}
					- m_polygon = 0 ;
					- m_nNameFormat = 0;
					- m_nIsNameFormat = 0;
					- Compartments = { IRPYRawContainer 
						- size = 0;
					}
					- Attrs = { IRPYRawContainer 
						- size = 0;
					}
					- Operations = { IRPYRawContainer 
						- size = 0;
					}
				}
				
				- m_access = 'Z';
				- m_modified = 'N';
				- m_fileVersion = "";
				- m_nModifyDate = 0;
				- m_nCreateDate = 0;
				- m_creator = "";
				- m_bScaleWithZoom = 1;
				- m_arrowStyle = 'S';
				- m_pRoot = GUID 1cd02c84-9a71-43aa-a78e-37875649fbda;
				- m_currentLeftTop = 0 0 ;
				- m_currentRightBottom = 0 0 ;
				- m_bFreezeCompartmentContent = 0;
			}
			- _defaultSubsystem = { IHandle 
				- _m2Class = "ISubsystem";
				- _filename = "Default.sbs";
				- _subsystem = "";
				- _class = "";
				- _name = "Default";
				- _id = GUID 2670be32-609a-4e80-9b93-ad7f5dd5a822;
			}
		}
	}
	- Components = { IRPYRawContainer 
		- size = 1;
		- value = 
		{ IComponent 
			- fileName = "DefaultComponent";
			- _id = GUID 0dbbce9b-7112-4df4-b6ef-a432c0bb78e2;
		}
	}
}

