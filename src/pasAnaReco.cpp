//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Reconstruction test Unit			  --
// (C) Piero Giubilato 2010-2012, CERN										  --
//------------------------------------------------------------------------------


//______________________________________________________________________________
// [File name]		"pasAnaReco.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.5"
// [Modified by]	"Piero Giubilato"
// [Date]			"24 Oct 2012"
// [Language]		"C++"
//______________________________________________________________________________
 

// Root.
#include "TGraph.h"
#include "TGAxis.h"
#include "TStyle.h"
#include "TExec.h"

// PAS general.
#include "pasConsole.h"
#include "pasAnaReco.h"

// #############################################################################
namespace pas { namespace ana {


// *****************************************************************************
// **							  Special members							  **
// *****************************************************************************

//______________________________________________________________________________
Reco::Reco(pas::obj::Ana& MA): _MA(MA)
{
   	/*!	Default ctor. */

	// Initializes the random number generator.
	_Rand.SetSeed((unsigned int)(time(NULL) % 16384));
	
	// Reco	status.
	_rStepCurr = 0;

	// Tetra variables preset.
	_XYtoUV[0] = 0;		// Pointer to the XY->U map.
	_XYtoUV[1] = 0;		// Pointer to the XY->V map.
	_UVtoXY[0] = 0;		// Pointer to the UV->X map.
	_UVtoXY[1] = 0;		// Pointer to the UV->Y map.
	_TProj[3] = _TProj[2] = _TProj[1] = _TProj[0] = 0; // Pointers to the projections.
	_TUseTable = 0;		// Pointer to the XYUV usage table.
	_TLayerN = 0;		// Side of the Layer map.
	_TN = 0;			// Side of the XYUV maps (equal to the minimum higher odd of TLayerN).
	_TdigiMap = 0;

	// DPix variables preset.
	_DpxMap = 0;		// No derived map available.
}

//______________________________________________________________________________
Reco::~Reco()
{
	/*!	Default dtor. */
}



// *****************************************************************************
// **							  Workflow members							  **
// *****************************************************************************

//______________________________________________________________________________
void Reco::Run()
{
	// Unit header.
	std::cout << COL(LCYAN);
	std::cout << "\nReconstruction simulation\n";
	std::cout << "-------------------------\n\n";
	std::cout << COL(DEFAULT);
	
	// Setup data structures.
	Setup();

	// Timing, used just for the MC runs.		
	std::cout << COL(LWHITE) << "Running MCs" << COL(DEFAULT) << "...\n";
//	_Timer.Start();
		
	// Initialize the cluster generator.
	// ---------------------------------	
	if (_MA._Sim.flagDebug) std::cout << " Initializing cluster generator...\n";
		
		// Create the object.
		pas::obj::Cluster clGen;
		
		// Load a cluster collection in case.
		if (_MA._Sim.clzsRootsCount > 0) {
		
			// Get a local copy of the simulation requested roots.
			unsigned int* tmpRoot = new unsigned int[_MA._Sim.clzsRootsCount];
			for (unsigned int i = 0; i < _MA._Sim.clzsRootsCount; i++) {
				tmpRoot[i] = static_cast<unsigned int>(_MA._Sim.clzsRootsParam[i]); 
			}
			clGen.CollectionLoad(tmpRoot, _MA._Sim.clzsRootsCount); 
		
			// Remove pivots.
			delete[] tmpRoot;
		} 
		
		// Set random generation properties.
		clGen.RandomSet(_MA._Sim.clzsParam);   

	// Decide if we are gonna using clusters.
	bool clOn = false;
	if (_MA._Sim.clzsCount > 0 || _MA._Sim.clzsRootsCount > 0) clOn = true;

	// The projection pivot and reco hit referencea.
	std::vector<long double> prjW;
	std::vector<unsigned int> hitReco;
	std::vector<clObj> clxReco;

	// Stepping.
	unsigned int rSteps = _MA._Sim.sizeCount * _MA._Sim.pxszCount * _MA._Sim.hitsCount;
	_rStepCurr = 0;
	
	// Loop over the tested sizes (cols).
	for (unsigned int sizeIdx = 0; sizeIdx < _MA._Sim.sizeCount; sizeIdx++) {

		// Square/Rectangular shape (still to be completed).
		unsigned int cols = _MA._Sim.sizeList[sizeIdx];
		unsigned int rows = cols;
		unsigned int cnt = 0;

		// Defines a test layer.
		pas::obj::Layer ly(cols, rows, &clGen); 
		
		// Setup the Tetra (XYUV) algorithm data space.
		if (_MA._Ana.flagUV) TSetup(cols, 1);
		
		// Loop over the tested pixel sizes.	
		for (unsigned int pxszIdx = 0; pxszIdx < _MA._Sim.pxszCount; pxszIdx++) {

			// Loop over the tested hits per frame.	
			for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {

				// Loop over the MC runs.	
				for (unsigned int runIdx = 0; runIdx < _MA._Sim.MCRuns; runIdx++) {
					
					// Layer generation
					// ----------------
					
					// Fill the layer.
					ly.FillHits(_rFrameHitsAvg[_rStepCurr], _MA); 
					unsigned int hitsCount = ly.GetHitIdx().size();			
		
					// Store the actual frame hits count.
					_rRecoHitsCount[_rStepCurr] += hitsCount;
										
					// Debug (show hitlist).
					if (_MA._Sim.flagDebug) {
						
						std::cout << "\nHits raw list: (" << ly.GetHitIdx().size() << " hits)\n" << std::hex;
						for (unsigned int i = 0; i < ly.GetHitIdx().size(); i++) {
							std::cout << " #" << i << ": (" 
									  << COL(LWHITE) << ly.GetHitX()[i] << COL(DEFAULT) << ","
									  << COL(LWHITE) << ly.GetHitY()[i] << COL(DEFAULT) << ") [" 
									  << COL(LWHITE) << ly.GetHitIdx()[i] << COL(DEFAULT) << "]\n"; 
						}
						std::cout << std::dec << "\n";
					}
		
					// Debug (show layer pixel by pixel).
					if (_MA._Sim.flagDebugLayer) {
						
						// Creates the forms where to show the graphs.
						_MA._Canvas.push_back(new TCanvas("c5", "Layer Map", 100,100, 600, 600));  
						ly.Dump(true)->Draw("colz");
					}
					
					// Derivative pixel application.
					if (_MA._Ana.flagDerive) {

						// Defines a pivot pixel map.
						if (_DpxMap) delete[] _DpxMap;
						_DpxMap = new unsigned short[ly.GetPxCols() * ly.GetPxRows()];

						// Calculate the derived pixel map.
						DParse(_DpxMap, ly.GetPxMap(), ly.GetPxCols(), ly.GetPxRows());
					}

					//// W single projection mapping.
					//if (_MA._Ana.flagW) {
					//	if (_MA._Sim.flagDebug)  std::cout << "fW...\n";
					//	// Loop over the different multiplicities.
					//	for (unsigned int mIdx = 0; mIdx < _MA._Sim.multCount; mIdx++) {	
					//		// Get the fW image.
					//		WEncode(prjW, cols, _MA._Sim.multList[mIdx], ly.GetHitIdx());   
					//		// Try decoding the X, Y and W images.
					//		if (XYWDecode(hitReco, ly.GetPrjX(), ly.GetPrjY(), prjW)) {
					//			// Reco alg says reconstruction is ok.
					//			_rRecofWSelfPurity[step * _MA._Sim.multCount + mIdx]++;
					//			// Check if the returned hitlist is actually correct.
					//			if (compareIdxList(ly.GetHitIdx(), hitReco) == hitsCount) {
					//				_rRecofWRealPurity[step * _MA._Sim.multCount + mIdx] ++;
					//			}
					//		}
					//	} // mIdx LOOP
					//}
					
					// Tetra (T) projections mapping.
					if (_MA._Ana.flagUV) {
					
						// Debug info.
						if (_MA._Sim.flagDebug)  std::cout << COL(LRED) << "T-Stuff" << COL(DEFAULT) << "...\n";

						// Encode the four projections.
						//TEncode(_TProj, ly.GetHitX(), ly.GetHitY());					
						if (_MA._Ana.flagDerive) {
							TEncode(_TProj, _DpxMap, ly.GetPxCols(), ly.GetPxRows());
							delete[] _DpxMap;
						} else {
							TEncode(_TProj, ly.GetPxMap(), ly.GetPxCols(), ly.GetPxRows());
						}
						
						// Try decoding the X, Y, U and V projections.
						//if (TDecode(hitReco, _TProj)) {
						int hitsReco = TDecodeClx(clxReco, _TProj, &ly);
						
						// Report the number of correctly reconstructed hits.
						_rRecoUVHitsCount[_rStepCurr] += compareClxList(ly.GetHitIdx(), clxReco);
	
						// == hitsCount) _rRecoMOPRealPurity[_rStepCurr] ++;

							// Check if the returned hitlist is actually correct.
							//int okHits = compareIdxList(ly.GetHitIdx(), hitReco);
							//if (okHits == hitsCount) _rRecoMOPRealPurity[_rStepCurr] ++;
						//}
					}
					
					// Show Debug Map in case.
					if (_MA._Sim.flagDebug) debugMap(ly, ly.GetHitIdx(), hitReco);

				} // MC LOOP
									   \
				// Info.
				std::cout << COL(DEFAULT) << "Step [" << _rStepCurr + 1 << "/" << rSteps << "]\t" <<
							 "N = " << COL(LWHITE) << cols << COL(DEFAULT) << "\t hits/mm2 = " <<
							 COL(LRED) << _rHitsDensity[_rStepCurr] << COL(DEFAULT) << "\n";			  
				
				// Stepping index.
				_rStepCurr++;
									
			} // hitIdx LOOP
		
		} // pxszIdx LOOP

		// Clear the Tetra (XYUV) algorithm data space.
		if (_MA._Ana.flagUV) TClear();

	} // sizeIdx LOOP

	// Timing info.
//	_Timer.Stop();
//	std::cout << "\nOverall done in " << _Timer.Elapsed() << "s\n\n";

	// Show results.
	Show();	
}

//______________________________________________________________________________
void Reco::Setup()
{
	/*!	Inflate and initialize all the necessary data structures. */
		
	// Loop over the tested sizes (cols).
	for (unsigned int sizeIdx = 0; sizeIdx < _MA._Sim.sizeCount; sizeIdx++) {

		// Square/Rectangular shape (still to be completed).
		unsigned int cols = _MA._Sim.sizeList[sizeIdx];
		unsigned int rows = cols;
		
		// Loop over the pixel sizes.	
		for (unsigned int pxszIdx = 0; pxszIdx < _MA._Sim.pxszCount; pxszIdx++) {
		
			// Set current pixel size (only square pixels at the moment).	
			double pxWidth = _MA._Sim.pxszList[pxszIdx];
			double pxHeight = pxWidth;

			// Loop over the tested hits per frame.	
			for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {
								
				// Generates a title for the current step.
				std::stringstream title("");
				title << "Cols: " << cols << ", Rows: " << rows 
					  << ", <hit density>: " << _MA._Sim.hitsList[hitsIdx]  
					  << ", Poisson is: " << (_MA._Sim.flagPoisson ? "ON" : "OFF");
				_rStepTitle.push_back(title.str()); 	
				
				// Computes the actual number of average hits per frame.
				double hitsCount, hitsDensity;
				double frameArea = cols * rows * pxWidth * pxHeight;
				if (_MA._Sim.flagHitsAbsolute) {
					hitsCount = _MA._Sim.hitsList[hitsIdx];
					hitsDensity = hitsCount / frameArea;
				} else {
					hitsDensity = _MA._Sim.hitsList[hitsIdx];
					hitsCount = hitsDensity * frameArea;
				}
				
				// Store all the presets for the run.
				_rSizeColsN.push_back(cols); 
				_rSizeRowsN.push_back(rows);
				_rPxWidth.push_back(pxWidth); 
				_rPxHeight.push_back(pxHeight);
				_rFrameArea.push_back(frameArea);
				_rHitsDensity.push_back(hitsDensity); 
				_rFrameHitsAvg.push_back(hitsCount);  
				_rCoordHitsAvg.push_back(0);
				_rCoordOverkillCount[0].push_back(0);
				_rCoordOverkillCount[1].push_back(0);
				_rCoordOverkillCount[2].push_back(0);
				_rCoordOverkillCount[3].push_back(0);

				// Initializes the purity result containers.
				for (unsigned int mIdx = 0; mIdx < _MA._Sim.multCount; mIdx++) {
					_rRecoHitsCount.push_back(0);
					_rRecofWSelfPurity.push_back(0);
					_rRecofWRealPurity.push_back(0);
					_rRecoMOPSelfPurity.push_back(0);
					_rRecoMOPRealPurity.push_back(0);
					_rRecoUVHitsCount.push_back(0);
					_rRecoTAliasCount.push_back(0);
					_rRecoTOverkillCount[0].push_back(0);
					_rRecoTOverkillCount[1].push_back(0);
					_rRecoTOverkillCount[2].push_back(0);
					_rRecoTOverkillCount[3].push_back(0);
					_rRecoTOverkillAvg.push_back(0);
				} // Multiplicity LOOP.

			} // Hits LOOP.
	
		} // Pixel Size LOOP.
	
	} // Size LOOP.

}

//______________________________________________________________________________
void Reco::Show()
{
	/*!	Show analysis results. */

	// Unit header.
	std::cout << COL(LCYAN);
	std::cout << "\nReconstruction summary\n";
	std::cout << "----------------------\n\n";
	std::cout << COL(DEFAULT);

	// Stepping.
	unsigned int step = 0;
	
	// Loop over the tested sizes (N).
	for (unsigned int sizeIdx = 0; sizeIdx < _MA._Sim.sizeCount; sizeIdx++) {

		// Loop over the tested pixel sizes.
		for (unsigned int pxszIdx = 0; pxszIdx < _MA._Sim.pxszCount; pxszIdx++) {
				
			// ROOT stuff
			// --------------------------------------
				
				// Create the purity summary GRAPH.
				TGraph* grpPurity = new TGraph();
	
				// Creates the forms where to show the graphs.
				_MA._Canvas.push_back(new TCanvas("Reco_1", "Reconstruction purity", 100, 100, 800, 600));  
			
				// Creates the main pads. padA is for the secondary axis.
				TPad *padA = new TPad("padA", "", 0.0, 0.0, 1.0, 1.0);
				padA->SetFillStyle(4000); // Transparent.
				padA->Draw();	   
				TPad* padG = new TPad("padG", "", 0, 0.1, 1, 1);
				padG->SetFillStyle(4000); // Transparent.
				padG->SetGrid(1, 1);
				padG->Draw();
				padG->cd();
						

			// Show current configuration info.
			std::cout	<< "\n" << COL(DEFAULT) << "Reco:\n"
						<< " Pixel size = " << COL(LGREEN) << _rPxWidth[step] << COL(DEFAULT)
						<< " x " << COL(LGREEN) << _rPxHeight[step] << COL(DEFAULT) << " [mm]\n"
						<< " Matrix N = " << COL(CYAN) << _MA._Sim.sizeList[sizeIdx] << "\n" << COL(DEFAULT) 
						<< " Frame area = " << COL(LGREEN) << _rFrameArea[step] << COL(DEFAULT) << "[mm2]\n";  
			
			
			// Set spacing for table.
			int colWidth = 12;
			std::setw(colWidth);
			
			// Write table labels for hits density.
			std::cout << "\nhits/mm2 \t" << COL(LRED) << std::setprecision(4);	
			for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {	
				std::cout << std::setw(colWidth) << _rHitsDensity[step + hitsIdx];// << "\t";
			}
			std::cout << COL(DEFAULT) << "\n";	
						
			// Write table labels for hits density in hits/frame units.
			std::cout << "hits/frame \t" << COL(LRED) << std::setprecision(4);	
			for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {	
				std::cout << std::setw(colWidth) << _rFrameHitsAvg[step + hitsIdx];// << "\t";
			}
			std::cout << COL(DEFAULT) << "\n";	
			
			// Write table labels for hits density per coordinate in hits/frame units.
			std::cout << "hits/coord \t" << COL(YELLOW) << std::setprecision(4);	
			for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {	
				//std::cout << _rCoordHitsAvg[step + hitsIdx]	/ _MA._Sim.MCRuns << "\t";		
				std::cout << std::setw(colWidth) << _rCoordHitsAvg[step + hitsIdx]; // << "\t";
			}
			std::cout << COL(DEFAULT) << "\n";	
			
			// Write table labels for aliases into the hitlist before independence check.
			std::cout << "al/hitlist \t" << COL(YELLOW) << std::setprecision(4);	
			for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {	
				std::cout << std::setw(colWidth) << _rRecoTAliasCount[step + hitsIdx] / _MA._Sim.MCRuns - _rFrameHitsAvg[step + hitsIdx];// << "\t";
			}
			std::cout << COL(DEFAULT) << "\n";	
						
			// Write table labels for coordinates being hit more than once.
			for (unsigned int p = 0; p < 4; p++) {
				std::cout << "CR ovkl (" << COL(CYAN) << p << COL(DEFAULT) << ") \t" << COL(CYAN) << std::setprecision(4);	
				for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {	
					std::cout << std::setw(colWidth) << _rCoordOverkillCount[p][step + hitsIdx] / _MA._Sim.MCRuns; // << "\t";
				}
				std::cout << COL(DEFAULT) << "\n";
			}
			
			// Write table labels for coordinates being used more than once.
			for (unsigned int p = 0; p < 4; p++) {
				std::cout << "UT ovkl (" << COL(BLUE) << p << COL(DEFAULT) << ") \t" << COL(BLUE) << std::setprecision(4);	
				for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {	
					std::cout << std::setw(colWidth) << _rRecoTOverkillCount[p][step + hitsIdx] / _MA._Sim.MCRuns; // << "\t";
				}
				std::cout << COL(DEFAULT) << "\n";
			}
			
			// RECO Tetra	
			if (_MA._Ana.flagUV) {
			
				// Tetra good frames.
				std::cout << "T (" << COL(LCYAN) << "4" << COL(DEFAULT) << ") Frames\t" << COL(PURPLE) << std::setprecision(6);
				for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {
					std::cout << std::setw(colWidth) << 100.0 * (_rRecoMOPRealPurity[step + hitsIdx]) / _MA._Sim.MCRuns; // << "%\t";
				}
				std::cout << "\n" << COL(DEFAULT);
	  
				// Tetra-Hits / Tot-Hits.
				std::cout << "T (" << COL(LCYAN) << "4" << COL(DEFAULT) << ") safe\t" << COL(PURPLE);
				for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {
					std::cout << std::setw(colWidth) << std::setprecision(6) << 100.0 * (_rRecoUVHitsCount[step + hitsIdx]) / _rRecoHitsCount[step + hitsIdx]; // << "%\t";
			
					// Add the point to the purity graph.
					grpPurity->SetPoint(hitsIdx, _rHitsDensity[step + hitsIdx], 100.0 * (_rRecoUVHitsCount[step + hitsIdx]) / _rRecoHitsCount[step + hitsIdx]);
				}
  				std::cout << "\n" << COL(DEFAULT);

				// (Tot-Hits - Tetra-Hits) / Tot-Hits.
				std::cout << "T (" << COL(LCYAN) << "4" << COL(DEFAULT) << ") weak\t" << COL(PURPLE);
				for (unsigned int hitsIdx = 0; hitsIdx < _MA._Sim.hitsCount; hitsIdx++) {
					std::cout << std::setw(colWidth) << std::setprecision(6) << 1.0 * (_rRecoHitsCount[step + hitsIdx] - _rRecoUVHitsCount[step + hitsIdx]) / _rRecoHitsCount[step + hitsIdx]; // << "%\t";

					// Add the point to the purity graph.
					//grpPurity->SetPoint(hitsIdx, _rHitsDensity[step + hitsIdx], 100.0 * static_cast<double>(_rRecoUVHitsCount[step + hitsIdx]) / _rRecoHitsCount[step + hitsIdx]);
				}
				std::cout << "\n" << COL(DEFAULT);


			} // Tetra IF
						
			// Complete the GRAPHs.
			if (grpPurity) {
		
				// Set the final title on the graph
				std::stringstream title("");
				title << "Reco N = " << _rSizeColsN[step] << ", Px size = "
						<< _rPxWidth[step] << " x " << _rPxHeight[step]
						<< " [mm], Area = " << _rFrameArea[step] << " [mm2]";
				
				// Set Graph.
				grpPurity->SetTitle(title.str().c_str());
				grpPurity->GetYaxis()->SetRangeUser(0, 100);

				// Actually draws the graph (only the first one with axis and labels...).
				grpPurity->GetXaxis()->SetTitle("Hits density [hits/mm2]");
				grpPurity->GetYaxis()->SetTitle("Purity [%]");
				grpPurity->GetXaxis()->SetLimits(_rHitsDensity[step + 0], _rHitsDensity[step + _MA._Sim.hitsCount - 1]);
				grpPurity->Draw("ALP");
			
				// Draw the hits/frame additional axis.
				//_MA._Form.back()->Canvas()->cd();
				padA->cd();
				padA->Range(-0.125, 0, 1.125, 1);
				TGaxis* axis = new TGaxis(0.0, 0.1, 1, 0.1, _rFrameHitsAvg[0], _rFrameHitsAvg[_MA._Sim.hitsCount - 1], 10, "");
				axis->SetLabelSize(0.03f);
				axis->SetLabelColor(kBlue);
				axis->SetLabelFont(2);
				axis->SetTitleColor(kBlue);
				axis->SetTitleSize(0.03f);
				axis->SetTitle("[hits/frame]");
				axis->Draw("LP");

			} // grpPurity.size() IF
	
			// Increase the general step.
			step += _MA._Sim.hitsCount;
			std::cout << "\n";
						
		} // sizeIdx LOOP	
				
	} // pxszIdx LOOP
}


// *****************************************************************************
// **						fW Encoding/Decoding algorithms					  **
// *****************************************************************************

//______________________________________________________________________________
void Reco::WEncode (std::vector<long double>& imgW,
			  const unsigned int& N, const unsigned int& m,
			  const std::vector<unsigned int>& HitIdx)
{
	/*! Encodes the image of fW and store it into imgW using the standard bitMask
	 *	encoding for the its number (up to 64 different pixels encoding). The
	 *	matrix side size \c N and the fW multiplicity \c m must be provided.
	 *	\c HitIdx contains the pixel lists with absolute indexing (0...N^2-1).
	 * 
	 *  To speed up, the map is calculated and stored every time the size or m 
	 *  parameters change, and then used as a simple remapping. If \c display is
	 *	set true also prints the f map itself.
	 */

	// Maps storage.
	static std::vector<unsigned int*> fMap;
	static unsigned int fSide = 0;
	
	// Reset everything if N changes.
	if (N != fSide) {
		for (size_t i = 0; i < fMap.size(); i++) delete[] fMap[i];
		fMap.clear();
		fSide = N;
	}
	
	// Add maps if m bigger than the biggest used yet.
	if (m >= fMap.size()) {
		for (size_t i = fMap.size(); i <= m; i++) fMap.push_back(0);	
	
		// Info.
		if (_MA._Sim.flagDebug) std::cout << "fW Map expanded to hold N = " << N << " and m = " << m << "\n";
	}
		
	// If this m has not mapped yet, map it!	
	if (fMap[m] == 0) {

		// Assigns the map space.
		fMap[m] = new unsigned int[N * N];
		
		// Calculates the map.
		for (unsigned int y = 0; y < N; y++) {		
			for (unsigned int x = 0; x < N; x++) {		
				
				// The y / m is an integer division here, it's truncated!
				fMap[m][y * N + x] = (y % m) * N + (x + (y / m) * m) % N;
			}
		}

		// Info.
		if (_MA._Sim.flagDebug) std::cout << "fW Map calculated for N = " << N << " and m = " << m << "\n";
	
		// Debug Maps printout.
		if (_MA._Sim.flagDebugMaps) {
			for (int y = N - 1; y >= 0; y--) {
				for (unsigned int x = 0; x < N; x++) {		
					std::cout << fMap[m][y * N + x] << " ";
				}
				std::cout << "\n";
			}
		}
	} // End generating a new map.

  	// Reset previous image.
	imgW.resize(N * m);
	for (unsigned int i = 0; i < N * m; i++) imgW[i] = 0;
	
	// Apply the map with bitMask encoding!
	for (size_t i = 0; i < HitIdx.size(); i++) {
		//std::cout << i << ", " << HitIdx[i] << "\n";
		imgW[fMap[m][HitIdx[i]]] += (1 << i);
	}
}

//______________________________________________________________________________
bool Reco::XYWDecode(std::vector<unsigned int>& hitIdxList,	
			   const std::vector<unsigned char>& prjX,
			   const std::vector<unsigned char>& prjY,
			   const std::vector<long double>& prjW)
{
	/*! Reconstruct a pixel list from two canonical projections (\c prjX \c prjY) 
	 *	and the image of a fW map.The matrix side size N and the fW multiplicity
	 *	are automatically derived from the sizes of the projections.
	 *	Reconstructed pixels are stored into the hitIdxList list, where they are
	 *	indexed with the canonical absolute addressing (x + y * N). If the
	 *	reconstruction was able to get rid of all the ambiguity, the result is
	 *	math-correct at the function return true, otherwise the returned list
	 *	must undergo further checking and the function returns false.
	 *	
	 *	At the moment this function is optimized for readability, not speed!
	 */

	// Derives N and m.
	unsigned int N = prjX.size();
	unsigned int m = prjW.size() / prjX.size();

	// Buildup a list of X and Y coordinates from the projections.
	std::vector<unsigned int> hitX;
	std::vector<unsigned int> hitY;
	for (unsigned int i = 0; i < N; i++) {
		if (prjX[i] > 0) hitX.push_back(i); 
		if (prjY[i] > 0) hitY.push_back(i);
	}
	
	// That's has to be check if really effective.
	std::vector<long double> hitW;
	for (unsigned int i = 0; i < N * m; i++) if (prjW[i] > 0) hitW.push_back(prjW[i]);

	// Determines the minimum number of necessary hits.
	unsigned int hitsMin = hitX.size();
	if (hitY.size() > hitsMin) hitsMin = hitY.size();
	if (hitW.size() > hitsMin) hitsMin = hitW.size();	

	// Here we store how many times a coordinate (X,Y or W) projection is used 
	// by a potential hit. Onetime used projections identifies the first order 
	// unanbiguous hits.
	int* prjCntX =	new int[N];
	int* prjCntY =	new int[N];
	int* prjCntW =	new int[N * m];	
	for (unsigned int i = 0; i < N; i++) prjCntY[i] = prjCntX[i] = 0;
	for (unsigned int i = 0; i < N * m; i++) prjCntW[i] = 0;

	// Recovered hits storage space.
	struct sHit {unsigned int x; unsigned int y; unsigned int w;};
	std::vector<sHit> hitList;

	// Loops all over the x and y potential hits; add the pair to the hit-list 
	// only if they matches a checked fW image. In that case, also increases
	// the involved projections counters.
	sHit hit;
	for (unsigned int y = 0; y < hitY.size(); y++) {
		for (unsigned int x = 0; x < hitX.size(); x++) {
			
			// Get coordinates.
			hit.x = hitX[x];
			hit.y = hitY[y];
			
			// Calculates the correspondent w projections.
			hit.w = (hit.y % m) * N + (hit.x + (hit.y / m) * m) % N;
	
			// If there is a matching record, store the hit.
			if (prjW[hit.w] > 0) {
				hitList.push_back(hit);
				prjCntX[hit.x] ++;
				prjCntY[hit.y] ++;
				prjCntW[hit.w] ++;
			}
		}
	}

	// Debug.
	if (_MA._Sim.flagDebug) {	
		std::cout << "\nXYW potential hit list (x,y,w):\n";
		for (size_t i = 0; i < hitList.size(); i++) {
			std::cout << hitList[i].x << ", " << hitList[i].y << ", " << hitList[i].w 
					  << "  [" << hitList[i].y * N + hitList[i].x << "]\n";
		}
	}

	// Among the possible hits, retains only those which are unambiguous hits.
	hitIdxList.clear();
	for (size_t i = 0; i < hitList.size(); i++) {
		if (prjCntX[hitList[i].x] == 1	||	
			prjCntY[hitList[i].y] == 1	||
			prjCntW[hitList[i].w] == 1) {
			hitIdxList.push_back(hitList[i].y * N + hitList[i].x); 
		}
	}

	// Releases projections counters.
	delete[] prjCntX;
	delete[] prjCntY;
	delete[] prjCntW;

	// Returns. The reconstruction is safe if the number of unambiguous hits
	// is bigger or equal the number of minimal expected hits.
	return hitIdxList.size() >= hitsMin;
}


// *****************************************************************************
// **					  Tetra Encoding/Decoding algorithms				  **
// *****************************************************************************

//______________________________________________________________________________
void Reco::TSetup (const unsigned int& N, const unsigned int& m)
{
	/*! Encodes the Mutually Orthogonal Projections (XYUV) images and store 
	 *	them into _XYtoUV. Store also the inverse map _UVtoXY. The matrix side 
	 *	size \c N and the dimensional multiplicity \c m must be provided.
	 */

	// Debug.
	if (_MA._Sim.flagDebug) std::cout << COL(LRED) << " T-Setup" << COL(DEFAULT) << "...\n";

	// Save the real layer side size (used to reconvert from reco XYUV maps
	// coordinate to real layer coordinates).
	_TLayerN = N;

	// Force the side size of the maps to be odd.
	if (N % 2) _TN = static_cast<unsigned short>(N);
	else _TN = static_cast<unsigned short>(N + 1);
	
	// Create digital map matrix
	_TdigiMap = new unsigned short[_TN * _TN];
	
	// Assigns the maps space.
	if (_MA._Sim.flagDebug) std::cout << " Allocating maps...\n";
	_XYtoUV[0] = new unsigned short[_TN * _TN];
	_XYtoUV[1] = new unsigned short[_TN * _TN];
	_UVtoXY[0] = new unsigned short[_TN * _TN];
	_UVtoXY[1] = new unsigned short[_TN * _TN];
		
	// Resizes the T (XYUV) projections space.
	if (_MA._Sim.flagDebug) std::cout << " Allocating projections...\n";
	for (unsigned int i = 0; i < 4; i++) {
		_TProj[i] = new unsigned short[_TN];
	}

	// Resize the use table.
	if (_TUseTable) delete[] _TUseTable;
	_TUseTable = new unsigned short[4 * _TN];
	
	// Calculates the maps. The map size is actually 2*N, where the
	// first and second halves are used to store the first and second
	// coordinate respectively.
	if (_MA._Sim.flagDebug) std::cout << " Building new maps...\n";
	unsigned short u, v;
	for (unsigned short y = 0; y < _TN; y++) {		
		for (unsigned short x = 0; x < _TN; x++) {		
			
			// Direct maps, XY->UV
			_XYtoUV[0][y * _TN + x] = u = (x + y) % _TN;
			_XYtoUV[1][y * _TN + x] = v = (x + y * (_TN - 1)) % _TN;

			// Reverse maps, UV->XY
			_UVtoXY[0][v * _TN + u] = x;
			_UVtoXY[1][v * _TN + u] = y;
				
		}
	}

	// Debug printout.
	if (_MA._Sim.flagDebugMaps) {
		std::cout << std::hex;
		for (unsigned int d = 0; d < 2; d++) {
			std::cout << "\nXY->UV map calculated for N = " << _TN << " dimension # " << d << "\n";
			for (int y = _TN - 1; y >= 0; y--) {
				for (unsigned int x = 0; x < _TN; x++) {		
					std::cout << std::setw(2) << _XYtoUV[d][y * _TN + x] << " ";
				}
				std::cout << "\n";
			}
		}
		for (unsigned int d = 0; d < 2; d++) {
			std::cout << "\nUV->XY map calculated for N = " << _TN << " dimension # " << d << "\n";
			for (int v = _TN - 1; v >= 0; v--) {
				for (unsigned int u = 0; u < _TN; u++) {		
					std::cout << std::setw(2) << _UVtoXY[d][v * _TN + u] << " ";
				}
				std::cout << "\n";
			}
		}
		std::cout << std::dec;
	}
	if (_MA._Sim.flagDebug) std::cout << " TSetup completed.\n";
}

//______________________________________________________________________________
void Reco::TClear ()
{
	/* Clear all the data structure used by the Tetra (XYUV) encoding. */
	
	// Tetra (XYUV) maps.
	delete[] _XYtoUV[0];
	delete[] _XYtoUV[1];
	delete[] _UVtoXY[0];
	delete[] _UVtoXY[1];
	_XYtoUV[0] = 0;
	_XYtoUV[1] = 0;
	_UVtoXY[0] = 0;
	_UVtoXY[1] = 0;
	
	// Tetra (XYUV) projections.
	for (unsigned int i = 0; i < 4; i++) {
		delete[] _TProj[i];
		_TProj[i] = 0;
	}
	
	// Tetra (XYUV) Use table.
	delete[] _TUseTable;
	_TUseTable = 0;	

	delete[] _TdigiMap;
}

//______________________________________________________________________________
bool Reco::DParse(unsigned short* outPxMap, 
			const unsigned short* inPxMap, 
			const unsigned int& pxCols, 
			const unsigned int& pxRows)
{
	/*! Applies the derivative pixel encoding to a pxMap. */

	// Debug.
	if (_MA._Sim.flagDebug) std::cout << COL(LRED) << " D-Parsing" << COL(DEFAULT) << "...\n";

	// Comparators emulations.
	//
	//				  Top
	//				   |
	//			/-------------\
	//			|			  |
	//			|			  |
	//	Left ---|	 Main	  |
	//			|			  |
	//			|			  |
	//			\-------------/
	//
	struct pxCmp {bool Main; bool Left; bool Top;};
	pxCmp* cmpMap = new pxCmp[pxCols * pxRows];

	// Retrieve digi parameters.
	int pxThr = _MA._Ana.tetraParam[0];		// Pixel main threshold.
	//int pxRange = _MA._Ana.tetraParam[1];
	//int pxBits = _MA._Ana.tetraParam[2];
	
	// Fills the comparators by parsing pixels value.
	for (unsigned int y = 1; y < pxRows; y++) {
		for (unsigned int x = 1; x < pxCols; x++) {

			// Pivots.
			unsigned short thisPix = inPxMap[y * pxCols + x];
			pxCmp thisCmp =	cmpMap[y * pxCols + x];

			// Main.
			(thisPix > pxThr) ? thisCmp.Main = true : thisCmp.Main = false; 

			// Left.
			(thisPix > inPxMap[y * pxCols + x - 1]) ? thisCmp.Left = true : thisCmp.Left = false; 

			// Top.
			(thisPix > inPxMap[(y - 1) * pxCols + x]) ? thisCmp.Top = true : thisCmp.Top = false; 
		}
	}	

	// Apply comparators logic.
	for (unsigned int y = 0; y < pxRows - 1; y++) {
		for (unsigned int x = 0; x < pxCols - 1; x++) {

			// Pivot.
			pxCmp thisCmp =	cmpMap[y * pxCols + x];

			// Assign zero to the ut map.
			outPxMap[y * pxCols + x] = 0;
			
			// Comparators gluing.
			if (thisCmp.Main == true) {
				if (thisCmp.Left == true && thisCmp.Top == true) {
					if (cmpMap[y * pxCols + x + 1].Left == false && cmpMap[(y - 1) * pxCols + x].Top == false) {
						outPxMap[y * pxCols + x] = pxThr + 100;
					}
				}
			}
		}
	}	
	
	// Clear comparators.
	delete[] cmpMap;
		
	// Well done!
	return false;
}

//______________________________________________________________________________
void Reco::TEncode (unsigned short** proj, 
					const unsigned short* pxMap, 
					const unsigned int& pxCols, 
					const unsigned int& pxRows)
{
	/*! Encodes the Tetra mutually orthogonal projections (XYUV) images and 
	 *	store them into _TProj. This overload starts from an arbitrary pxmap
	 *	where every pixel different from 0 is considered a potential signal.
	 */
		
	// Debug.
	if (_MA._Sim.flagDebug) std::cout << COL(LRED) << " T-Encoding" << COL(DEFAULT) << "...\n";
	
	// Clear previous projections.
	for (unsigned short i = 0; i < _TN; i++) {
		proj[0][i] = 0;	// X.
		proj[1][i] = 0;	// Y.
		proj[2][i] = 0;	// U.
		proj[3][i] = 0;	// V.
	}
	
	// Create debug map.
	TH2I* digiMap = 0;
	if (_MA._Sim.flagDebug) digiMap = new TH2I("", "Digi map", _TN-1, 0, _TN-1, _TN-1, 0, _TN-1);

	// Projects the hits!
	unsigned int pxCount = pxCols * pxRows;
	unsigned int oIdx = 0;
	
	// Retrieve digi parameters.
	int pxThr = _MA._Ana.tetraParam[0];
	int pxRange = _MA._Ana.tetraParam[1];
	int pxBits = _MA._Ana.tetraParam[2];
	int pxSteps = static_cast<int>(pow(2.0, pxBits));
	int dVal = 0;
	int pxDigi = 0;

	// Scan the full map.
	for (unsigned int y = 0; y < pxRows; y++) {
		for (unsigned int x = 0; x < pxCols; x++) {
		
			// Get the digi value.
			dVal = (-pxThr + pxMap[y * pxCols + x]);
			
			// Increases projections if over threshold.		
			if (dVal > 0) {
				oIdx = y * _TN + x;
				proj[0][x] += dVal;	// X.
				proj[1][y] += dVal;	// Y.
				proj[2][_XYtoUV[0][oIdx]] += dVal;	// U.
				proj[3][_XYtoUV[1][oIdx]] += dVal;	// V.
			};

			// Creates the visual map.
			pxDigi = 0;
			if (dVal > 0) {
				if (dVal > pxRange) dVal = pxRange;
				if (pxSteps > 1) pxDigi = static_cast<int>(floor(0.5 + static_cast<float>(dVal) / pxRange * pxSteps));
				else pxDigi = 1;
			}	
			_TdigiMap[y * pxCols + x] = pxDigi;
			if (_MA._Sim.flagDebug && pxDigi > 0) digiMap->Fill(x, y, pxDigi);
		}
	}

	// "Digitize" the projections by signals summation.
	for (unsigned int p = 0; p < 4; p++) {
		for (unsigned int i = 0; i < _TN; i++) {
			if (proj[p][i] >= pxRange) {
				//proj[p][i] = pxRange;
			} else {
				//proj[p][i] = static_cast<int>(floor(0.5 + static_cast<float>(proj[p][i]) / pxRange * pxSteps));
			}
		}
	}

	// STAT: saves how many more than one hits per each coordinate.
	for (unsigned int i = 0; i < _TN; i++) {
		if (proj[0][i] > 1) _rCoordOverkillCount[0][_rStepCurr]++; 
		if (proj[1][i] > 1) _rCoordOverkillCount[1][_rStepCurr]++; 
		if (proj[2][i] > 1) _rCoordOverkillCount[2][_rStepCurr]++; 
		if (proj[3][i] > 1) _rCoordOverkillCount[3][_rStepCurr]++; 
	}

	// Debug showing the digi map.
	if (_MA._Sim.flagDebug) {
		
		// Build and show the window.
		_MA._Canvas.push_back(new TCanvas("Reco_2", "Digi map", 40, 40, 600, 600));  
		TPad* pad_Out = new TPad("", "Digi map", 0.0, 0.0, 1.0, 1.0);
				
		// Plot pxMaps.
		pad_Out->GetCanvas()->cd();
		pad_Out->GetCanvas()->SetGrid(1,1);
		digiMap->GetXaxis()->SetNdivisions(8, _TN / 8, 0, false); 
		digiMap->GetYaxis()->SetNdivisions(8, _TN / 8, 0, false);
		digiMap->Draw("colz");
	}
	
}

//______________________________________________________________________________
bool Reco::TDecode(std::vector<unsigned int>& hitIdxList, unsigned short const* const* proj)
{
	/*! Reconstruct a pixel list from the Tetra projections (\c prjX \c prjY 
	 *	\c prjU and \c prjV). This basic version does not consider any pixel
	 *	cluster in reconstruction.
	 *	
	 *	At the moment this function is optimized for readability, not speed!
	 */

	// Debug.
	if (_MA._Sim.flagDebug) std::cout << COL(LRED) << " T-Decoding" << COL(DEFAULT)<< "...\n";

	// Clean the Usage Table.
	for (int i = 0; i < 4 * _TN; i++) _TUseTable[i] = 0;

	// Buildup a list of X and Y coordinates from the projections.
	std::vector<std::vector<unsigned int> > hitXY;
	std::vector<unsigned int> hitX;
	std::vector<unsigned int> hitY;
	for (unsigned int i = 0; i < _TN; i++) {
		if (proj[0][i] > 0) hitX.push_back(i); 
		if (proj[1][i] > 0) hitY.push_back(i);
	}

	// Update stat on coordinate use (just 2 coord at the time, not 4).
	_rCoordHitsAvg[_rStepCurr] += 0.5 * hitX.size();
	_rCoordHitsAvg[_rStepCurr] += 0.5 * hitY.size();
				
	// Stores the X and Y list into a single list.
	hitXY.push_back(hitX); hitX.clear();	
	hitXY.push_back(hitY); hitY.clear();	

	// Creates the candidate lists.
	struct hit {unsigned int x; unsigned int y;
				unsigned int u; unsigned int v;};
	hit tmpHit;
	
	// Reconstruct all the possible XY hits.
	std::vector<hit> hitList;
	for (unsigned int xId = 0; xId < hitXY[0].size(); xId++) {
		for (unsigned int yId = 0; yId < hitXY[1].size(); yId++) {
			
			// Check if there is a correspondig UV
			unsigned int mIdx = hitXY[1][yId] * _TN + hitXY[0][xId];
			if (proj[2][_XYtoUV[0][mIdx]] > 0 && proj[3][_XYtoUV[1][mIdx]] > 0 ) {
			
				// Add the hit to the temporary list.	
				tmpHit.x = hitXY[0][xId];
				tmpHit.y = hitXY[1][yId];
				tmpHit.u = _XYtoUV[0][mIdx];
				tmpHit.v = _XYtoUV[1][mIdx];
				hitList.push_back(tmpHit);

				// Update the usage table.
				_TUseTable[0 * _TN + tmpHit.x]++;
				_TUseTable[1 * _TN + tmpHit.y]++;
				_TUseTable[2 * _TN + tmpHit.u]++;
				_TUseTable[3 * _TN + tmpHit.v]++;
			}
		}
	}

	// Debug moment: projection and usage table.
	if (_MA._Sim.flagDebug) {
		
		// Create the histos.
		TH2I* mPrj = new TH2I("", "Projections", _TN, 0, _TN, 4, 0, 4);
		TH2I* mUse = new TH2I("", "Usage map", _TN, 0, _TN, 4, 0, 4);

		// Fill with projections and usage.
		for (unsigned int d = 0; d < 4; d++) {
			for (unsigned int i = 0; i < _TN; i++) {
				mPrj->Fill(i, d, proj[d][i]);
				mUse->Fill(i, d, _TUseTable[d * _TN + i]);
			}
		}		
		
		// Build and show the window.
		_MA._Canvas.push_back(new TCanvas("Reco_3", "Reco inner maps", 60,60, 600, 600));  
		TPad *pad_Out = new TPad("", "Reco inner maps", 0.0, 0.0, 1.0, 1.0);
		pad_Out->GetCanvas()->SetFillColor(kWhite);
		pad_Out->GetCanvas()->Divide(1, 2);	
		pad_Out->GetCanvas()->cd(1);
		mPrj->Draw("colz");
		pad_Out->GetCanvas()->cd(2);
		mUse->Draw("colz");
	}

	// Update stat on how many Aliases get into the hitlist before independence check.
	_rRecoTAliasCount[_rStepCurr] += hitList.size();

	// Update stat on average over-usage (bins with more than one hit)
	// of the usage-table. Does it for each coordinate.
	for (int p = 0; p < 4; p++) {
		for (int i = 0; i < _TN; i++) {
			if (_TUseTable[p * _TN + i] > 1) { 
				_rRecoTOverkillCount[p][_rStepCurr] += 1;
			}
		}
	}

	// Debug (Final Hitlist).
	if (_MA._Sim.flagDebug) std::cout << "   Final hitlist (x,y,u,v) [idx]\n";
		
	// From the candidates list, confirm only those hits with AT LEAST ONE
	// coordinate which has been used just one time.
	hitIdxList.clear();
	for (unsigned int i = 0; i < hitList.size(); i++) {
		
		// Check if the hit is worth
		if (_TUseTable[0 * _TN + hitList[i].x] == 1 ||
			_TUseTable[1 * _TN + hitList[i].y] == 1 ||
			_TUseTable[2 * _TN + hitList[i].u] == 1 ||
			_TUseTable[3 * _TN + hitList[i].v] == 1 ) 
		{
			// Add it to the final hit list.
			hitIdxList.push_back(hitList[i].y * _TLayerN + hitList[i].x); 
		
			// Debug
			std::cout << COL(PURPLE);
		}

		// Debug.
		if (_MA._Sim.flagDebug) {
			std::cout	<< " (" << hitList[i].x << ", " << hitList[i].y
						<< ", " << hitList[i].u << ", " << hitList[i].v 
						<< ") [" << hitList[i].y * _TLayerN + hitList[i].x 
						<< "]\n" << COL(DEFAULT);
		}	
	}

	// Debug.
	if (_MA._Sim.flagDebug) {
		std::cout << std::dec << "\n";
		std::cout << "T-Found: " << hitIdxList.size() << "\n";
	}

	// Returns. The reconstruction is safe if the number of unambiguous hits
	// is bigger or equal the number of minimal expected hits.
	_rRecoUVHitsCount[_rStepCurr] += hitIdxList.size();
	return hitIdxList.size() >= 0;
}


//______________________________________________________________________________
unsigned int Reco::TDecodeClx(std::vector<clObj>& clReco, unsigned short const* const* proj, pas::obj::Layer* ly)
{
	/*! Reconstruct a pixel list from the Tetra projections (\c prjX \c prjY 
	 *	\c prjU and \c prjV). This version handles cluster of pixels as single
	 *	hits.
	 *	
	 *	At the moment this function is optimized for readability, not speed!
	*/

	// Debug.
	if (_MA._Sim.flagDebug) std::cout << COL(LRED) << " T-Decoding-Clx" << COL(DEFAULT)<< "...\n";

	
	// Step 0: Allocate 4 projections to store the aggregate Cluster Id (ACId),
	// plus the very same projection for cluster "personal" needs while bulding.
	unsigned int* clProjId[4];
	unsigned int* clPersId[4];
	for (unsigned int p = 0; p < 4; p++) {
		clProjId[p] = new unsigned int[_TN];
		clPersId[p] = new unsigned int[_TN];
	}

	// 1st Step: Creates 4 list of culster projections 
	// belonging to each one of the four projections.
	// ------------------------------------------------
	std::vector<clPrjObj> clProj[4];
	
	// Build up the per-coordinate clusters lists.
	for (unsigned int p = 0; p < 4; p++) {
		bool clIn = false;
		for (unsigned int i = 0; i < _TN; i++) {
			clProjId[p][i] = 0;
			if (proj[p][i] > 0) {
				
				// Continue current cluster.
				if (clIn) {
					clProj[p].back()._Size ++; 
					clProj[p].back()._Stop ++;
					//clProjId[p][i] = clProj[p].size();	

				// Starts a new cluster.	
				} else {
					clProj[p].push_back(clPrjObj(i, i, 1, 0));
					//clProjId[p][i] = clProj[p].size();
					clIn = true;
				}	
			} else {
				clIn = false;
			}
		}
	}

	// Prepares debug pxMap.
	TH2I* pxMap[3] = {0, 0, 0};
	if (_MA._Sim.flagDebug) {
		pxMap[0] = new TH2I("", "Raw de-projections", _TN-1, 0, _TN-1, _TN-1, 0, _TN-1);
		pxMap[1] = new TH2I("", "Confirmed pixels", _TN-1, 0, _TN-1, _TN-1, 0, _TN-1);
		pxMap[2] = new TH2I("", "Confirmed clusters", _TN-1, 0, _TN-1, _TN-1, 0, _TN-1);
		//pxMap[3] = new TH2I("", "Encoded digitez map", _TN-1, 0, _TN-1, _TN-1, 0, _TN-1);
	}	
	
	// 2st Step: build a first tentative pxList by eliminating
	// pixels not correctly tagged by the projections. Groups
	// the surviving pixels by cluster derived by the X and Y
	// projections.	
	// ---------------------------------------------------------
	std::vector<pxObj> pxList;
	std::vector<clObj> clList;
	
	// For all cluster combination generated from the X and Y projections.
	// Map present cluster situation for debug.
	for (unsigned int xId = 0; xId < clProj[0].size(); xId++) {
		for (unsigned int yId = 0; yId < clProj[1].size(); yId++) {
			
			// Clear cluster "personal projections".
			for (unsigned int i = 0; i < _TN; i++) {
				clPersId[3][i] = clPersId[2][i] = clPersId[1][i] = clPersId[0][i] = 0;
			}
			bool clStarted = false;

			// For all pixels belonging to the two cluster combination.
			for (unsigned int x = clProj[0][xId]._Start; x <= clProj[0][xId]._Stop; x++) {
				for (unsigned int y = clProj[1][yId]._Start; y <= clProj[1][yId]._Stop; y++) {

					// debug map.
					if (_MA._Sim.flagDebug) pxMap[0]->Fill(x, y); 
						
					// Retrieve u and v coordinates.
					unsigned int mIdx = y * _TN + x;
					unsigned int u = _XYtoUV[0][mIdx];
					unsigned int v = _XYtoUV[1][mIdx];
					
					// Check if there is a correspondig UV projection.
					if (proj[2][u] > 0 && proj[3][v] > 0 ) {
			
						// Add the pixel to the current cluster.	
						if (clStarted) {
							clList.back()._pxList.push_back(pxObj(x, y, u, v));
						} else {	
							clList.push_back(clObj(pxObj(x, y, u, v)));
							clStarted = true;
						}

						// Increase the Projection Usage Table.
						clPersId[0][x] = 1;
						clPersId[1][y] = 1;
						clPersId[2][u] = 1;
						clPersId[3][v] = 1;
					}
				}
			}

			// If a cluster was found, add its "personal" projection to the gobal ones.
			if (clStarted) {
				for (unsigned int p = 0; p < 4; p++) {
					for (unsigned int i = 0; i < _TN; i++) {
						clProjId[p][i] += clPersId[p][i];
					}
				}	
			}
		}
	}

	// Update stat on how many Aliases get into the hitlist before independence check.
	_rRecoTAliasCount[_rStepCurr] += clList.size();

	// Map present cluster situation for debug.
	if (_MA._Sim.flagDebug) {
		for (unsigned int c = 0; c < clList.size(); c++) {
			for (unsigned int p = 0; p < clList[c]._pxList.size(); p++) {
				pxMap[0]->Fill(clList[c]._pxList[p]._X, clList[c]._pxList[p]._Y); 
				pxMap[1]->Fill(clList[c]._pxList[p]._X, clList[c]._pxList[p]._Y); 
			}
		}
	}

	// 3rd Step: remove all those cluster which have no
	// one single pixel hitting a single usage coordinate.
	// ---------------------------------------------------
	unsigned int kills = 0;
	for (unsigned int c = 0; c < clList.size(); c++) {
		
		// Check if there is at least one safe pixel in the cluster.
		bool clGood = false;
		for (unsigned int i = 0; i < clList[c]._pxList.size(); i++) {
			pxObj* px = &(clList[c]._pxList[i]);
			if (clProjId[0][px->_X] == 1 || clProjId[1][px->_Y] == 1 ||
				clProjId[2][px->_U] == 1 || clProjId[3][px->_V] == 1)
			{
				clGood = true;
				break;	
			}
		}
		
		// Remove pixels from cluster in case.
		if (!clGood) {
			clList[c]._pxList.clear();
			kills++;
		}
	}

	
	// 4th Step: assembles the final cluster list.
	// -------------------------------------------
	clReco.clear();
	for (unsigned int c = 0; c < clList.size(); c++) {
		if (clList[c]._pxList.size() > 0) clReco.push_back(clList[c]); 
	}

	
	// Debug moment: Show clusters lists and projections.
	// --------------------------------------------------
	if (_MA._Sim.flagDebug) {
				
		// Add to the map the surviving clusters.
		for (unsigned int c = 0; c < clReco.size(); c++) {
			for (unsigned int p = 0; p < clReco[c]._pxList.size() ; p++) {
				pxMap[1]->Fill(clReco[c]._pxList[p]._X, clReco[c]._pxList[p]._Y); 
				pxMap[2]->Fill(clReco[c]._pxList[p]._X, clReco[c]._pxList[p]._Y); 
			}
		}

		// Clusters lists.
		//std::cout << " Cluster(s) Projections Lists\n";	
		//for (unsigned int p = 0; p < 4; p++) {
		//	std::cout << "   Proj [" << p << "]:\n";	
		//	for (unsigned int i = 0; i < clProj[p].size(); i++) {
		//		std::cout << "   cl [" << i + 1 << "] " << clProj[p][i]._Start << ", " 
		//				  << clProj[p][i]._Stop << ", " << clProj[p][i]._Size  << ", "
		//				  << clProj[p][i]._Used << "\n";				
		//	}	
		//}

		// Info.
		std::cout << " Surviving clusters: " << COL(RED) << clList.size() << COL(DEFAULT) << "\n";
		std::cout << " Killed clusters: " << COL(RED) << kills << COL(DEFAULT) << "\n";

		// Create the histos.
		TH2I* mPrj = new TH2I("", "Projections", _TN, 0, _TN, 4, 0, 4);
		TH2I* mUse = new TH2I("", "Proj Usage Table", _TN, 0, _TN, 4, 0, 4);
			
		// Fill with projections and usage.
		for (unsigned int p = 0; p < 4; p++) {
			for (unsigned int i = 0; i < _TN; i++) {
				mPrj->Fill(i, p, proj[p][i]);
				mUse->Fill(i, p, clProjId[p][i]);
			}
		}		
		
		// Build and show the window.
		_MA._Canvas.push_back(new TCanvas("Reco_4", "Reco inner maps", 40,40,600, 600));  
		TPad *pad_Out = new TPad("", "Reco inner maps", 0.0, 0.0, 1.0, 1.0);
		pad_Out->GetCanvas()->Divide(2, 3);	
		
		// Plot Projections.
		pad_Out->GetCanvas()->cd(1);
		pad_Out->GetCanvas()->GetPad(1)->SetGrid(1, 1);
		mPrj->GetXaxis()->SetNdivisions(8, _TN / 8, 0, true); 
		mPrj->GetYaxis()->SetNdivisions(8, _TN / 8, 0, true);
		mPrj->Draw("colz");
		
		// Plot Projections Usage Table.
		pad_Out->GetCanvas()->cd(2);
		pad_Out->GetCanvas()->GetPad(2)->SetGrid(1, 1);
		mUse->GetXaxis()->SetNdivisions(8, _TN / 8, 0, true); 
		mUse->GetYaxis()->SetNdivisions(8, _TN / 8, 0, true);
		mUse->Draw("colz");
		
		// Plot pxMaps.
		for (unsigned int i = 0; i < 3; i++) {
			pad_Out->GetCanvas()->cd(3 + i);
			pad_Out->GetCanvas()->GetPad(3 + i)->SetGrid();
			pxMap[i]->GetXaxis()->SetNdivisions(8, _TN / 8, 0, false); 
			pxMap[i]->GetYaxis()->SetNdivisions(8, _TN / 8, 0, false);
			pxMap[i]->Draw("colz");
		}
	}

	// 5th Step: free memory and remove pivots.
	// ----------------------------------------
	for (unsigned int p = 0; p < 4; p++) {
		delete[] clProjId[p];
		delete[] clPersId[p];
	}
	
	// 6th Step: returns the number of safe reco hits.
	return clReco.size();
}

// *****************************************************************************
// **							Support functions							  **
// *****************************************************************************

//______________________________________________________________________________
int Reco::compareIdxList(const std::vector<unsigned int>& listReal,
					const std::vector<unsigned int>& listReco)
{
	/*! Compares a Real and a Reco list and returns the number of matching
	 *	pairs (returns ZERO if more than the original hits are provided). 
	 *	Argument order IS important, the Real list must be the first. 
	 *	Superimposed hits are correctly handled.
	 */
	
	// Check size.
	if (listReco.size() > listReal.size()) return 0;
	int count = 0;

	// Check elements.
	size_t i, j;
	for (i = 0; i < listReal.size(); i++) {
		for (j = 0; j < listReco.size(); j++) {
			if (listReal[i] == listReco[j]) {
				count ++;
				break;
			}
		}
	}
	
	// Done.
	if (_MA._Sim.flagDebug) std::cout << "Verified: " << count << " hits\n";
	return count;
}

//______________________________________________________________________________
int Reco::compareClxList(const std::vector<unsigned int>& hitList,
						const std::vector<clObj>& clReco)
{
	/*! Compares a Real and a Reco Cluster list, and returns the number 
	 *	of matching real hits actually contained in the cluster list.
	 *	Returns ZERO if a fake (a cluster with no matching real hits)
	 *	is found.
	 *	Argument order IS important, the Real list must be the first. 
	 *	Superimposed hits are correctly handled.
	 */
	
	// Check size.
	//if (clReco.size() > hitList.size()) return 0;
	int count = 0;

	// Check elements.
	for (unsigned int h = 0; h < hitList.size(); h++) {
		unsigned int x = hitList[h] % _TLayerN;
		unsigned int y = static_cast<unsigned int>(hitList[h] / _TLayerN);
		for (unsigned int c = 0; c < clReco.size(); c++) {
			if (clReco[c].Belong(x, y)) {
				count ++;
				break; 
			}
		}
	}
	
	// Done.
	if (_MA._Sim.flagDebug) std::cout << " Verified: " << COL(LGREEN) << count << COL(DEFAULT) << " hits\n";
	return count;
}

//______________________________________________________________________________
void Reco::debugMap(const pas::obj::Layer& ly,
					const std::vector<unsigned int>& listReal,
					const std::vector<unsigned int>& listReco)
{
	/*! Show a debug map with highlighted the two list in different colors */

	// Get the layer pixel map first.
	TH2I* pxMap = ly.Dump(true); 
	TH2I* recoMap = new TH2I(*pxMap);
	pxMap->SetTitle("Raw layer pxMap"); 
	recoMap->SetTitle("Reco hits pxMap");

	// Clear a copy of the pxMap.
	recoMap->Reset("M");
	unsigned short N = ly.GetPxCols(); 

	// Fill the real hits.
	for (unsigned int i = 0; i < listReal.size(); i++) {
		recoMap->Fill(listReal[i] % N, listReal[i] / N, 5);	
	}	

	// OverFill the reco hits.
	for (unsigned int i = 0; i < listReco.size(); i++) {
		recoMap->Fill(listReco[i] % N, listReco[i] / N, 10);	
	}	
	
	// Executable objects enable defining different palettes in the same canvas.
	TExec *ex1 = new TExec("ex1","gStyle->SetPalette(1, 0);");
	TExec *ex2 = new TExec("ex2","gStyle->SetPalette(1, 0);");
 
	// Show the histo.
	_MA._Canvas.push_back(new TCanvas("Reco_5", "Reco Map", 100, 100, 600, 600));  
	_MA._Canvas.back()->Divide(2,1);
 	gStyle->SetOptStat(false);
	
	// Draw pxMap.
	_MA._Canvas.back()->GetPad(1)->cd();
	_MA._Canvas.back()->GetPad(1)->SetGrid(1, 1);
	pxMap->Draw("colz");
	pxMap->GetXaxis()->SetNdivisions(8, N / 8, 0, false); 
	pxMap->GetYaxis()->SetNdivisions(8, N / 8, 0, false);
	ex1->Draw();
	pxMap->Draw("colz same");
	
	// Draw recoMap.
	_MA._Canvas.back()->GetPad(2)->cd();
	_MA._Canvas.back()->GetPad(2)->SetGrid(1, 1);
	recoMap->Draw("colz");
	recoMap->GetXaxis()->SetNdivisions(8, N / 8, 0, false); 
	recoMap->GetYaxis()->SetNdivisions(8, N / 8, 0, false);
	ex2->Draw();
	recoMap->Draw("colz same");
}


// #############################################################################
}} // CLosing namespaces 