//------------------------------------------------------------------------------
// PAS - Analysis object													  --
// (C) Piero Giubilato 2011-2013, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasObjAna.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"24 Oct 2012"
// [Language]		"C++"
//______________________________________________________________________________


// PAS general.
#include "pasConsole.h"

// Analysis items.
#include "pasObjAna.h"

// #############################################################################
namespace pas {	namespace obj {

//______________________________________________________________________________
Ana::Ana()
{
 	/*! Default ctor. Set default parameter for the analysis environment. */

	// Analysis.
	_Ana.flagTest = false;
	_Ana.flagStat = false;
	_Ana.flagReco = false;
	_Ana.flagW = false;
	_Ana.flagUV = true;
	_Ana.flagDerive = false;

	// Analysis, tetra parameters.
	_Ana.tetraCount = 3;	
	_Ana.tetraParam[0] = 0;			// Main threshold value (thr0).
	_Ana.tetraParam[1] = 400;		// ADC range [arbitrary counts].
	_Ana.tetraParam[2] = 1;			// ADC resolution [bits].
	
	// Simulation.
	_Sim.MCRuns = 1000;				// 1000 runs by feault.
	_Sim.flagPoisson = false;		// Affect all hits params with Poisson fluctuations.
	_Sim.flagHitsAbsolute = false;	// If true use the hits number as per frame, not per mm2.
	_Sim.flagDebug = false;			// Shows/hide debug info.
	_Sim.flagDebugLayer = false;	// Shows/hide layer info.
	_Sim.flagDebugMaps = false;		// Shows/hide maps info.
	_Sim.flagCluster = false;		// Use/do not use clusters.
	_Sim.flagNoise = false;			// Use/do not use noise.
	
	// Misc
	_Misc.flagDataPlot = false;		// Forces arbitrary data plotting.

	// Predefined sizes [pixels side count N].
	_Sim.sizeCount = 1;
	_Sim.sizeList[0] = 128;
	_Sim.sizeList[1] = 256;
	_Sim.sizeList[2] = 512;
	_Sim.sizeList[3] = 1024;

	// Predefined hit densities [hits/mm^2].
	_Sim.hitsCount = 1;
	_Sim.hitsList[0] = 0.1;
	_Sim.hitsList[1] = 0.2;
	_Sim.hitsList[2] = 0.5;
	_Sim.hitsList[3] = 1.0;
	_Sim.hitsList[4] = 2.0;
	_Sim.hitsList[5] = 5.0;
	
	// Predefined multiplicities for the fW map.
	_Sim.multCount = 1;
	_Sim.multList[0] = 1;
	_Sim.multList[1] = 2;
	_Sim.multList[2] = 3;
	_Sim.multList[3] = 4;

	// Predefined pixel sizes [mm].
	_Sim.pxszCount = 1;
	_Sim.pxszList[0] = 0.010;
	_Sim.pxszList[1] = 0.015;
	_Sim.pxszList[2] = 0.020;

	// Clusters properties.
	_Sim.clzsCount = 6;			// Number of cluster properties.
	_Sim.clzsParam[0] = 100;	// Cluster average seed height.
	_Sim.clzsParam[1] = 10;		// Cluster average seed height sigma.	
	_Sim.clzsParam[2] = 12;		// Cluster average size in pixels.
	_Sim.clzsParam[3] = 4;		// Cluster average size sigma.
	_Sim.clzsParam[4] = 1;		// Cluster average aspect ratio (x/y).
	_Sim.clzsParam[5] = 0;		// Cluster average aspect ratio sigma.

	// Clusters collection roots.
	_Sim.clzsRootsCount = 0;	// Number of roots selected.
	_Sim.clzsRootsParam[0] = 0;	// First selected root.
	_Sim.clzsRootsParam[1] = 0;	// First selected root.	

	// Noise parameters.
	_Sim.noiseCount = 2;
	_Sim.noiseParam[0] = 0;		// Noise average is zero.
	_Sim.noiseParam[1] = 0;		// Noise sigma is zero.
}

//______________________________________________________________________________
Ana::~Ana()
{
	/*! Default dtor. */
}

// #############################################################################
}} // CLose namespaces.
