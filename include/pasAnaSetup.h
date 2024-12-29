//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Analysis Setup Unit				  --
// (C) Piero Giubilato 2010-2012, Padova University							  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasAnaSetup.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.0"
// [Modified by]	"Piero Giubilato"
// [Date]			"21 Jun 2011"
// [Language]		"C++"
//______________________________________________________________________________


// Overloading check
#ifndef pasAnaSetup_H
#define pasAnaSetup_H

// PAW objects.
#include "pasObjAna.h"

// #############################################################################
namespace pas { namespace ana {

// Unit functions.
void Setup(pas::obj::Ana&);	// The unit tag function

// #############################################################################
}}

// Overloading check.
#endif
