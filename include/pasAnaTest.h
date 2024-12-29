//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Analysis Test Unit					  --
// (C) Piero Giubilato 2010-2012, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pas_ana_Test.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.0"
// [Modified by]	"Piero Giubilato"
// [Date]			"21 Jun 2011"
// [Language]		"C++"
//______________________________________________________________________________


// Overloading check
#ifndef pasAnaTest_H
#define pasAnaTest_H

// PAW objects.
#include "pasObjAna.h"

// #############################################################################
namespace pas { namespace ana {

// Unit functions.
void Test(pas::obj::Ana&);	// The unit tag function

// #############################################################################
}}

// Overloading check.
#endif
