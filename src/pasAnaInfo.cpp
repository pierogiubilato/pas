//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Analysis Info Unit					  --
// (C) Piero Giubilato 2010-2012, Padova University							  --
//------------------------------------------------------------------------------


//______________________________________________________________________________
// [File name]		"pasAnaInfo.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"10 Oct 2012"
// [Language]		"C++"
//______________________________________________________________________________

// Analysis common header.
#include "pasConsole.h"
#include "pasAnaInfo.h"
#include "pasMain.h"

// #############################################################################
namespace pas { namespace ana {

//______________________________________________________________________________
void Info(pas::obj::Ana& MA)
{
	/*! Show informations about the analysis setup. Also generate some basic
	 *	reference plots about different pixel representation approaches.
	 */
   	
	// Info.
	std::cout << COL(LCYAN);
	std::cout << "\nAnalysis Info\n";
	std::cout << "-------------\n\n";
	std::cout << COL(DEFAULT);

	// Simulation info.
	
	// MC runs.
	std::cout << "Monte Carlo runs:  " 
			  << COL(LWHITE) << MA._Sim.MCRuns << COL(DEFAULT) << "\n";
	
	// Poisson mode.
	std::cout << "Poisson mode:      "; 
	MA._Sim.flagPoisson ? std::cout << COL(LGREEN) << "ON" : std::cout << COL(LRED) << "OFF"; 
	std::cout << COL(DEFAULT) <<"\n"; 
	
	// Noisy mode.
	std::cout << "Noisy mode:        "; 
	MA._Sim.flagNoise ? std::cout << COL(LGREEN) << "ON" : std::cout << COL(LRED) << "OFF"; 
	std::cout << COL(DEFAULT) <<"\n"; 
	
	// Cluster mode.
	std::cout << "Cluster mode:      "; 
	MA._Sim.flagCluster ? std::cout << COL(LGREEN) << "ON" : std::cout << COL(LRED) << "OFF"; 
	std::cout << COL(DEFAULT) <<"\n"; 
	
	// Frame size.
	std::cout << "Frame size (N):    " << COL(BLUE);
	for (unsigned int i = 0; i < MA._Sim.sizeCount; i++) std::cout << MA._Sim.sizeList[i] << " ";
	std::cout << COL(DEFAULT) << "\n";
	
	// Pixel size.
	std::cout << "Pixel sizes [mm]:  " << COL(LGREEN);
	for (unsigned int i = 0; i < MA._Sim.pxszCount; i++) std::cout << MA._Sim.pxszList[i] << " ";
	std::cout << COL(DEFAULT) << "\n";
	
	// Hits density.
	if (MA._Sim.flagHitsAbsolute) std::cout << "Hits [hits/frame]: " << COL(LRED);
	else std::cout << "Hits [hits/mm2]:   " << COL(LRED);	
	for (unsigned int i = 0; i < MA._Sim.hitsCount; i++) std::cout << MA._Sim.hitsList[i] << " ";
	std::cout << COL(DEFAULT) << "\n";
	
	// fW multiplicity.
	std::cout << "fW multiplicities: " << COL(LCYAN);
	for (unsigned int i = 0; i < MA._Sim.multCount; i++) std::cout << MA._Sim.multList[i] << " ";
	std::cout << COL(DEFAULT) << "\n";
	
	
	// Noise.
	std::cout << "Noise [avg, sgm]:  " << COL(PURPLE);
	std::cout << MA._Sim.noiseParam[0] << COL(DEFAULT) << ", "
			  << COL(PURPLE) << MA._Sim.noiseParam[1] << COL(DEFAULT) << "\n";
	
	// Cluster properties.
	if (MA._Sim.flagCluster) {
		std::cout << "Clusters properties:  \n"
				  << "   seed avg [ADC]: " << COL(YELLOW) << MA._Sim.clzsParam[0] << COL(DEFAULT) << "\n" 
				  << "   seed sgm [ADC]: " << COL(YELLOW) << MA._Sim.clzsParam[1] << COL(DEFAULT) << "\n"
				  << "   size avg [px]:  " << COL(YELLOW) << MA._Sim.clzsParam[2] << COL(DEFAULT) << "\n" 
				  << "   size sgm [px]:  " << COL(YELLOW) << MA._Sim.clzsParam[3] << COL(DEFAULT) << "\n"
				  << "   aspect [x/y]:   " << COL(YELLOW) << MA._Sim.clzsParam[4] << COL(DEFAULT) << "\n"
				  << "   aspect sgm:     " << COL(YELLOW) << MA._Sim.clzsParam[5] << COL(DEFAULT) << "\n";
	}

	if (MA._Sim.clzsRootsCount > 0) {
		std::cout << "Clusters roots:  \n";
		for (unsigned int i = 0; i < MA._Sim.clzsRootsCount; i++) {
			std::cout << MA._Sim.clzsRootsParam[i] << ": \n";
		}
		std::cout << COL(DEFAULT) << "\n";
	}

	// Analysis info.
	std::cout << "Run reco fW:       "; 
	MA._Ana.flagW ? std::cout << COL(LGREEN) << "ON" : std::cout << COL(LRED) << "OFF"; 
	std::cout << COL(DEFAULT) <<"\n"; 
	std::cout << "Run reco XYUV:     "; 
	MA._Ana.flagUV ? std::cout << COL(LGREEN) << "ON" : std::cout << COL(LRED) << "OFF"; 
	std::cout << COL(DEFAULT) <<"\n\n"; 

	// Tetra parameters.
	std::cout << "XYUV parameters:   " 
			  << " thr0: " << COL(LYELLOW) << MA._Ana.tetraParam[0] << COL(DEFAULT)
			  << ", range: " << COL(LYELLOW) << MA._Ana.tetraParam[1] << COL(DEFAULT)
			  << ", x bits: " << COL(LYELLOW) << MA._Ana.tetraParam[2] << COL(DEFAULT) << "\n"; 
		
}

// #############################################################################
}} // Close namespaces.