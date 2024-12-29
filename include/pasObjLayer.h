//------------------------------------------------------------------------------
// PAS - Layer Object														  --
// (C) Piero Giubilato 2011-2013, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasObjLayer.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"25 Sep 2012"
// [Language]		"C++"
//______________________________________________________________________________

// Overloading check
#ifndef pasObjLayer_H
#define pasObjLayer_H

// Root.
#include "TH2I.h"

// Application main.
#include "pasObjCluster.h"
#include "pasObjAna.h"

// #############################################################################
namespace pas {	namespace obj {

//! Hit object class.
/*! pas::obj::Hit is the object used to store one hit.
 *	
 *	\author Piero Giubilato
 *	\version 1.2
 *	\date 25 Sep 2012
 */

//class Cluster {};

//______________________________________________________________________________
class Hit
{
	public:

		// Data.
		double _X;
		double _Y;
		unsigned char* _clMtx;
		unsigned int _clCols;
		unsigned int _clRows;

		// Special members.
		Hit(const double& x, const double& y) {_X = x; _Y = y;};
		~Hit() {delete[] _clMtx;};
};


//! Layer object class.
/*! pas::Layer is the object used to manage/fill pixel layers. It can store,
 *	fill and retrieve hits data and provide convenient projections calculation
 *	and access. Its filling routines (both uniform and Poisson) are optimized
 *	for fast execution through direct memory access allocation.
 *	
 *	\author Piero Giubilato
 *	\version 1.2
 *	\date 28 Mar 2012
 */
//______________________________________________________________________________
class Layer
{
	private:   

		// Pixel store.
		int _pxCols;
		int _pxRows;
		long _pxCount;
		unsigned short* _pxMap;

		// Hits store.
		std::vector<unsigned int> _HitIdx;
		std::vector<unsigned short> _HitXY[2];
		
		// Clustr generator.
		pas::obj::Cluster* _clGen;
		
		// Random generator.
		TRandom3 _Rand;

	protected:

		// No protected at the moment
						
	public:

		//! Default ctor.
		Layer(const int& cols, 
			  const int& rows, 
			  pas::obj::Cluster* clGen);
		~Layer(); //!< Dtor.

		
		// Setting.
		// --------
		

		// Filling.
		// --------
			
			// Fill with noise
			void FillNoise(const pas::obj::Ana&);

			// Fill with precisely count hits.
			void FillHits(const double& count, const pas::obj::Ana&);
							 
			// Add a cluster.
			void ClusterAdd(const unsigned int& pxIdx, const unsigned short& = 0);
			
		// Retrieving data.
		// ----------------
		
			// PixMap.
			const unsigned short* GetPxMap() const;	// Returns the pixel map.
			long GetPxCount() const;			// Returns the pixel map pixels count.
			int GetPxCols() const;				// Returns the pixel map pixels columns.
			int GetPxRows() const;				// Returns the pixel map pixel rows.
			 

			// HitLists.
			const std::vector<unsigned int>& GetHitIdx() const;	// Returns Hit indexes.
			const std::vector<unsigned short>& GetHitX() const;	// Returns Hit X array.
			const std::vector<unsigned short>& GetHitY() const;	// Returns Hit Y array.
		
		// Misc.
		TH2I* Dump(bool graphical = false) const;			// Show the layer map.				

};

// #############################################################################
}} // Close Namespaces

// Overloading check.
#endif
