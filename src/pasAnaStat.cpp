//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulaiton - Analysis Statistical checks Unit	  --
// (C) Piero Giubilato 2010-2012, Padova University							  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pas_ana_Stat.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.0"
// [Modified by]	"Piero Giubilato"
// [Date]			"21 Mar 2012"
// [Language]		"C++"
//______________________________________________________________________________
 
// PAS general.
#include "pasConsole.h"

// Analysis common header.
#include "pasAnaStat.h"
#include "pasMain.h"

// #############################################################################
namespace pas { namespace ana {

//______________________________________________________________________________
void Stat(pas::obj::Ana& MA)
{
 //  	/*!	Check Poisson behaviour for randomly distributed hits over a frame. 
	// *	Instead of only using a Poisson-like generate statistic, further
	// *	improve the radomness by sub-frame sampling technique.
	// */

	//// Unit header.
	//std::cout << COL(LCYAN);
	//std::cout << "\nStatistics Checks\n";
	//std::cout << "-----------------\n\n";
	//std::cout << COL(DEFAULT);

	//// Settings.
	//// ---------

	//	// Density steps.
	//	
	//
	//// Data storage.
	//// -------------
	//
	//	// Step title.
	//	std::vector<std::string> rStepTitle;

	//	// Storage histogram.	
	//	std::vector<TH2I*> rHstLayer;
	//	std::vector<TH1I*> rHstHitsCount;
	//	std::vector<TH1I*> rHstPrjX;
	//	std::vector<TH1I*> rHstPrjY;
	//	std::vector<TH1I*> rHstPrjXCells;
	//	std::vector<TH1I*> rHstPrjYCells;

	//	// Storage results
	//	std::vector<double> rHitsDensity;
	//	std::vector<double> rFrameArea;
	//	std::vector<double> rFrameHitsAvg;
	//	std::vector<double> rFrameHitsCount;
	//	std::vector<double> rFrameHitsSingle;
	//	std::vector<double> rFrameHitsDouble;
	//	std::vector<double> rFrameHitsOver;
	//	std::vector<double> rColHitsCount;
	//	std::vector<double> rColHitsSingle;
	//	std::vector<double> rColHitsOver;
	//	std::vector<double> rRowHitsCount;
	//	std::vector<double> rRowHitsSingle;
	//	std::vector<double> rRowHitsOver;

	//	// Timing
	//	cake::Timer timer;

	//
	//// Data presets.
	//// -------------
	//
	//	// Loop over the tested sizes (cols).
	//	for (unsigned int sizeIdx = 0; sizeIdx < MA._Sim.sizeCount; sizeIdx++) {

	//		// Square/Rectangular shape (still to be completed).
	//		//unsigned int cols = static_cast<unsigned int>(pow(2.0, static_cast<double>(MA._Sim.sizeList[sizeIdx])));
	//		unsigned int cols = MA._Sim.sizeList[sizeIdx];
	//		unsigned int rows = cols;

	//		// Set how many graph per hit step.
	//		unsigned int rStepGraphCount = 3;
	//		
	//		// Generates a form for the current steps set.
	//		std::stringstream formTitle("");
	//		formTitle << "Stat checks with " << cols * rows << " pixels" ;
	//		MA._Form.push_back(new race::ui::Form(formTitle.str().c_str(), 800, 400));  
	//		MA._Form.back()->Canvas()->Divide(3, MA._Sim.hitsCount);
	//		
	//		// Loop over the tested hits per frame.	
	//		for (unsigned int hitsIdx = 0; hitsIdx < MA._Sim.hitsCount; hitsIdx++) {
	//							
	//			// Generates a title for the current step.
	//			std::stringstream title("");
	//			title << "Cols: " << cols << ", Rows: " << rows 
	//				  << ", Avg hit density: " << MA._Sim.hitsList[hitsIdx]  
	//				  << ", Poisson is: " << (MA._Sim.flagPoisson ? "ON" : "OFF");
	//			rStepTitle.push_back(title.str()); 	
	//			
	//			// Compute the actual number of average hits per frame.
	//			double frameArea = cols * rows * MA._Chip.pxWidth * MA._Chip.pxHeight;
	//			double hitsCount = MA._Sim.hitsList[hitsIdx] * frameArea;
	//			rFrameArea.push_back(frameArea);
	//			rHitsDensity.push_back(MA._Sim.hitsList[hitsIdx]); 
	//			rFrameHitsAvg.push_back(hitsCount);  
	//			
	//			// The hits count plot.
	//			std::stringstream hstTitle;
	//			hstTitle << "Hit count. " << title.str(); 
	//			rHstHitsCount.push_back(new TH1I("", hstTitle.str().c_str(), static_cast<unsigned int>(4 * hitsCount), 0, 4 * hitsCount));
	//			MA._Form.back()->Canvas()->cd(1 + rStepGraphCount * hitsIdx);
	//			rHstHitsCount.back()->Draw();

	//			// The X projection plot.
	//			hstTitle.clear();
	//			hstTitle << "X projection. " << title.str(); 
	//			rHstPrjX.push_back(new TH1I("", hstTitle.str().c_str(), cols, 0, cols));
	//			MA._Form.back()->Canvas()->cd(2 + rStepGraphCount * hitsIdx);
	//			rHstPrjX.back()->Draw();

	//			// The Y projection plot.
	//			hstTitle.clear();
	//			hstTitle << "Y projection. " << title.str(); 
	//			rHstPrjY.push_back(new TH1I("", hstTitle.str().c_str(), rows, 0, rows));
	//			MA._Form.back()->Canvas()->cd(3 + rStepGraphCount * hitsIdx);
	//			rHstPrjY.back()->Draw();

	//			// Initializes the result containers.
	//			rFrameHitsSingle.push_back(0);
	//			rFrameHitsDouble.push_back(0);
	//			rFrameHitsOver.push_back(0);
	//			rFrameHitsCount.push_back(0);
	//			rColHitsSingle.push_back(0);
	//			rColHitsOver.push_back(0);
	//			rColHitsCount.push_back(0);
	//			rRowHitsSingle.push_back(0);
	//			rRowHitsOver.push_back(0);
	//			rRowHitsCount.push_back(0);

	//		} // Hits loop.
	//	
	//	} // Cols loop.

	//
	//// Runs MC
	//// -------

	//	// Stepping, used here and for the data display loop.
	//	unsigned int steps = MA._Sim.sizeCount * MA._Sim.hitsCount;
	//	unsigned int step = 0;
	//	
	//	// Timing, used just for the MC runs.		
	//	std::cout << "Running MCs... ";
	//	timer.Start();
	//	
	//	// Loop over the tested sizes (cols).
	//	for (unsigned int sizeIdx = 0; sizeIdx < MA._Sim.sizeCount; sizeIdx++) {

	//		// Square/Rectangular shape (still to be completed).
	//		unsigned int cols = MA._Sim.sizeList[sizeIdx];
	//		unsigned int rows = cols;
	//		unsigned int cnt = 0;

	//		// Define a test layer.
	//		pas::obj::Layer ly(cols, rows); 

	//		// Loop over the tested hits per frame.	
	//		for (unsigned int hitsIdx = 0; hitsIdx < MA._Sim.hitsCount; hitsIdx++) {

	//			// Loop over the MC runs.	
	//			for (unsigned int runIdx = 0; runIdx < MA._Sim.MCRuns; runIdx++) {
	//				
	//				// Fill the layer.
	//				(MA._Sim.flagPoisson) ? ly.FillPoisson(rFrameHitsAvg[step]) : ly.FillPrecise(rFrameHitsAvg[step]);
	//				
	//				// Get actual number of hits.
	//				cnt = ly.GetHitIdx().size();
	//				rFrameHitsCount[step] += cnt;
	//						
	//				// Frame Hits multiplicity indexes.
	//				if (cnt == 1) rFrameHitsSingle[step]++;
	//				else if (cnt == 2) rFrameHitsDouble[step]++;
	//				else if (cnt > 2) rFrameHitsOver[step]++;
	//				rHstHitsCount[step]->Fill(cnt);

	//				// X Projection Hits multiplicity indexes.
	//				for (unsigned int x = 0; x < cols; x++) {
	//					//cnt = ly.GetPrjXCnt()[x];
	//					if(cnt > 0) {
	//						rColHitsCount[step] += cnt;
	//						if(cnt == 1) rColHitsSingle[step] ++;
	//						else if(cnt > 1) rColHitsOver[step] += cnt;
	//						rHstPrjXCells[step]->Fill(cnt);
	//					}
	//				}
	//				
	//				// Y Projection Hits multiplicity indexes.
	//				for (unsigned int y = 0; y < rows; y++) {
	//					//cnt = ly.GetPrjYCnt()[y];
	//					if(cnt > 0) {
	//						rRowHitsCount[step] += cnt;
	//						if(cnt == 1) rRowHitsSingle[step] ++;
	//						else if(cnt > 1) rRowHitsOver[step] += cnt;
	//						rHstPrjYCells[step]->Fill(cnt);
	//					}
	//				}

	//			} // MC LOOP

	//			// Stepping index.
	//			step++;

	//			// Info.
	//			std::cout << "[" << step << "/" << steps << "] ";
	//			

	//		} // Hits LOOP
	//	} // Size LOOP

	//	// Timing info.
	//	timer.Stop();
	//	std::cout << "done in " << timer.Elapsed() << "s\n\n";


	//// Sum up numerical results.
	//// -------------------------	
	//
	//	// Stepping.
	//	step = 0;

	//	// Loop over the tested sizes (cols).
	//	for (unsigned int step = 0; step < steps; step++) {
	//			
	//		// Frame
	//		rFrameHitsSingle[step] /= MA._Sim.MCRuns;
	//		rFrameHitsDouble[step] /= MA._Sim.MCRuns;
	//		rFrameHitsOver[step] /= MA._Sim.MCRuns;
	//		rFrameHitsCount[step] /= static_cast<double>(MA._Sim.MCRuns);
	//		
	//		// Cols.
	//		rColHitsSingle[step] /= rColHitsCount[step];
	//		rColHitsOver[step] /= rColHitsCount[step];
	//		rColHitsCount[step]	/= static_cast<double>(MA._Sim.MCRuns);

	//		// Rows.		
	//		rRowHitsSingle[step] /= rRowHitsCount[step];
	//		rRowHitsOver[step] /= rRowHitsCount[step];
	//		rRowHitsCount[step]	/= static_cast<double>(MA._Sim.MCRuns);
	//		
	//		// Show numerical results.
	//		std::cout << rStepTitle[step] << "\n";
	//		std::cout << std::setprecision(4);
	//		std::cout << "   Frame area:        " << COL(LWHITE) << rFrameArea[step] << COL(DEFAULT) << " [mm2]\n";
	//		std::cout << "   Avg hits count:    " << COL(LWHITE) << rFrameHitsCount[step] << COL(DEFAULT) << " [hits/frame]\n";
	//		std::cout << "   Avg hits density:  " << COL(LRED) << rHitsDensity[step] << COL(DEFAULT) << " [hits/mm2]\n";
	//		std::cout << "   Single hits:       " << COL(LGREEN) << rFrameHitsSingle[step] << COL(DEFAULT) << "\n";
	//		std::cout << "   Double hits:       " << COL(LWHITE) << rFrameHitsDouble[step] << COL(DEFAULT) << "\n";			
	//		std::cout << "   Multiple hits:     " << COL(LWHITE) << rFrameHitsOver[step] << COL(DEFAULT) << "\n";
	//		//std::cout << "   Col average hits:  " << COL(YELLOW) << rColHitsCount[step] << COL(DEFAULT) << "\n";
	//		//std::cout << "   Col single hit:    " << COL(LYELLOW) << rColHitsSingle[step] << COL(DEFAULT) << "\n";
	//		//std::cout << "   Col multiple hits: " << COL(LYELLOW) << rColHitsOver[step] << COL(DEFAULT) << "\n";
	//		//std::cout << "   Row average hits:  " << COL(CYAN) << rRowHitsCount[step] << COL(DEFAULT) << "\n";
	//		//std::cout << "   Row single hit:    " << COL(LCYAN) << rRowHitsSingle[step] << COL(DEFAULT) << "\n";
	//		//std::cout << "   Row multiple hits: " << COL(LCYAN) << rRowHitsOver[step] << COL(DEFAULT) << "\n";
	//	}

}

// #############################################################################
}} // CLose namespaces.