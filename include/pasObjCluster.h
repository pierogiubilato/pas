//------------------------------------------------------------------------------
// PAS - Cluster Object														  --
// (C) Piero Giubilato 2011-2013, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasObjCluster.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"25 Sep 2012"
// [Language]		"C++"
//______________________________________________________________________________

// Overloading check
#ifndef pasObjCluster_H
#define pasObjCluster_H

// Root.
#include "TChain.h"
#include "TRandom3.h"

// Application main.
//#include "pasMain.h"

// #############################################################################
namespace pas {	namespace obj {

//! Hit object class.
/*! pas::obj::Hit is the object used to store one hit.
 *	
 *	\author Piero Giubilato
 *	\version 1.2
 *	\date 25 Sep 2012
 */


//! Cluster object class.
/*! pas::obj::Cluster is the object used to manage/fill pixel Clusters. It can store,
 *	fill and retrieve hits data and provide convenient projections calculation
 *	and access. Its filling routines (both uniform and Poisson) are optimized
 *	for fast execution through direct memory access allocation.
 *	
 *	\author Piero Giubilato
 *	\version 1.2
 *	\date 25 Sep 2012
 */
//______________________________________________________________________________
class Cluster
{
	private:   

		// Pixel store.
		unsigned int _pxCols;
		unsigned int _pxRows;
		unsigned long _pxCount;
		unsigned char* _pxMap;

		// File pivots for the collection.
		struct kFile {
			
			// General folder.
			std::string Folder;

			// File root(s).
			struct kFileRoot {std::string Name; 
							  std::string Description; 
							  int Count; 
							  bool Selected;};
			std::vector<kFileRoot> Root;
			
			// Pivot vector for the Spill  header.
			Float_t spillHeader[16];

			// Pivot vector for the Frame  header.
			Int_t frameHeader[16];

			// Vectors for the tree entries.
			std::vector<Float_t>* clsCenX;	// Cluster x center off mass.	
			std::vector<Float_t>* clsCenY;	// Cluster y center off mass.
			std::vector<Float_t>* clsMax;	// Max cluster value.	
			std::vector<Float_t>* clsMtxSgn;// Cluster pixel matrix (signal).
			std::vector<Float_t>* clsMtxNoise;	// Cluster pixel matrix (noise).
			std::vector<Int_t>* clsSeedX;	// Cluster seed X.
			std::vector<Int_t>* clsSeedY;	// Cluster seed Y.
			std::vector<UInt_t>* clsFID; 	// Cluster frame ID.
			std::vector<UInt_t>* clsPID;	// Cluster plane ID.
			
			// Chains for CDS data
			TChain chainSpillHeader;		// ("clsHeader");
			TChain chainFrameHeader;		// ("clsHeader");
			TChain chainFrameData;			// ("clsHData");

			// Avoid troubles with self-generated copy constructor.
			// Root TChain cannot be copied (cannot access its parent 
			// class members), so avoid any operation involving copying
			// this structure, just use pointers.
			//kFile& operator=(kFile const&);
		};
		
		// Collection files.
		kFile _clFile;
		
		// Collection item.
		struct kClItem {unsigned short* mtxData;
						float* mtxNoise;};

		// Collection vector.
		//unsigned int _clMtxCount;
		unsigned int _clStoreMtxCols;
		unsigned int _clStoreMtxRows;
		std::vector<kClItem> _clStore;

		// Random generation parameters.
		double _clParam[4];				
		// Random generator.
		TRandom3 _Rand;
	
	protected:

		// Build the collection after file loading.
		void CollectionBuild();
					
	public:

		// Special members.
		Cluster();	//!< Default ctor.
		~Cluster();	//!< Dtor.

		// Colletion setting/properties.
		unsigned int CollectionRootCount() const;
		std::string CollectionRootTitle(const unsigned int& rootIdx) const;
		std::string CollectionRootDescription(const unsigned int& rootIdx) const;
		void CollectionLoad(const unsigned int* rootList, const unsigned int& rootCount);
			
		// Random setting.
		void RandomSet(const double* clParam);
		
		// Cluster retrive.
		void clGet(unsigned short*& mtx, int& cols, int& rows);
		void clGetRandom(unsigned short*& mtx, int& cols, int& rows);
		void clGetStored(unsigned short*& mtx, int& cols, int& rows);
		void clGetNoise(float*& mtx, int& cols, int& rows);
		void clGetNoiseStored(float*& mtx, int& cols, int& rows);
};

// #############################################################################
}} // Close Namespaces

// Overloading check.
#endif
