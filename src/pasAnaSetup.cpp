//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Analysis Setup Unit				  --
// (C) Piero Giubilato 2010-2012, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
//
// The pas_Main.cpp translation unit provides the general environmental settings 
// plus a main entrance routine. All the necessary application headers and 
// libraries are defined into pas_Main.h. Translation-unit specific headers had 
// to be added here.
// The translation units need to provide unique function naming as they all 
// share the same pas namespace (pas_Main.cpp takes care of opening and closing 
// it).
// Convention is that the particular analysis carryed out by the translation
// unit is started by invoking a function named as the last word of the cpp file
// name, i.e. pas_ana_Test.cpp has a function called Test(pas::obj::Ana& MA) which
// starts the job. The main function must accept a general pas::obj::Ana& object
// as argument. The pas::obj::Ana& object simply contains all the shared data of
// the current analysis.	
//______________________________________________________________________________

//______________________________________________________________________________
// [File name]		"pasAnaSetup.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.0"
// [Modified by]	"Piero Giubilato"
// [Date]			"17 Oct 2011"
// [Language]		"C++"
//______________________________________________________________________________
 
// PAS general.
#include "pasConsole.h"

// Analysis common header.
#include "pasAnaSetup.h"
#include "pasMain.h"

// #############################################################################
namespace pas { namespace ana {

//______________________________________________________________________________
void Setup(pas::obj::Ana& MA)
{
   	/*!	Sets all the analysis parameters.*/

	// Unit header.
	std::cout << COL(LCYAN);
	std::cout << "Analysis Setup\n";
	std::cout << "--------------\n\n";
	std::cout << COL(DEFAULT);




}

// #############################################################################
}} // CLose namespaces.