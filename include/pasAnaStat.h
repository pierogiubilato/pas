//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Analysis Statistical checks Unit	  --
// (C) Piero Giubilato 2010-2012, Padova University							  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasAnaStat.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.0"
// [Modified by]	"Piero Giubilato"
// [Date]			"30 Jun 2011"
// [Language]		"C++"
//______________________________________________________________________________


// Overloading check
#ifndef pasAnaStat_H
#define pasAnaStat_H

// PAW objects.
#include "pasObjAna.h"

// #############################################################################
namespace pas { namespace ana {

// Unit functions.
void Stat(pas::obj::Ana&);	// The unit tag function

// #############################################################################
}}

// Overloading check.
#endif
