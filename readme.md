========================================================================
    CONSOLE APPLICATION : GETComp Project Overview
========================================================================

AppWizard has created this GETComp application for you.

This file contains a summary of what you will find in each of the files that
make up your GETComp application.


GETComp.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

GETComp.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

GETComp.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named GETComp.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

t0:         the algorithm's begining mark
t2 - t1:    populate
t4 - t3:    create columns
t6 - t5:    other constraints
t8 - t7:    solver + localB
t10 - t9:   print solution

Criterias:
> SIW: sum of inversed weights // soma dos arcos invertidos
> DEGREE: node degree [1] // grau
> RDEGREE: relative node degree [2] // grau relativo
> CLOSENESS: node closeness [3] // proximidade
> RCLOSENESS relative node closeness [4] // proximidade relativa
> ECCENTRICITY: eccentricity of a node [5] // excentricidade
> RADIAL: radial [6] // centralidade radial

/////////////////////////////////////////////////////////////////////////////
