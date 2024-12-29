//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Main entry point					  --
// (C) Piero Giubilato 2011-2025, Padova University							  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasMain.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"24 Oct 2012"
// [Language]		"C++"
//______________________________________________________________________________


// Root app.
#include "TApplication.h"
//#include "TRint.h"
//#include "TCanvas.h"
//#include "TRootCanvas.h"

// Main PAS header.
#include "pasMain.h"
#include "pasConsole.h"

// PAS objects.
#include "pasObjAna.h"
#include "pasObjCluster.h"
#include "pasMisc.h"

// PAS analysis units.
//#include "pasAnaSetup.h"
#include "pasAnaInfo.h"
#include "pasAnaReco.h"


//______________________________________________________________________________
// Inner "private" functions declaration.
void credits(const int& argc);
void cmdLine(const int& argc, char** argv, pas::obj::Ana& MA);


// *****************************************************************************
// **						  Application Entry Point						  **
// *****************************************************************************

//______________________________________________________________________________
int main(int argc, char** argv)
{
	/*! The analysis entrance function. */

	// Define the Main Analysis object.
	pas::obj::Ana MA;

	// Show credits.
	credits(argc);

	// Parse the command line.
	cmdLine(argc, argv, MA);

	// Launch ROOT application.
	argc = 1;
	TApplication rootApp("PAS", &argc, argv);

	// Instantiate analysis units.
	pas::Misc misc;
	pas::ana::Reco anaReco(MA);

	// Plot arbitrary data
	//if (MA._Misc.flagDataPlot) misc.DataPlot(MA);

	// Performs a simulation and analysis.
	if (!MA._Misc.flagDataPlot && MA._Ana.flagReco) {
		pas::ana::Info(MA);		// Shows analysis setting and useful info.
		//if (MA._Ana.flagStat) pas::ana::Stat(MA);	// Performs statistical checks.
		anaReco.Run();		// Performs reco algorithm MCs.
	}

	// Reset console color.
	std::cout << COL(DEFAULT);

	// Run interactivity if any form to display.
	if (MA._Canvas.size() >= 1) rootApp.Run();

	// After the user quit, deletes all the forms.
	for (auto i = 0; i < MA._Canvas.size(); i++) delete MA._Canvas[i];

	// Exit application
	std::cout << COL(DEFAULT);
	std::cout << "Bye bye!\n";
	return 0;
}


// *****************************************************************************
// **							Application Utilities						  **
// *****************************************************************************

