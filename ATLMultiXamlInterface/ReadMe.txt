// December 5, 2016 ////////////////////////////////////////////////////////////////
Upgrading project 'ATLMultiDocEdit'...
	Configuration 'Debug|Win32': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Debug|x64': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Release|Win32': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Release|x64': changing Platform Toolset to 'v140' (was 'v120').
Upgrading project 'ATLXAML'...
	Configuration 'Debug|Win32': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Debug|x64': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Release|Win32': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Release|x64': changing Platform Toolset to 'v140' (was 'v120').
Upgrading project 'MultiRich'...
	Configuration 'Debug|Win32': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Debug|x64': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Release|Win32': changing Platform Toolset to 'v140' (was 'v120').
	Configuration 'Release|x64': changing Platform Toolset to 'v140' (was 'v120').
Retargeting End: 3 completed, 0 failed, 0 skipped
////////////////////////////////////////////////////////////////////////////////////

The ATL provides CWindowImpl<class, CWindow, CWinTraits>. CWinTraits come in
Frame, MDI Child and Control flavours. If using the frame as a MDI frame one cannot
just simply set m_pfnSuperWindowProc with the overide because ::DefFrameProc has an
extra parameter for the superclassed "MDICLIENT". One must overide GetWindowProc which
StartWindowProc calls and does some magic to change an HWND into a pointer to call the
overidden WNDPROC with.

The only example of non MFC MDI was found  at VC2010 Samples\C++\OS\WindowsXP\GetImage.
The MDI documentation is somewhat confusing until figured out.
	Create a frame window which creates a "MDICLIENT" class window, then create MDIChildren
========================================================================
    ACTIVE TEMPLATE LIBRARY : ATLXAML Project Overview
========================================================================

AppWizard has created this ATLXAML project for you to use as the starting point for
writing your Executable (EXE).

This file contains a summary of what you will find in each of the files that
make up your project.

ATLXAML.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

ATLXAML.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

//////Removed
ATLXAML.idl
    This file contains the IDL definitions of the type library, the interfaces
    and co-classes defined in your project.
    This file will be processed by the MIDL compiler to generate:
        C++ interface definitions and GUID declarations (ATLXAML.h)
        GUID definitions                                (ATLXAML_i.c)
        A type library                                  (ATLXAML.tlb)
        Marshaling code                                 (ATLXAML_p.c and dlldata.c)

ATLXAML.h
    This file contains the C++ interface definitions and GUID declarations of the
    items defined in ATLXAML.idl. It will be regenerated by MIDL during compilation.
//////Removed
ATLXAML.cpp
    This file contains the object map and the implementation of WinMain.

ATLXAML.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.


/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named ATLXAML.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file that defines resource IDs.

/////////////////////////////////////////////////////////////////////////////
//////Removed
Proxy/stub DLL project and module definition file:

ATLXAMLps.vcxproj
    This file is the project file for building a proxy/stub DLL if necessary.
	The IDL file in the main project must contain at least one interface and you must
	first compile the IDL file before building the proxy/stub DLL.	This process generates
	dlldata.c, ATLXAML_i.c and ATLXAML_p.c which are required
	to build the proxy/stub DLL.

ATLXAMLps.vcxproj.filters
    This is the filters file for the proxy/stub project. It contains information about the 
    association between the files in your project and the filters. This association is 
    used in the IDE to show grouping of files with similar extensions under a specific
    node (for e.g. ".cpp" files are associated with the "Source Files" filter).

ATLXAMLps.def
    This module definition file provides the linker with information about the exports
    required by the proxy/stub.

//////Removed
/////////////////////////////////////////////////////////////////////////////
