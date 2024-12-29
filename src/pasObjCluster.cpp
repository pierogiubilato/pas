//------------------------------------------------------------------------------
// PAS - Cluster Object														  --
// (C) Piero Giubilato 2011-2013, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pas_obj_Cluster.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"27 Sep 2012"
// [Language]		"C++"
//______________________________________________________________________________


// PAS general.
#include "pasConsole.h"

// Include application main.
#include "pasObjCluster.h"


// #############################################################################
namespace pas { namespace obj {


// *****************************************************************************
// **							  Special Members							  **
// *****************************************************************************

//______________________________________________________________________________
Cluster::Cluster()
{
	/*! Default ctor. \c width is the pixel array width, and \c height 
	 *	its height. 
	 */
	
	// Collection files.
	// -----------------

		// Zeroes the headers.
		for (int i = 0; i < 16; i++) {
			_clFile.frameHeader[i] = 0;
			_clFile.spillHeader[i] = 0; 
		}		

		// Storage vectors (used by file loader) cleanup (MANDATORY!!!)
		_clFile.clsCenX = 0;		// Cluster x center off mass.	
		_clFile.clsCenY = 0;		// Cluster y center off mass.
		_clFile.clsMax = 0;			// Max cluster value.	
		_clFile.clsMtxSgn = 0;		// Cluster pixel matrix (signal).
		_clFile.clsMtxNoise = 0;	// Cluster pixel matrix (noise).
		_clFile.clsSeedX = 0;		// Cluster seed X.
		_clFile.clsSeedY = 0;		// Cluster seed Y.
		_clFile.clsFID = 0; 		// Cluster frame ID.
		_clFile.clsPID = 0;			// Cluster plane ID.
	
		// Sets file dir and count.
		_clFile.Folder = "C:/Develop_Files/Analysis/pas/Clusters/";
	
		// Set files root & name.
		// ---------------------
		_clFile.Root.resize(4);
		for (unsigned int i = 0; i < _clFile.Root.size(); i++) {
			_clFile.Root[i].Name = "";
			_clFile.Root[i].Count = 0;
			_clFile.Root[i].Selected = false;
		}
		_clFile.Root[0].Name = "VD_50V_30V/Run0074_0_"; _clFile.Root[0].Description = "SOI, 13.75um pitch pixel, 30V Bias"; _clFile.Root[0].Count = 15;
		_clFile.Root[1].Name = "VD_50V_50V/Run0073_0_"; _clFile.Root[1].Description = "SOI, 13.75um pitch pixel, 50V Bias"; _clFile.Root[1].Count = 15;
		_clFile.Root[2].Name = "VD_50V_70V/Run0075_0_"; _clFile.Root[2].Description = "SOI, 13.75um pitch pixel, 70V Bias"; _clFile.Root[2].Count = 15;
		_clFile.Root[3].Name = "VD_50V_90V/Run0076_0_"; _clFile.Root[3].Description = "SOI, 13.75um pitch pixel, 90V Bias"; _clFile.Root[3].Count = 15;

	// Initializes the random number generator.
	_Rand.SetSeed((unsigned int)(time(NULL) % 16384));

	// In case of bad initialization from the Analysis:
	_clParam[0] = 1; // Avg pixel count.
	_clParam[1] = 0; // Pixel count sigma.
	_clParam[2] = 1; // Avg aspect ratio.
	_clParam[3] = 0; // Aspect ratio sigma.
}

//______________________________________________________________________________
Cluster::~Cluster()
{
	// Clear present collection in case.
	for (unsigned int i = 0; i <_clStore.size(); i++) {
		delete[] _clStore[i].mtxData;
		delete[] _clStore[i].mtxNoise; 
	}	
}


// *****************************************************************************
// **								Random Management						  **
// *****************************************************************************

//______________________________________________________________________________
void Cluster::RandomSet(const double* clParam)
{
	/*!	Defines the cluster random generation parameters.
	 *	clParam:	[0] = cluster size (pixels) average value.
	 *				[1] = cluster size distribution sigma.	
	 *				[2] = cluster aspect ratio (X/Y) average value.
	 *				[3] = cluster aspect ratio distrinution sigma.
	 */

	// Stores the params.
	for (unsigned int i = 0; i < 4; i++) _clParam[i] = clParam[i];
	
	// Check.
	if (_clParam[0] < 1) _clParam[0] = 1;
}


// *****************************************************************************
// **							Collection Management						  **
// *****************************************************************************

//______________________________________________________________________________
unsigned int Cluster::CollectionRootCount() const
{
	/*!	Returns the number of available roots. Root's indexes the range from
	 *	0 to CollectionRootCount() - 1.	
	 */
	return _clFile.Root.size();
}

//______________________________________________________________________________
std::string Cluster::CollectionRootTitle(const unsigned int& rootIdx) const
{
	/*!	Returns the title of the 'rootIdx' root. Returns a null string in 
	 *	case of out-of-range index..
	 */
	if (rootIdx < _clFile.Root.size()) return _clFile.Root[rootIdx].Name;
	else return std::string("");  
}


//______________________________________________________________________________
std::string Cluster::CollectionRootDescription(const unsigned int& rootIdx) const
{
	/*!	Returns the description of the 'rootIdx' root. Returns a null string in 
	 *	case of out-of-range index..
	 */
	if (rootIdx < _clFile.Root.size()) return _clFile.Root[rootIdx].Description;
	else return std::string("");  
}


//______________________________________________________________________________
void Cluster::CollectionLoad(const unsigned int* rootList, const unsigned int& rootCount)
{
	/*!	Load a chosen set of files and built a cluster collection, to provide
	 *	a real clusters repository where to look for when populating frames.
	 */
	
	// Sets which roots to load. 
	for (unsigned int i = 0; i < _clFile.Root.size(); i++) {
		_clFile.Root[i].Selected = false;
	}
	for (unsigned int r = 0; r < rootCount; r++) {
		if (rootList[r] < _clFile.Root.size()) {
			_clFile.Root[rootList[r]].Selected = true;
		}
	}

	// Sets collection trees corresponding to those in files.
	_clFile.chainSpillHeader.SetName("headertree");
	_clFile.chainFrameHeader.SetName("eventtree");
	_clFile.chainFrameData.SetName("hitstree");
  	
	// Chain the files.	
	std::cout << "   Cluster collection: loading:\n"; 
	for (unsigned int rootIdx = 0; rootIdx < _clFile.Root.size(); rootIdx++) {

		// If the file root has been selected, chain it!
		if (_clFile.Root[rootIdx].Selected) {
			
			// Info.
			std::cout << "    Loading root: " << _clFile.Root[rootIdx].Name << "... ";
			
			// Load all the files for this root.
			for (int fileIdx = 0; fileIdx < _clFile.Root[rootIdx].Count; fileIdx++) {
			
				// Builds the complete filename.
				std::stringstream fileName;
				fileName << _clFile.Folder << _clFile.Root[rootIdx].Name << fileIdx << "_CLZ.root";
				
				// Insert data into the chain.
				if (fileIdx == 0) _clFile.chainSpillHeader.Add(fileName.str().c_str());
				_clFile.chainFrameData.Add(fileName.str().c_str());
				_clFile.chainFrameHeader.Add(fileName.str().c_str());
			}
			std::cout << "Done!\n";
		}
	}
	
	// Retieve Spill header, and clustering param with it.
	_clFile.chainSpillHeader.SetBranchAddress("run_header", &_clFile.spillHeader);
	_clFile.chainSpillHeader.GetEntry(0);
	
	// Save matrix size and other clusterizing param for the spill
	_clStoreMtxCols = (int)sqrt((float)_clFile.spillHeader[10]);
	_clStoreMtxRows = _clStoreMtxCols;

	// Debug info for the Spill.
	//std::cout << COL(LWHITE) << "   Spill Header:\n" << COL(DEFAULT);
	//std::cout << "   Id           [0]:  " << _clFile.spillHeader[0] << "\n";
	//std::cout << "   Col count    [1]:  " << ana._spillHeader[1] << "\n";	
	//std::cout << "   Row count    [2]:  " << ana._spillHeader[2] << "\n";
	//std::cout << "   Layer count  [3]:  " << _clFile.spillHeader[3] << "\n";
	//std::cout << "   CDS On       [4]:  " << ana._spillHeader[4] << "\n";
	//std::cout << "   Start row    [5]:  " << ana._spillHeader[5] << "\n";	
	//std::cout << "   Start col    [6]:  " << ana._spillHeader[6] << "\n";
	//std::cout << "   Hardware ID  [7]:  " << ana._spillHeader[7] << "\n";	
	//std::cout << "   Detector ID  [8]:  " << ana._spillHeader[8] << "\n";	
	//std::cout << "   Time         [9]:  " << ana._spillHeader[9] << "\n";
	//std::cout << "   Matrix size  [10]: " << _clFile.spillHeader[10] << "\n";	
   	//std::cout << "   Noise Thrs   [11]: " << _clFile.spillHeader[11] << "\n";
	//std::cout << "   Seed Thres   [12]: " << _clFile.spillHeader[12] << "\n";	
	//std::cout << "   Mass Thrs    [13]: " << _clFile.spillHeader[13] << "\n";	
	//std::cout << "   Mult Thrs    [14]: " << _clFile.spillHeader[14] << "\n";
	//std::cout << "   Free         [15]: " << ana._spillHeader[15] << "\n";	

	// Connects frame header pivot to tree structure.
	_clFile.chainFrameHeader.SetBranchAddress("event_header", &_clFile.frameHeader);

	// Connects cluster data pivots to tree structure.
	_clFile.chainFrameData.SetBranchAddress("clusterx", &_clFile.clsCenX);		// X center of gravity for cluster.
	_clFile.chainFrameData.SetBranchAddress("clustery", &_clFile.clsCenY);		// Y center of gravity for cluster.
	_clFile.chainFrameData.SetBranchAddress("clusterh", &_clFile.clsMax);		// Max Value for cluster.
	_clFile.chainFrameData.SetBranchAddress("ph_nxn",	 &_clFile.clsMtxSgn);	// Cluster Matrix data for cluster.
	_clFile.chainFrameData.SetBranchAddress("noise_nxn", &_clFile.clsMtxNoise);	// Cluster Matrix data for cluster noise.
	_clFile.chainFrameData.SetBranchAddress("col_seed", &_clFile.clsSeedX);		// Cluster seed X.
	_clFile.chainFrameData.SetBranchAddress("row_seed", &_clFile.clsSeedY);		// Cluster seed Y.
	_clFile.chainFrameData.SetBranchAddress("fid_val",	 &_clFile.clsFID);		// Cluster plane ID
	_clFile.chainFrameData.SetBranchAddress("pid_val",  &_clFile.clsPID);		// Cluster frame ID

	// Build the cluster collection from the roots data.
	CollectionBuild();
}

//______________________________________________________________________________
void Cluster::CollectionBuild()
{
	/*!	Load a chosen set of files and built a cluster collection, to provide
	 *	a real clusters repository where to look for when populating frames.
	 */

	// Info.
	std::cout << "   Cluster collection: " << COL(LWHITE) << "building" << COL(DEFAULT) << "...\n";

	// Clear present collection.
	for (unsigned int i = 0; i <_clStore.size(); i++) {
		delete[] _clStore[i].mtxData;
		delete[] _clStore[i].mtxNoise; 
	}

	//	Retrieves frame count.
	unsigned long frmCount = static_cast<unsigned long>(_clFile.chainFrameData.GetEntries());
			
	// Cluster counters.
	int clTotalCount = 0;
	int clAcceptedCount = 0;
	
	// Pivot var for addressing and ordering.
	unsigned int mtxSize = _clStoreMtxCols * _clStoreMtxRows;
	unsigned int mtxSide = _clStoreMtxCols;
	
	// In-cluster pixel pivot vars.
	int pxStart = 0;
	
	// Loop over all the frames (1 entry = 1 frame)
	for(unsigned int frmIdx = 0; frmIdx < frmCount; frmIdx++) {
		
		// Retrieves how many clusters in frame.
		_clFile.chainFrameData.GetEntry(frmIdx);
		int clFrameCount = _clFile.clsCenX->size();
		clTotalCount += clFrameCount;
		
		// Loop over all the clusters into the frame
		for(int clsIdx = 0; clsIdx < clFrameCount; clsIdx++) {
				
			// Consider only clusters with PID = 0.
			if (_clFile.clsPID->at(clsIdx) > 0) continue;

			// Retrieves cluster global position (could be any layer).
			int clSeedX = static_cast<int>(_clFile.clsSeedX->at(clsIdx));
			int clSeedY = static_cast<int>(_clFile.clsSeedY->at(clsIdx));
			
			// Skip cluster if not from the singlet.
			if (clSeedX > 63) continue;
			
			// Retrieves cluster's layer from its global position.
			//	0 |	3
			// -------
			//  1 | 2			
			//if (cl.SeedX < lyCols) {
			//	(cl.SeedY < lyRows) ? cl.LyrIdx = 1: cl.LyrIdx = 0;
			//} else {
			//	(cl.SeedY < lyRows) ? cl.LyrIdx = 2: cl.LyrIdx = 3;
			//}
 
			// Gets seed signal and noise.
			//cl.SeedMass = _MA.File.clsMtxSgn->at(pxStart + mtxSizeHalf);
			//clSeedNoise = _MA.File.clsMtxNoise->at(pxStart + mtxSizeHalf);
			// Build up the cluster.
			pxStart = clsIdx * mtxSize;
			kClItem clNew;
			clNew.mtxData = new unsigned short[mtxSize];
			clNew.mtxNoise = new float[mtxSize];
			unsigned int pxOverNoise = 0;
			unsigned int pxOverThreshold = 0;
			
			// Loop over all matrix pixels.
			for(unsigned int i = 0; i < mtxSize; i++) {
				
				// Copy matrixes.
				clNew.mtxNoise[i] = _clFile.clsMtxNoise->at(pxStart + i); 
				double pxValue = floor(0.5 + _clFile.clsMtxSgn->at(pxStart + i));
				if (pxValue > 0) clNew.mtxData[i] = static_cast<unsigned int>(pxValue);
				else clNew.mtxData[i] = static_cast<unsigned int>(-pxValue);
				
				// Get cluster quality info.
				if (pxValue > (1 + clNew.mtxNoise[i]) * 4) pxOverNoise++;
				if (pxValue > (1 + clNew.mtxNoise[i]) * 10) pxOverThreshold++;
			}

			// Check the cluster quality (no more tha 80% pixel over zero)
			if (pxOverThreshold >= 1 && pxOverThreshold < mtxSide) _clStore.push_back(clNew);		
			//else if (pxOverNoise >= 4 && pxOverNoise < mtxSize * 0.5) _clStore.push_back(clNew);
			//_clStore.push_back(clNew);
		
		} // End loop over clusters
	} // End loop over frames (frmIdx).
	
	// Info
	std::cout << "   Parsed clusters: " << COL(LWHITE) << clTotalCount << COL(DEFAULT) << "\n";
	std::cout << "   Stored clusters: " << COL(LWHITE) << _clStore.size() << COL(DEFAULT) << "\n";

	// Clear files.

}

// *****************************************************************************
// **								Data Filling							  **
// *****************************************************************************

//______________________________________________________________________________
void Cluster::clGet(unsigned short*& mtx, int& cols, int& rows) 
{
	/*! Generate a cluster contained into a matrix of size 'cols' * 'rows'. 
	 *	The cluster is returned as a pointer to a newly allocated unsigned 
	 *	short matrix 'mtx'. The cluster will be randomly generated or randomly
	 * picked accordingly to the availability of the cluster collection.
	 */

	if (_clStore.size() > 0) clGetStored(mtx, cols, rows);	
	else clGetRandom(mtx, cols, rows);
}


//______________________________________________________________________________
void Cluster::clGetRandom(unsigned short*& mtx, int& cols, int& rows) 
{
	/*! Generate a cluster contained into a matrix of size 'cols' * 'rows'. 
	 *	The cluster is returned as a pointer to a newly allocated unsigned 
	 */
	
	// Cluster generation vars.
	double clSize;
	double clSeed;
	int hCols, hRows, mtxCount;
		
	// Generates (gaussian) cluster seed, pixel count, aspect ratio, ...	
	clSeed = _Rand.Gaus(_clParam[0], _clParam[1]); 
	if (clSeed < 1) clSeed = 1;
	clSize = _Rand.Gaus(_clParam[2], _clParam[3]); 
	if (clSize < 1) clSize = 1;
	cols = static_cast<unsigned int>(ceil(sqrt(clSize)));
	rows = static_cast<unsigned int>(ceil(sqrt(clSize)));

	// Optimize canvas matrix size..
	hCols = static_cast<int>(ceil(cols / 2.0)) + 1; // Half cluster width.
	hRows = static_cast<int>(ceil(rows / 2.0)) + 1; // Half cluster height.
	cols = 2 * hCols + 1;
	rows = 2 * hRows + 1;
	mtxCount = (cols * rows);
	
	// The cluster support Matrix is 2 rows and 2 columns bigger
	// than the actual accepted max cluster's width and height
	// to allow for easy border checking. Its central pixel is by
	// default switched on.
	mtx = new unsigned short[mtxCount];
	unsigned short* clMtx = new	unsigned short[mtxCount];
	for (int i = 0; i < mtxCount; i++) {
		clMtx[i] = 0;
		mtx[i] = 0;
	}
	clMtx[hRows * cols + hCols] = 1;
	
	// Populates the cluster.
	int pxX, pxY, pxCount = 1, trCount = 0;
		
	// Throw pixels until the required number is accepted.
	while (pxCount < clSize && trCount < clSize * 2) {
			
		// Throw a pixel with by axis gaussian dispersion around the center
		pxX = static_cast<int>(floor(0.5 + hCols + _Rand.Gaus(0, hCols / 2)));
		pxY = static_cast<int>(floor(0.5 + hRows + _Rand.Gaus(0, hRows / 2)));
		
		if (pxX < 1 || pxX >= cols - 1) continue;
		if (pxY < 1 || pxY >= rows - 1) continue;
		
		// If the pixel is already hit, increase but do not count it.
		if (clMtx[pxY * cols + pxX] > 0) {
			clMtx[pxY * cols + pxX] ++;			
			continue;
		}
			
		// Check how many close neighborhood pixels are filled.
		int nC = 0;
		if (clMtx[pxY * cols + pxX + 1] > 0) nC++;		
		if (clMtx[pxY * cols + pxX - 1] > 0) nC++;
		if (clMtx[(pxY - 1) * cols + pxX] > 0) nC++;
		if (clMtx[(pxY + 1) * cols + pxX] > 0) nC++;

		// Accept it ony if at least one neighborhood pixel is filled.
		if (nC > 0) {
			clMtx[pxY * cols + pxX]++;
			pxCount++;		
		}
		
		// If too many throws, exit anyway.
		trCount++;
	} // End of cluster population.
	
	// Adapt cluster heights..
	clMtx[hRows * cols + hCols] = static_cast<unsigned short>(clSeed);
	for (int y = 1; y < (rows - 1); y++) {
		for (int x = 1; x < (cols - 1); x++) {
			//mtx[y * cols + x] = clMtx[y * cols + x] * 11;
			double dst = sqrt(pow(0.0 + x - hCols, 2) + pow (0.0 + y - hRows, 2));
			dst = 1 + _Rand.Uniform() * (dst -1);
			if (dst > 0) {
				mtx[y * cols + x] = static_cast<unsigned short>(_Rand.Gaus(clSeed / dst, _clParam[1]));
				//mtx[y * cols + x] = static_cast<unsigned short>(_Rand.Gaus(clSeed / dst, 1));
			}
		}
	}
	
	// Apply a smooth filter to the generated cluster while moving it to the target matrix.
	//std::cout << pxIdx % _pxCols << ", " << (int)(pxIdx / _pxCols) << "\n";
	double sum = 0;
	for (int y = 1; y < (rows - 1); y++) {
		for (int x = 1; x < (cols - 1); x++) {
			mtx[y * cols + x] = clMtx[y * cols + x] * 11;
			for (int nY = y - 1; nY <= y + 1; nY++) {
				for (int nX = x - 1; nX <= x + 1; nX++) {
					mtx[y * cols + x] += clMtx[nY * cols + nX];
				}	
			}
			mtx[y * cols + x] = static_cast<unsigned short>(floor(0.5 + 1.0 * mtx[y * cols + x] / 20));
			//mtx[y * cols + x] = clMtx[y * cols + x];
		}
	}
	
	// Debug.
	//for (int y = 0; y < rows; y++) {
	//	for (int x = 0; x < cols; x++) {
	//		std:: cout << mtx[y * cols + x] << " ";
	//	}
	//	std::cout << "\n";
	//}
	//std::cout << "\n";

	// Clear pivot arrays.
	delete[] clMtx;
}

//______________________________________________________________________________
void Cluster::clGetStored(unsigned short*& mtx, int& mtxCols, int& mtxRows)
{
	/*! Copy a cluster into 'mtx'. Loads 'mtxCols' and 'mtxRows' with 'mtx'
	 *	dimensions. The cluster will be shifted to have its smaller pixel 
	 *	equal to zero.
	 */

	// Random chose orientation.
	unsigned int clOrt = _Rand.Integer(4); 

	// Fit the target matrix accordingly to orientation.
	if (clOrt == 0 || clOrt == 2) { 
		mtxCols = _clStoreMtxCols;
		mtxRows = _clStoreMtxRows;
	} else {
		mtxCols = _clStoreMtxRows;
		mtxRows = _clStoreMtxCols;
	}
	unsigned int mtxCount;
	mtxCount = mtxCols * mtxRows;
	mtx = new unsigned short[mtxCount];	

	// Select one memebr from the loaded collection.
	unsigned int clIdx = _Rand.Integer(_clStore.size()); 

	// Get cluster minimum.
	unsigned short clMin = _clStore[clIdx].mtxData[0];
	for (unsigned int i = 1; i < mtxCount; i++) {
		if (_clStore[clIdx].mtxData[i] < clMin) clMin = _clStore[clIdx].mtxData[i]; 
	}

	// Copy it to the target matrix, applying rotation.
	unsigned int t;
	for (unsigned int i = 0; i < mtxCount; i++) {
		switch (clOrt) {
			case 0: t = i;							// Linear.
			case 1:	t = mtxCols - 1 - i % mtxCols + (int)(i / mtxCols) * 4;	// X Mirror.	
			case 2:	t = i % mtxCols + mtxCols * (mtxRows - 1) - (int)(i / mtxCols) * 4;	// Y Mirror.
			case 3:	t = mtxCount - i - 1;			// XY Mirror.
		}
		mtx[t] = _clStore[clIdx].mtxData[t] - clMin; 	
	}
}

//______________________________________________________________________________
void Cluster::clGetNoise(float*& mtx, int& cols, int& rows) 
{
	/*! Generate a cluster contained into a matrix of size 'cols' * 'rows', 
	 *	but just noise-filled (containing the sigma value of the noise for
	 *	the pixel.)
	 */

	if (_clStore.size() > 0) clGetNoiseStored(mtx, cols, rows);	
	else {
		cols = _clStoreMtxCols;
		rows = _clStoreMtxRows;
		mtx = new float[cols * rows];
		for (int i = 0; i < cols * rows; i++) mtx[i] = 0;
	}
}

//______________________________________________________________________________
void Cluster::clGetNoiseStored(float*& mtx, int& cols, int& rows) 
{
	/*! Generate a cluster contained into a matrix of size 'cols' * 'rows', 
	 *	but just noise-filled (containing the sigma value of the noise for
	 *	the pixel.)
	 */

	// Random chose orientation.
	unsigned int clOrt = _Rand.Integer(4); 

	// Fit the target matrix accordingly to orientation.
	if (clOrt == 0 || clOrt == 2) { 
		cols = _clStoreMtxCols;
		rows = _clStoreMtxRows;
	} else {
		cols = _clStoreMtxRows;
		rows = _clStoreMtxCols;
	}
	unsigned int mtxCount;
	mtxCount = cols * rows;
	mtx = new float[mtxCount];	

	// Select one memebr from the loaded collection.
	unsigned int clIdx = _Rand.Integer(_clStore.size()); 

	// Copy it to the target matrix, applying rotation.
	unsigned int t;
	for (unsigned int i = 0; i < mtxCount; i++) {
		switch (clOrt) {
			case 0: t = i;							// Linear.
			case 1:	t = cols - 1 - i % cols + (int)(i / cols) * 4;	// X Mirror.	
			case 2:	t = i % cols + cols * (rows - 1) - (int)(i / cols) * 4;	// Y Mirror.
			case 3:	t = mtxCount - i - 1;			// XY Mirror.
		}
		mtx[t] = _clStore[clIdx].mtxNoise[i]; 	
	}	
}

// #############################################################################
}} // Close namespace.