//______________________________________________________________________________
void cmdLine(const int& argc, char** argv, pas::obj::Ana& MA)
{
	/*! Parse the command line. Saves all the given parameter into the provided
	 *	pas::obj::Ana structure.
	 */

	 // Types
	enum kArg { kaNull, kaRuns, kaSize, kaHits, kaMult, kaPxsz, kaClzs, kaClzsRoots, kaNoise, kaXYUV };
	int argType = kaNull;
	const int tknCount = 10;			// Max # different tokens handled.
	const int prmCountLimit = 32;	// Max # parameters per token handled.
	int prmCount[tknCount];
	double prmStore[tknCount][prmCountLimit];
	for (int i = 0; i < tknCount; i++) prmCount[i] = 0;

	// Load command line arguments/switches.
	for (int i = 0; i < argc; i++) {

		// Parameters.
		std::string argString = argv[i];
		if (argString == "-r") { argType = kaRuns; continue; }
		if (argString == "-s") { argType = kaSize; continue; }
		if (argString == "-h") { argType = kaHits; continue; }
		if (argString == "-m") { argType = kaMult; continue; }
		if (argString == "-p") { argType = kaPxsz; continue; }
		if (argString == "-z") { argType = kaNoise; continue; }
		if (argString == "-c") { argType = kaClzs; continue; }
		if (argString == "-cR") { argType = kaClzsRoots; continue; }
		if (argString == "-t") { argType = kaXYUV; continue; }

		// Switches (Simulation).
		if (argString == "-P") { MA._Sim.flagPoisson = true;	argType = kaNull; continue; }
		if (argString == "-hA") { MA._Sim.flagHitsAbsolute = true; argType = kaNull; continue; }
		if (argString == "-C") { MA._Sim.flagCluster = true; argType = kaNull; continue; }
		if (argString == "-Z") { MA._Sim.flagNoise = true; argType = kaNull; continue; }
		if (argString == "-Debug") { MA._Sim.flagDebug = true; argType = kaNull; continue; }
		if (argString == "-DebugMaps") { MA._Sim.flagDebugMaps = true; argType = kaNull; continue; }
		if (argString == "-DebugLayer") { MA._Sim.flagDebugLayer = true; argType = kaNull; continue; }

		// Switches (Analysis).
		if (argString == "-S") { MA._Ana.flagStat = true; argType = kaNull; continue; }
		if (argString == "-R") { MA._Ana.flagReco = true; argType = kaNull; continue; }
		if (argString == "-D") { MA._Ana.flagDerive = true; argType = kaNull; continue; }
		if (argString == "-W") { MA._Ana.flagW = true; argType = kaNull; continue; }
		if (argString == "-UV") { MA._Ana.flagUV = true; argType = kaNull; continue; }

		// Switches (Others functions).
		if (argString == "-!DP") { MA._Misc.flagDataPlot = true; argType = kaNull; continue; }

		// Load token's values into prmStore (doing boundary check on prmStore size).
		if (prmCount[argType] < prmCountLimit) {
			std::istringstream param(argv[i]);
			param >> prmStore[argType][prmCount[argType]];
			prmCount[argType]++;
		}
	}

	// Feed the analysis object with the given parameters.
	for (int tknIdx = 0; tknIdx < tknCount; tknIdx++) {

		// For every loaded token's parameter.
		for (int prmIdx = 0; prmIdx < prmCount[tknIdx]; prmIdx++) {

			// Retrive the value.
			double prmValue = prmStore[tknIdx][prmIdx];

			// Assigns parameter value to the correct analysis property.
			switch (tknIdx) {
			case kaNull: break;
			case kaRuns: MA._Sim.MCRuns = static_cast<int>(prmValue); break;
			case kaSize: MA._Sim.sizeList[prmIdx] = static_cast<int>(prmValue); break;
			case kaHits: MA._Sim.hitsList[prmIdx] = prmValue; break;
			case kaMult: MA._Sim.multList[prmIdx] = static_cast<int>(prmValue); break;
			case kaPxsz: MA._Sim.pxszList[prmIdx] = prmValue; break;
			case kaNoise: MA._Sim.noiseParam[prmIdx] = prmValue; break;
			case kaClzs: MA._Sim.clzsParam[prmIdx] = prmValue; break;
			case kaClzsRoots: MA._Sim.clzsRootsParam[prmIdx] = prmValue; break;
			case kaXYUV: MA._Ana.tetraParam[prmIdx] = static_cast<int>(prmValue); break;
			}
		}

		// Save token's parameters count.
		if (prmCount[tknIdx] > 0) {
			switch (tknIdx) {
			case kaNull: break;
			case kaRuns: break;
			case kaSize: MA._Sim.sizeCount = prmCount[tknIdx]; break;
			case kaHits: MA._Sim.hitsCount = prmCount[tknIdx]; break;
			case kaMult: MA._Sim.multCount = prmCount[tknIdx]; break;
			case kaPxsz: MA._Sim.pxszCount = prmCount[tknIdx]; break;
			case kaClzs: MA._Sim.clzsCount = prmCount[tknIdx]; break;
			case kaClzsRoots: MA._Sim.clzsRootsCount = prmCount[tknIdx]; break;
			}
		}
	}
}

