//------------------------------------------------------------------------------
// PAS - Layer Object														  --
// (C) Piero Giubilato 2011-2013, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasObjLayer.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"25 Sep 2012"
// [Language]		"C++"
//______________________________________________________________________________


// Root.
#include "TStyle.h"

// PAS general.
#include "pasConsole.h"
#include "pasObjLayer.h"
#include "pasObjCluster.h"

// #############################################################################
namespace pas { namespace obj {


// *****************************************************************************
// **							  Special Members							  **
// *****************************************************************************

//______________________________________________________________________________
Layer::Layer(const int& cols, const int& rows, pas::obj::Cluster* clGen)
{
	/*! Default ctor. \c width is the pixel array width, and \c height 
	 *	its height. 
	 */
	
	// Define the pixel storage space.
	//_Pixel = new cake::trk::Layer<unsigned int>(width, height);
	_pxCols = cols;
	_pxRows = rows;
	_pxCount = cols * rows;
	_pxMap = new unsigned short [_pxCount];

	// Reset the hits storage(s).
	_HitIdx.clear();
	_HitXY[0].clear();
	_HitXY[1].clear();
	
	// Initializes the random number generator.
	_Rand.SetSeed((unsigned int)(time(NULL) % 16384));

	// CLuster generator.
	_clGen = clGen;
}

//______________________________________________________________________________
Layer::~Layer()
{
	// Remove the pxMap array.
	delete[] _pxMap;
}


// *****************************************************************************
// **								Data Filling							  **
// *****************************************************************************

//______________________________________________________________________________
void Layer::FillHits(const double& count, const pas::obj::Ana& MA)
{
	/*!	Fills the pixel layer with precisely \c 'count' hits having random uniform
	 *	spatial distribution. Uses single pixels or clusters accordingly to the
	 *	cluster flag in the MA structure.
	 */
	
	// Cleans the map.
	if (MA._Sim.flagNoise) FillNoise(MA);
	else for (long i = 0; i < _pxCount; i++) _pxMap[i] = 0;

	// Use Poisson in case.
	unsigned int hitsCount;	
	if (MA._Sim.flagPoisson) hitsCount = static_cast<unsigned int>(floor(0.5 + _Rand.Poisson(count)));
	else hitsCount = static_cast<unsigned int>(floor(count + 0.5));

	// Defines the pivot arrays to be filled with the relative hit coords.
	double* pxIdxR = new double[hitsCount];
	_Rand.RndmArray(hitsCount, pxIdxR); 
	
	// Fit the hitLists to accomodate the projected hits count.
	_HitIdx.resize(hitsCount);
	_HitXY[0].resize(hitsCount);
	_HitXY[1].resize(hitsCount); 
	
	// Fill the pixel map.
	unsigned int pxIdx;
	for (unsigned int i = 0; i < hitsCount; i++) {
	
		// Calculate absolute hit index value and store it.
		pxIdx = static_cast<unsigned int>(floor(pxIdxR[i] * _pxCount));	
	
		// Add a cluster or a single hit to the _pxMap.
		if (MA._Sim.flagCluster) ClusterAdd(pxIdx);
		else _pxMap[pxIdx] ++;
			
		// Updates the hitlist.
		_HitIdx[i] = pxIdx;	
		_HitXY[0][i] = static_cast<unsigned short>(pxIdx % _pxCols);
		_HitXY[1][i] = static_cast<unsigned short>(pxIdx / _pxCols);
	}

	// Clear the pivot arrays.
	delete[] pxIdxR;
}


//______________________________________________________________________________
void Layer::FillNoise(const pas::obj::Ana& MA)
{
	/*!	Fills the pixel layer with gaussian noise of average value 'avg' and 
	 *	dispersion 'sigma'. If using clusters is enabled and a cluster 
	 *	collection is available, directly use the real cluster noise.
	 */
	
	// It'a all about noise!
	double noise;
	unsigned int bias = 4 * MA._Sim.noiseParam[1]; // 4-sigma baseline.

	// Cleans the map.
	if (MA._Sim.flagCluster && MA._Sim.clzsRootsCount > 0) {
		int mtxX, mtxY, mtxCols, mtxRows;
		mtxX = mtxY = mtxCols = mtxRows = 0;
		float* mtxNoise = 0;
		
		// Covers the full _pxMap.
		while (mtxX < _pxCols && mtxY < _pxRows) {
			
			// Get the noise matrix.
			_clGen->clGetNoise(mtxNoise, mtxCols, mtxRows); 
			
			// Stitch the noise!
			for (int y = 0; y < mtxRows; y++) {
				for (int x = 0; x < mtxCols; x++) {
					int i = (mtxY + y) * _pxCols + mtxX + x;
					if (i < _pxCount) {
						noise = floor(0.5 + _Rand.Gaus(0, mtxNoise[y * mtxCols + x]));
						if (bias + noise >= 0) _pxMap[i] = static_cast<unsigned int>(bias + noise);
						else _pxMap[i] = static_cast<unsigned int>(bias - noise);
					}
				}
			}

			// Increase stitching position.
			mtxX += mtxCols;
			if (mtxX >= _pxCols) {
				mtxX = 0;
				mtxY += mtxRows;
			}

			// Remove the noise matrix.
			delete[] mtxNoise;		
		}

	// Uniform gaussian noise (no from collection).
	} else {
		for (long i = 0; i < _pxCount; i++) {
			noise = _Rand.Gaus(MA._Sim.noiseParam[0], MA._Sim.noiseParam[1]);
			if (bias + noise >= 0) _pxMap[i] = static_cast<unsigned int>(bias + noise);
			else _pxMap[i] = static_cast<unsigned int>(bias - noise);
		}
	}
}


//______________________________________________________________________________
void Layer::ClusterAdd(const unsigned int& pxIdx, const unsigned short& offset) 
{
	/*! Add a cluster to the current '_pxMap'. Cluster center is passed as pixel 
	 *	address 'pxIdx'. If defined, it sums 'offset' to every cluster pixel.
	 */
   	
	// Prepare the cluster canvas.
	unsigned short* mtx = 0;
	int cols = 0;
	int rows = 0;
	
//std::cout << "Ops\n";

	// Get the cluster.
	_clGen->clGet(mtx, cols, rows); 
	int hCols = static_cast<int>(floor(0.5 + cols / 2));
	int hRows = static_cast<int>(floor(0.5 + rows / 2));
	int offX = (pxIdx % _pxCols) - hCols;
	int offY = (int)(pxIdx / _pxCols) - hRows;
//std::cout << "cols: " << cols << "\n";
//std::cout << "rows: " << rows << "\n";
//std::cout << "hCols: " << hCols << "\n";
//std::cout << "hRows: " << hRows << "\n";
//std::cout << "offX: " << offX << "\n";
//std::cout << "offY: " << offY << "\n";
//std::cout << "Ops2\n"; 
	// Get cluster and pxMap averages to correctly place it.	
	int tX, tY;
	long pxAvg = 0, pxCount = 0;
	long clAvg = 0, clCount = 0;
	unsigned short clMin = mtx[hRows * cols + hCols];
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			
			// clMap.
			if (mtx[y * cols + x] > 0 && x != hCols && y != hRows) {
				if (mtx[y * cols + x] < clMin) clMin = mtx[y * cols + x];
				clAvg += mtx[y * cols + x];
				clCount++;
			}

			// pxMap.
			tX = x + offX;
			tY = y + offY;
			if (tX >= 0 && tY >= 0) {
				if (tX < _pxCols && tY < _pxRows) {
					pxAvg += _pxMap[tY * _pxCols + tX];
					pxCount++;
				}
			}
		}
	}
	if (pxCount > 0) pxAvg /= pxCount;
	if (clCount > 0) clAvg /= clCount;
//std::cout << "pxAvg: " << pxAvg << "\n";
//std::cout << "clAvg: " << clAvg << "\n";
	
