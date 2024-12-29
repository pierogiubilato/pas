//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Analysis Info Unit					  --
// (C) Piero Giubilato 2010-2012, Padova University							  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasAnaInfo.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"22 Mar 2012"
// [Language]		"C++"
//______________________________________________________________________________


// Overloading check
#ifndef pasAnaInfo_H
#define pasAnaInfo_H

// PAW objects.
#include "pasObjAna.h"

// #############################################################################
namespace pas { namespace ana {

// Unit functions.
void Info(pas::obj::Ana&);	// The unit tag function

// #############################################################################
}}

// Overloading check.
#endif
