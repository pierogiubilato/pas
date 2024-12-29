//------------------------------------------------------------------------------
// PAS - Pixel Analysis Workshop - Miscellaneous tasks Unit					  --
// (C) Piero Giubilato 2010-2012, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasMisc.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.0"
// [Modified by]	"Piero Giubilato"
// [Date]			"24 Oct 2012"
// [Language]		"C++"
//______________________________________________________________________________


// Overloading check
#ifndef pasMisc_H
#define pasMisc_H

// PAW objects.
#include "pasObjAna.h"

// #############################################################################
namespace pas {

//! Misc class.
/*! pas::Misc is a unit containing miscellaneous items (mostly test).
 *	
 *	\author Piero Giubilato
 *	\version 1.0
 *	\date 24 Oct 2012
 */
//______________________________________________________________________________
class Misc
{
	private:

		
	public:

		// Special members.
		Misc();					//!< Default ctor.	
		~Misc();				//!< Default dtor.

		// Analysis workflow.
		void DataPlot(pas::obj::Ana&); // Arbitrary Data Plotting.
};


// #############################################################################
}

// Overloading check.
#endif