	// Set the cluster bias level.
	int clBias = pxAvg;// - clAvg + offset;
	if (clBias < 0) clBias = 0;

	// Stitches the cluster to the current _pxMap.
	for (int y = 1; y < (rows - 1); y++) {
		for (int x = 1; x < (cols - 1); x++) {
			if (mtx[y * cols + x] > 0) {
				tX = x + offX;
				tY = y + offY;
				if (tX >= 0 && tY >= 0) {
					if (tX < _pxCols && tY < _pxRows) {
						
						//_pxMap[tY * _pxCols + tX] =	static_cast<unsigned short>(mtx[y * cols + x]);
						//_pxMap[tY * _pxCols + tX] = static_cast<unsigned short>(mtx[y * cols + x] + pxAvg - clAvg + offset);
						_pxMap[tY * _pxCols + tX] += mtx[y * cols + x] - clMin;// + clBias;
					}
				}
			}
		}
	}

	// Clear pivot array.
	delete[] mtx;
}



// *****************************************************************************
// **								 Data Access							  **
// *****************************************************************************

//______________________________________________________________________________
const unsigned short* Layer::GetPxMap() const
{
	/*!	Returns the pixel map. Pixel map total dimension is given by the
	 */
	return _pxMap;
}

//______________________________________________________________________________
long Layer::GetPxCount() const
{
	/*!	Returns total the pixel count. */
	return _pxCount;
}

