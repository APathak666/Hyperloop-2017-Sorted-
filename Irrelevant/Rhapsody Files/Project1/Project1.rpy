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
	- _modifiedTimeWeak = 5.24.2017::9:37:4;
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
		- size = 4;
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
	}
	- Diagrams = { IRPYRawContainer 
		- size = 0;
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

