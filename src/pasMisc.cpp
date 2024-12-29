//------------------------------------------------------------------------------
// PAS - Analysis object													  --
// (C) Piero Giubilato 2011-2013, CERN										  --
//------------------------------------------------------------------------------

//______________________________________________________________________________
// [File name]		"pas_obj_Ana.cpp"
// [Author]			"Piero Giubilato"
// [Version]		"1.2"
// [Modified by]	"Piero Giubilato"
// [Date]			"24 Oct 2012"
// [Language]		"C++"
//______________________________________________________________________________

// Root.
#include "TGraph.h"
#include "TAxis.h"


// PAS general.
#include "pasConsole.h"
#include "pasMisc.h"

// #############################################################################
namespace pas {

//______________________________________________________________________________
Misc::Misc()
{
 	/*! Default ctor. Set default parameter for the analysis environment. */
}

//______________________________________________________________________________
Misc::~Misc()
{
	/*! Default dtor. */
}

//______________________________________________________________________________
void Misc::DataPlot(pas::obj::Ana& MA)
{
	/*! Plots arbitrary hard-coded data. */

	// Define data points.
	struct kData {double x; double y;};
	int pCount = 0;
	kData pData[100];
	kData pMax, pMin;

	// LePix Diode
	// -----------
	
		//// Define labels.
		//std::stringstream gTitle("Pippo");
		//std::stringstream gXAxis("Bias potential [V]");
		//std::stringstream gYAxis("Current [mA]");
		//
		//// Set graph limits.
		//pCount = 22;
		//pMax.x = 210; pMax.y = 0.15E-3;
		//pMin.x = 0; pMin.y = 0;//-0.02E-3;
	
		//// Set data points.
		//pData[0].x = 0;		pData[0].y = 0;
		//pData[1].x = 10;	pData[1].y = 3.0E-9;
		//pData[2].x = 20;	pData[2].y = 5.0E-9;
		//pData[3].x = 30;	pData[3].y = 8.0E-9;
		//pData[4].x = 40;	pData[4].y = 1.33E-8;
		//pData[5].x = 50;	pData[5].y = 3.2E-7;
		//pData[6].x = 60;	pData[6].y = 1.0E-6;
		//pData[7].x = 70;	pData[7].y = 2.3E-6;
		//pData[8].x = 80;	pData[8].y = 5.0E-6;
		//pData[9].x = 90;	pData[9].y = 8.5E-6;
		//pData[10].x = 100;	pData[10].y = 1.2E-5;
		//pData[11].x = 110;	pData[11].y = 1.65E-5;
		//pData[12].x = 120;	pData[12].y = 2.10E-5;
		//pData[13].x = 130;	pData[13].y = 2.58E-5;
		//pData[14].x = 140;	pData[14].y = 3.09E-5;
		//pData[15].x = 150;	pData[15].y = 3.70E-5;
		//pData[16].x = 160;	pData[16].y = 4.40E-5;
		//pData[17].x = 170;	pData[17].y = 5.20E-5;
		//pData[18].x = 180;	pData[18].y = 6.00E-5;
		//pData[19].x = 190;	pData[19].y = 7.70E-5;
		//pData[20].x = 200;	pData[20].y = 9.60E-5;
		//pData[21].x = 210;	pData[21].y = 1.40E-4;

	// LePix Laser Scan
	// ----------------
	
		// Define labels.
		std::stringstream gTitle("Pippo");
		std::stringstream gXAxis("Position [um]");
		std::stringstream gYAxis("Signal [ADC counts]");
		
		// Set graph limits.
		pCount = 15;
		pMin.x = -5;		pMin.y = 0;
		pMax.x = 65;		pMax.y = 600;
	
		// Set data points.
		pData[0].x = -5;	pData[0].y = 477;
		pData[1].x = 0;		pData[1].y = 516;
		pData[2].x = 5;		pData[2].y = 498;
		pData[3].x = 10;	pData[3].y = 390;
		pData[4].x = 15;	pData[4].y = 229;
		pData[5].x = 20;	pData[5].y = 114;
		pData[6].x = 25;	pData[6].y = 87;
		pData[7].x = 30;	pData[7].y = 108;
		pData[8].x = 35;	pData[8].y = 211;
		pData[9].x = 40;	pData[9].y = 420;
		pData[10].x = 45;	pData[10].y = 516;
		pData[11].x = 50;	pData[11].y = 553;
		pData[12].x = 55;	pData[12].y = 505;
		pData[13].x = 60;	pData[13].y = 393;
		pData[14].x = 65;	pData[14].y = 215;
	

	// Create a general purpose graph.
	TGraph* graph = new TGraph();
	
	// Creates the forms where to show the graphs.
//	MA._Form.push_back(new race::ui::Form("Plot name here", 1280, 1024));  
//	MA._Form.push_back(new race::ui::Form("Plot name here", 1280, 1024));
			
	// Creates the pad for the secondary X axis (in case).
	TPad *padA = new TPad("padA", "", 0.0, 0.0, 1.0, 1.0);
	padA->SetFillStyle(4000); // Transparent.
	padA->Draw();	   
	
	// Creates the main graph pad.
	TPad* padG = new TPad("padG", "", 0, 0.1, 1, 1);
	padG->SetFillStyle(4000); // Transparent.
	padG->SetGrid(1, 1);
	padG->Draw();
	padG->cd();
		
	// Fill the graph.
	for (int i = 0; i < pCount; i++) {
		graph->SetPoint(i, pData[i].x, pData[i].y);
	}

	// Main Plot.
	graph->SetTitle(gTitle.str().c_str());
	
	// X-Axis.
	graph->GetXaxis()->SetTitle(gXAxis.str().c_str());
	graph->GetXaxis()->SetLimits(pMin.x, pMax.x);
	graph->GetXaxis()->SetLabelSize(0.04f); 
	graph->GetXaxis()->SetTitleSize(0.05f);
	graph->GetXaxis()->SetTitleOffset(1.1f);

	// Y-Axis
	graph->GetYaxis()->SetTitle(gYAxis.str().c_str());
	graph->GetYaxis()->SetRangeUser(pMin.y, pMax.y);
	graph->GetYaxis()->SetLabelSize(0.04f);
	graph->GetYaxis()->SetTitleSize(0.05f);
	graph->GetYaxis()->SetTitleOffset(1.2f); 
	
	// Draw the graph.
	graph->SetMarkerSize(5);
 	graph->SetMarkerColor(5);
	graph->SetLineWidth(2); 
	graph->Draw("ALP");
			
	// Draw the hits/frame additional axis.
	//_MA._Form.back()->Canvas()->cd();
//	padA->cd();
//	padA->Range(-0.125, 0, 1.125, 1);
//	TGaxis* axis = new TGaxis(0.0, 0.1, 1, 0.1, _rFrameHitsAvg[0], _rFrameHitsAvg[_MA._Sim.hitsCount - 1], 10, "");
//	axis->SetLabelSize(0.03f);
//	axis->SetLabelColor(kBlue);
//	axis->SetLabelFont(2);
//	axis->SetTitleColor(kBlue);
//	axis->SetTitleSize(0.03f);
//	axis->SetTitle("[hits/frame]");
//	axis->Draw("LP");
}


// #############################################################################
} // CLose namespaces.