//______________________________________________________________________________
int Layer::GetPxCols() const
{
	/*!	Returns the pixel map columns. */
	return _pxCols;
}

//______________________________________________________________________________
int Layer::GetPxRows() const
{
	/*!	Returns the pixel map rows. */
	return _pxRows;
}

//______________________________________________________________________________
const std::vector<unsigned int>& Layer::GetHitIdx() const 
{
	/*!	Returns the Hits index position vector. An index position is the unique 
	 *	index the hit has into the linear array allocation table. The XY position  
	 *	can be retrieved from the Idx position as X = Idx % cols, where cols is
	 *	the matrix columns number, and Y = floor(Idx / cols), where floor is the 
	 *	floor function.
	 *	The size of the vector is equal to the \c count argument value issued in 
	 *	the last call to a Fill function, to the number of hits present into the 
	 *	layer (n hits in the same pixel add n to the total count). 
	 */
	return _HitIdx;
}

//______________________________________________________________________________
const std::vector<unsigned short>& Layer::GetHitX() const 
{
	/*! Returns the Hits X position vector. The size of the vector is equal to
	 * the \c count argument value issued in the last call to a Fill function. 
	 */
	return _HitXY[0];
}

//______________________________________________________________________________
const std::vector<unsigned short>& Layer::GetHitY() const 
{
	/*! Returns the Hits Y position vector. The size of the vector is equal to
	 * the \c count argument value issued in the last call to a Fill function. 
	 */
	return _HitXY[1];
}

// *****************************************************************************
// **								Miscellaneous							  **
// *****************************************************************************

//______________________________________________________________________________
TH2I* Layer::Dump(bool graphical) const 
{
	/*! Dump to screen the layer matrix. The 'graphical' bool (false by default)
	 *	switches between text based and graphical output. For big layers it is
	 *	strongly suggested to use the graphical one.
	 */
	
	// Text mode.
	if (!graphical) {

		// Build a reference array.
		unsigned char* map = new unsigned char[_pxCount];	
		for (int i = 0; i < _pxCount; i++) map[i] = 0;	

		// Fill it with the hitList.
		for (unsigned int i = 0; i < _HitIdx.size(); i++) map[_HitIdx[i]] = 1;	
	
		// Set hex mode.
		std::cout << std::hex;

		// Loop over the pixels
		for (int y = _pxRows - 1; y >= 0; y--) {
		
			// Row label.
			std::cout << COL(CYAN) << " " << y << COL(DEFAULT);
			if (y < 16) std::cout << " ";
	
			// Row values.
			for (int x = 0; x < _pxCols; x++) {
			
				// Data (colorized if > 0);
				unsigned int v = map[y * _pxCols + x];
				if (v > 0) std::cout << COL(LRED);
				std::cout << std::setw(2) << "X";
				if (v > 0) std::cout << COL(DEFAULT);
			}
			std::cout << "\n";
		}

		// Columns labels.
		std::cout << "   " << COL(CYAN);
		for (int x = 0; x < _pxCols; x++) std::cout << std::setw(2) << x;
		std::cout << "\n";
		
		// Restore color and decimal mode.
		std::cout << std::dec << COL(DEFAULT);

		// Nothing to return.
		return 0;

	// Graphical mode.	
	} else {
		
		// Set he palette.
		//gStyle->SetPalette(100, 0);	// Deep Sea.
		gStyle->SetPalette(1);	// Spectrum.

		// The same, but with a root histo.
		TH2I* histo = new TH2I("","Layer Map", _pxCols, 0, _pxCols, _pxRows, 0, _pxRows);

		// Fill the histo with the hit map.
		for (int y = 0; y < _pxRows; y++) {
			for (int x = 0; x < _pxCols; x++) {
				//if (_pxMap[y * _pxCols + x] > 0) {
					//histo->Fill(x, y);
					histo->Fill(x, y, _pxMap[y * _pxCols + x]);
				//}
			}
		}

		// Return the created histo.
		return histo;  
	
	} // Enf of (graphical) IF

}



// #############################################################################
}} // Close namespace.