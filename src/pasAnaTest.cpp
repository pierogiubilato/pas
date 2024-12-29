//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Analysis Setup Unit				  --
// (C) Piero Giubilato 2010-2012, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasAnaTest.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.0"
// [Modified by]	"Piero Giubilato"
// [Date]			"16 Jan 2012"
// [Language]		"C++"
//______________________________________________________________________________


// PAS general.
#include "pasConsole.h"

// Analysis common header.
#include "pasAnaTest.h"
#include "pasObjLayer.h"


// #############################################################################
namespace pas { namespace ana {

//______________________________________________________________________________
void Test(pas::obj::Ana& MA)
{
   	/*!	Tests the various analysis objects. This unit is intended to perform
	 *	quality/safety tests to check single elements for speed and reliability. 
	 *	It is not assumed to perform any analysis logic check! 
	 */

	// Unit title.
	std::cout << COL(LCYAN);
	std::cout << "Objects test\n";
	std::cout << "------------\n\n";
	std::cout << COL(DEFAULT);

	// Test Layer.
	std::cout << "16x16 layer demo\n";
	//pas::obj::Layer ly(16,16);
	//ly.FillHits(3);
	
	// Show whole layer.
	ly.Dump();
	std::cout << "\n"; 
 	
	// Show X projection.
//	std::cout << "X Projection (bitMap encoded)\n";
//	for (size_t i = 0; i < ly.GetPrjX().size(); i++) {
//		std::cout << ly.GetPrjX()[i] << " ";
//	} std::cout << "\n\n";		

	// Show Y projection.
//	std::cout << "Y Projection (bitMap encoded)\n";
//	for (size_t i = 0; i < ly.GetPrjY().size(); i++) {
//		std::cout << ly.GetPrjY()[i] << " ";
//	} std::cout << "\n\n";		

	// Show hitlist projection.
	std::cout << "Hitlist (Idx, X, Y)\n";
	for (size_t i = 0; i < ly.GetHitIdx().size(); i++) {
		std::cout << "   " << i << ": [" 
				  << ly.GetHitIdx()[i] << "] - (" 
				  << ly.GetHitX()[i] << ", " 
				  << ly.GetHitY()[i] << ")\n";
	} std::cout << "\n";		

}

// #############################################################################
}} // Close namespaces