//______________________________________________________________________________
void credits(const int& argc)
{
	/*! Show credits */

	// Simple defines.
#define CLW COL(LWHITE)
#define CD COL(DEFAULT)

// Title.
	std::cout << COL(LCYAN) << "\n-------------------------------------\n"
		<< COL(LCYAN) << "       P" << CD << "ixel"
		<< COL(LCYAN) << " A" << CD << "nalysis"
		<< COL(LCYAN) << " W" << CD << "orkshop\n";

	// (C) and credits.
	std::cout << "         Built: " << COL(YELLOW)
		<< __DATE__ << CD
		<< "\n" << "   (C) " << COL(LBLUE)
		<< "Padova University" << CD
		<< " 2011-2025\n" << "     " << CLW
		<< "      Piero Giubilato" << COL(LCYAN)
		<< "\n-------------------------------------\n\n" << CD;


	// Short usage help in case.
	// -------------------------
	if (argc == 1) {

		// Command line.
		std::cout << CLW << "Usage:\n" << CD
			<< "pas [-P] [-T] [-D] [-S] [-R] [-W] [-C] [-Z] [-UV] [-hA] [-r] {runs} [-s] {size1, size2, ..., sizeN}\n"
			<< "[-h] {hits1, hits2, ..., hitsN} [-m] {mult1, mult2, ..., multN} [-n] {avg, sigma} \n"
			<< "[-c] {px avg, px sgm, ...} -cR {root1, root2, ...} [-Debug] [-DebugMaps] [-DebugLayer] \n"
			<< "[-t] {threshold, range, n bits} [-z] {avg, sigma}\n\n";

		// Simulation switches.
		std::cout << CLW << "Simulation switches:\n" << CD
			<< CLW << "-P" << CD << " uses Poisson statistic during symulation.\n"
			<< CLW << "-hA" << CD << " uses hit density as per frame instead of per mm2.\n"
			<< CLW << "-C" << CD << " enables clusters.\n"
			<< CLW << "-Z" << CD << " enables noise.\n"
			<< CLW << "-Debug" << CD << " shows general debug info (use with care!).\n"
			<< CLW << "-DebugMaps" << CD << " shows generated maps (use with care!).\n"
			<< CLW << "-DebugLayer" << CD << " shows complete layer (use with care!).\n\n";

		// Simulation Parameters.
		std::cout << CLW << "Simulation parameters tokens:\n" << CD
			<< "Parameters are set by a token argument (ex. '-s') followed by as many values as needed.\n"
			<< "Value argument will be assigned to the latest token argument found in the command line.\n"
			<< CLW << "-r " << CD << "sets the number of Monte Carlo runs.\n"
			<< CLW << "-s " << CD << "sets the matrix side dimension.\n"
			<< CLW << "-h " << CD << "sets the hit density [hits/mm2 by default or hits/frame if -hA is used].\n"
			<< CLW << "-p " << CD << "sets the pixel size [mm].\n"
			<< CLW << "-z " << CD << "sets the noise [avg, sigma].\n"
			<< CLW << "-c " << CD << "sets the clusters properties (seed avg, seed sgm, mult avg, mult sig, , shape avg, shape sig).\n"
			<< CLW << "-cR " << CD << "sets the cluster collection roots (rootIdx1, rootIdx2, ...).\n"
			<< "For every token a maximum of 32 values can be probvided.\n"
			<< "Token order is not important. If no parameters are given, default values are:\n"
			<< "-r = 1000 runs.\n"
			<< "-s = 256.\n"
			<< "-h = 0.5 hits/mm2.\n"
			<< "-p = 0.010 mm.\n"
			<< "-c = 100 ADCs avg, 10 ADCs sigma, 5 px avg, 1 px sigma, 1 aspect ratio, 0 sigma.\n"
			<< "-z = 0 avg, 0 sigma.\n\n";

		// Analysis Switches.
		std::cout << CLW << "Analysis switches:\n" << CD
			<< CLW << "-R" << CD << " performs reconstruction analysis.\n"
			<< CLW << "-S" << CD << " performs statistical analysis.\n"
			<< CLW << "-D" << CD << " enables derivative pixel.\n"
			<< CLW << "-UV" << CD << " performs UV mapping.\n"
			<< CLW << "-W" << CD << " performs W mapping.\n\n";

		// Analysis Parameters.
		std::cout << CLW << "Analysis parameters tokens:\n" << CD
			<< "Parameters are set by a token argument (ex. '-s') followed by as many values as needed.\n"
			<< "Value argument will be assigned to the latest token argument found in the command line.\n"
			<< CLW << "-m " << CD << "sets the fW function multiplicity.\n"
			<< CLW << "-t " << CD << "sets the XYUV alg parameters (threshold, range, extra bits).\n"
			<< "For every token a maximum of 32 values can be probvided.\n"
			<< "Token order is not important. If no parameters are given, default values are:\n"
			<< "-m = 1 (fW image size = m*N).\n"
			<< "-t = 0 counts, 400 counts, 0 extra bits.\n\n";

		// Available cluster roots.
		pas::obj::Cluster clDummy;
		std::cout << CLW << "Available cluster roots:\n" << CD;
		for (unsigned int r = 0; r < clDummy.CollectionRootCount(); r++) {
			std::cout << "Idx: " << CLW << r << CD << " Root: "
				<< CLW << clDummy.CollectionRootTitle(r) << CD << " Description: "
				<< CLW << clDummy.CollectionRootDescription(r) << CD << "\n";
		}
		std::cout << "\n";

		// Examples.
		std::cout << CLW << "Examples:\n" << CD
			<< "pas -S -P"
			<< "pas -r 100000 -s 128 256 -h 5 2 1 0.5 0.05 0.01\n"
			<< "pas -h 5 2 1 -p 0.010 0.050\n"
			<< "pas -r 1000000 -s 1024 -h 1\n"
			<< "pas -R -r 100000 -m 1 2 3 4 -s 513 1025 -h 0.5 2\n -P -Debug"
			<< "...\n\n";
	}

	// Clear defines.
#undef CLW
#undef CD
}