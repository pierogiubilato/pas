//------------------------------------------------------------------------------
// PAS - Analysis object														  --
// (C) Piero Giubilato 2011-2013, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasObjAna.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"25 Sep 2012"
// [Language]		"C++"
//______________________________________________________________________________


// Overloading check
#ifndef pasObjAna_H
#define pasObjAna_H

// Root.
#include "TCanvas.h"

// Application default.
//#include "pasMain.h"

// #############################################################################
namespace pas {	namespace obj {

//! Basic analysis class.
/*! pas::obj::Ana is the object where all the common data, settings, properties,
 *	of the current analysis have to be stored. This object should be passed to 
 *	every job of the analysis as unique way to get/set all the necessary data
 *	and parameters.
 *	
 *	\author Piero Giubilato
 *	\version 1.2
 *	\date 24 Oct 2012
 */
//______________________________________________________________________________
class Ana
{
	private:   

		// General parameters.
		struct kAna {
					bool flagTest;
					bool flagStat;
					bool flagW; 
					bool flagUV; 
					bool flagReco;
					bool flagDerive;
					unsigned int tetraCount;
					int tetraParam[32];
					};

		// Simulation parameters.
		struct kSim {
					unsigned int MCRuns;
					bool flagPoisson;
					bool flagDebug;
					bool flagDebugMaps;
					bool flagDebugLayer;	
					bool flagHitsAbsolute;
					bool flagCluster;
					bool flagNoise;
					unsigned int sizeCount;
					unsigned int sizeList[32];
					unsigned int hitsCount;
					double       hitsList[32];
					unsigned int multCount;
					unsigned int multList[32];
					unsigned int pxszCount;
					double       pxszList[32];
					unsigned int clzsCount;
					double       clzsParam[32];
					unsigned int clzsRootsCount;
					double       clzsRootsParam[32];
					unsigned int noiseCount;
					double       noiseParam[32];
					};

		// Simulation parameters.
		struct kMisc {
					bool flagDataPlot;
					};
	protected:

		// No protected at the moment

	public:

		// Analysis properties.
		kAna _Ana;	
	
		// Simulation properties.
		kSim _Sim;
 
		// Miscellaneous properties.
		kMisc _Misc;
 
		// Forms vector (for displaying).
		std::vector<TCanvas*> _Canvas;

		// Special members.
		Ana();			//!< Default ctor.
		~Ana();		//!< Dtor.
};

// #############################################################################
}} // Close Namespaces.

// Overloading check.
#endif
