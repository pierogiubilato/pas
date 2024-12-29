//------------------------------------------------------------------------------
// PAS - Pixel Architecture Simulation - Reconstruction test Unit			  --
// (C) Piero Giubilato 2010-2012, Padova University							  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pasAnaReco.h"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"17 Sep 2012"
// [Language]		"C++"
//______________________________________________________________________________


// Overloading check
#ifndef pasAnaReco_H
#define pasAnaReco_H

// ROOT.
#include "TRandom3.h"

// PAW objects.
#include "pasObjLayer.h"

// #############################################################################
namespace pas { namespace ana {

//! Basic analysis class.
/*! pas::ana::Reco is the unit which test the reconstruction algorithm and
 *	the projective compression encoding-decoding.
 *	
 *	\author Piero Giubilato
 *	\version 1.5
 *	\date 17 Sep 2012
 */
//______________________________________________________________________________
class Reco
{
	private:

	// Data members	
	// ------------

		// Current analysis.
		pas::obj::Ana& _MA;

		// Random generator.
		TRandom3 _Rand;

		// Current step information.
		unsigned long _rStepCurr;
		std::vector<std::string> _rStepTitle;

		// T (XYUV) Maps.
		unsigned short* _XYtoUV[2];
		unsigned short* _UVtoXY[2];
		unsigned short	_TLayerN;
		unsigned short	_TN;
		unsigned short*	_TUseTable;
		unsigned short*	_TProj[4];
		
		
		// T Clx extensions.
		// -----------------

		// Visual result of the digital encoding.
		unsigned short*	_TdigiMap;
		
		// A 4D pixel object.
		class pxObj {
			public:
			unsigned int _X;
			unsigned int _Y; 
			unsigned int _U;
			unsigned int _V;
			pxObj(unsigned int x = 0, unsigned int y = 0, 
				  unsigned int u = 0, unsigned int v = 0)
				{
					_X = x; _Y = y; _U = u; _V = v;
				};
		};
				
		// A cluster projection object.
		class clPrjObj {
			public:
			unsigned int _Start;
			unsigned int _Stop; 
			unsigned int _Size;
			unsigned int _Used;
			
			// Ctor.
			clPrjObj(unsigned int start = 0, unsigned int stop = 0, 
				  unsigned int size = 0, unsigned int used = 0)
				{
					_Start = start; _Stop = stop;
					_Size = size; _Used = used;
				};
		};

		// A cluster object.
		class clObj {
			public:
			std::vector<pxObj> _pxList;
			
			// Ctor.
			clObj(const pxObj& px) {_pxList.push_back(px);}; 
			
			// Belong.
			bool Belong(const unsigned int& x, const unsigned int& y) const
				{
					for (unsigned int i = 0; i < _pxList.size(); i++) {
						if (_pxList[i]._X == x && _pxList[i]._Y == y) return true;
					}
					return false;
				};
		};

		// End of T Clx Extentions.
		// ------------------------
		
		// DPix.
		// -----

		unsigned short* _DpxMap;	// The "derived" _pxMap.

		// Results stat.
		std::vector<double> _rHitsDensity;
		std::vector<double> _rFrameArea;
		std::vector<double> _rSizeColsN;
		std::vector<double> _rSizeRowsN;
		std::vector<double> _rPxWidth;
		std::vector<double> _rPxHeight;
		std::vector<double> _rFrameHitsAvg;			// Average hits per frame.
		std::vector<double> _rCoordHitsAvg;			// Average hits per coordinate.
		std::vector<double> _rCoordOverkillCount[4];// Total coordinate bin cunting more than one hit.
		std::vector<double> _rFrameHitsCount;		// Total hits count in the frame.
		std::vector<double> _rRecoHitsCount;		// Total hits count in the run.
		std::vector<double> _rRecofWSelfPurity;		// Purity derived by the fW Decode alg.
		std::vector<double> _rRecofWRealPurity;		// Purity checked with fW a-priori test.
		std::vector<double> _rRecoMOPSelfPurity;	// Purity derived by the MOP Decode alg.
		std::vector<double> _rRecoMOPRealPurity;	// Purity checked with MOP a-priori test.
		std::vector<double> _rRecoUVHitsCount;		// Total hits reconstructed by the UV map.
		std::vector<double> _rRecoTAliasCount;		// Total alias remaining in the HitList prior to independece check.
		std::vector<double> _rRecoTOverkillAvg;		// Average coordinate bin cunting more than one hit.
		std::vector<double> _rRecoTOverkillCount[4];// Total usage table bin cunting more than one hit.
		

		// Timing.
//		cake::Timer _Timer;

	
	// Members	
	// -------

		// Analysis workflow.
		void Setup();								//!< Setup analysis.
		void Show();								//!< Show results.
		
		// The fW encoding implementation.
		void WEncode (std::vector<long double>& imgW,
					  const unsigned int& N, const unsigned int& m,
					  const std::vector<unsigned int>& HitIdx);

		// The XYW de-coding algorithm.
		bool XYWDecode(std::vector<unsigned int>& HitIdx,	
					   const std::vector<unsigned char>& prjX,
					   const std::vector<unsigned char>& prjY,
					   const std::vector<long double>& imgW);

		// The Tetra (XYUV) encoding maps generator.
		void TSetup (const unsigned int& N, const unsigned int& m);
		
		// The Tetra (XYUV) encoding cleaner.
		void TClear();

		// The Tetra (XYUV) encoding implementation.
		void TEncode (unsigned short** projections, const unsigned short* pxMap,
					  const unsigned int& pxCols, const unsigned int& pxRows);

		// The Tetra (XYUV) de-coding algorithm.
		bool TDecode(std::vector<unsigned int>& HitIdx, unsigned short const* const*);
		unsigned int TDecodeClx(std::vector<clObj>& clList, unsigned short const* const*, pas::obj::Layer* ly);

		// The DPix (derivative pixel) layer.
		bool DParse(unsigned short* outPxMap, 
					const unsigned short* inPxMap, 
					const unsigned int& pxCols, 
					const unsigned int& pxRows);

		// Support functions.
		int compareIdxList(const std::vector<unsigned int>& idxListA,
							const std::vector<unsigned int>& idxListB);

		// Support functions.
		int compareClxList(const std::vector<unsigned int>& hitList,
							const std::vector<clObj>& clReco);

		// Debug functions.
		void Reco::debugMap(const pas::obj::Layer& ly,
							const std::vector<unsigned int>& listReal,
							const std::vector<unsigned int>& listReco);

	public:

		// Special members.
		Reco(pas::obj::Ana&);	//!< Default ctor.	
		~Reco();				//!< Default dtor.

		// Analysis workflow.
		void Run();				//!< Run the analysis.
};


// #############################################################################
}}

// Overloading check.
#endif
