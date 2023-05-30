#include "TBplotengine.h"

TBplotengine::TBplotengine(const YAML::Node fNodePlot_, int fRunNum_, TButility fUtility_) 
:fNodePlot(fNodePlot_), fRunNum(fRunNum_), fUtility(fUtility_) {

	init();
}

void TBplotengine::init() {


	fUtility.loading("/Users/khwang/scratch/TB2023July/sample/Info/mapping_Aug2022TB.root");
	fUtility.loadped("/Users/khwang/scratch/TB2023July/sample/Info/Run" + std::to_string(fRunNum) + "_pedestalHist_mean.root");

	std::cout << "starting INIT" << std::endl;

	for(const auto& aCase : fNodePlot["Cases"]) {

		std::cout << " - Loading : " << aCase.first.as<std::string>() << std::endl;

		if ( aCase.first.as<std::string>() == "DWC" ) {
			PlotDWC(aCase);
			continue;
		}

		TBcid aCid = getCid(aCase.first.as<std::string>());
		std::vector<std::pair<PlotInfo, Plotter*>> aVec;

		for(const auto& aPlot : aCase.second) {

			std::cout << " --- Generating : " << aPlot.first.as<std::string>() << "...   ";

			PlotInfo which = getPlotInfo(aPlot.first.as<std::string>());
			Plotter* plot = getPlot(aCid, which, aPlot.second);

			aVec.push_back(std::make_pair(which, plot));

			std::cout << "Done!" << std::endl;
		}

		fPlotSet.insert(std::make_pair(aCid, aVec));
	}

	fMIDtoLoad.clear();
	for ( auto aPlotter : fPlotSet ) {
		fCIDtoPlot.push_back(aPlotter.first);
		int amid = aPlotter.first.mid();
		
		bool isMatch = false;
		for ( int i = 0; i < fMIDtoLoad.size(); i++ )
			if ( fMIDtoLoad.at(i) == amid ) 
				isMatch = true;

		if ( !isMatch )
			fMIDtoLoad.push_back(amid);
	}	

	PrintInfo();
}

TBcid TBplotengine::getCid(std::string input) {
	if ( input == "Pre-shower" )
		return fUtility.getcid(TBdetector::preshower);
		// return TBcid(1,1);

	if ( input == "Muon Counter" )
		return fUtility.getcid(TBdetector::muon);
		// return TBcid(1,2);

	if ( input == "DWC" )
		return TBcid(1,3);
		// return fUtility.getcid(TBdetector::DWC1analogue);

	if ( input.find("MID") != std::string::npos && input.find("CH") != std::string::npos ) {
		int mid = std::stoi(input.substr(input.find("MID") + 3, input.find("CH") - input.find("MID") - 4));
		int ch = std::stoi(input.substr(input.find("CH") + 2, input.size() - input.find("CH") - 2));

		if ( mid > 15 || mid < 1 )
			throw std::runtime_error("TBplotengine::getCid - allowed MID {1..15}, input is : " + std::to_string(mid));

		if ( ch > 32 || ch < 1)
			throw std::runtime_error("TBplotengine::getCid - allowed CH {1..32}, input is : " + std::to_string(ch));

		return TBcid(mid, ch);
	}

}

TBplotengine::PlotInfo TBplotengine::getPlotInfo(std::string input) {
	if ( input == "average time structure" )
		return PlotInfo::kAvgTimeStruc;

	if ( input == "integrated ADC" )
		return PlotInfo::kIntADC;

	if ( input == "average time structure" )
		return PlotInfo::kPeakADC;
}	

Plotter* TBplotengine::getPlot(TBcid cid, TBplotengine::PlotInfo which, const YAML::Node node) {

	// cid.print();

	if (which == TBplotengine::PlotInfo::kAvgTimeStruc) {
		TString name = "AvgTimeStruc_Mid" + std::to_string(cid.mid()) + "Ch" + std::to_string(cid.channel());
		float xScale = node["xScale"].as<float>();

		std::vector<float> input;
		input.push_back(xScale);

		Plotter* aPlotter = new cAvgTimeStruc();
		aPlotter->Set(input);
		aPlotter->SetHisto(new TH1F(name, (TString)(";bins [" + std::to_string(xScale) + "ns / bin]; ADC"), 1000, 0., 1000.));

		return aPlotter;
	}

	if (which == TBplotengine::PlotInfo::kIntADC) {
		TString name = "IntADC_Mid" + std::to_string(cid.mid()) + "Ch" + std::to_string(cid.channel());
		float ped = fUtility.retrievePed(cid);
		int nBin = node["nBin"].as<int>();
		float xMin = node["xMin"].as<float>();
		float xMax = node["xMax"].as<float>();
		std::vector<int> interval = node["interval"].as<std::vector<int>>();
		
		std::vector<float> input;
		input.push_back(ped);
		input.push_back(interval.at(0));
		input.push_back(interval.at(1));

		Plotter* aPlotter = new cIntADC();
		aPlotter->Set(input);
		aPlotter->SetHisto(new TH1F(name, (TString)(";Int. ADC; N_{events}"), nBin, xMin, xMax));

		return aPlotter;
	}

	if (which == TBplotengine::PlotInfo::kPeakADC) {
		TString name = "PeakADC_Mid" + std::to_string(cid.mid()) + "Ch" + std::to_string(cid.channel());
		float ped = fUtility.retrievePed(cid);
		float nBin = node["nBin"].as<int>();
		float xMin = node["xMin"].as<float>();
		float xMax = node["xMax"].as<float>();
		std::vector<int> interval = node["interval"].as<std::vector<int>>();
		
		std::vector<float> input;
		input.push_back(ped);
		input.push_back(interval.at(0));
		input.push_back(interval.at(1));

		Plotter* aPlotter = new cPeakADC();
		aPlotter->Set(input);
		aPlotter->SetHisto(new TH1F(name, (TString)(";Peak ADC; N_{events}"), nBin, xMin, xMax));

		return aPlotter;
	}

	return new Plotter();
}

void TBplotengine::PlotDWC(const YAML::Node node) {

}

void TBplotengine::PrintInfo() {

	std::cout << " " << std::endl;
	std::cout << " ### Plots to draw ### " << std::endl;
	for (auto aSet : fPlotSet) {

		std::cout << " ------------------------------ " << std::endl;
		std::cout << " - " ;
		aSet.first.print();

		for (auto aPlot : aSet.second) {
			std::cout << " -- | " << aPlot.first << " " << aPlot.second->GetHisto()->GetName() << std::endl;
		}

	}
	std::cout << " ============================== " << std::endl;
	std::cout << " " << std::endl;
}

void TBplotengine::Fill(TBevt<TBwaveform> anEvent) {
	for ( int i = 0; i < fCIDtoPlot.size(); i++ )
		for ( int j = 0; j < fPlotSet.at(fCIDtoPlot.at(i)).size(); j++ )
			fPlotSet.at(fCIDtoPlot.at(i)).at(j).second->Fill(anEvent.GetData(fCIDtoPlot.at(i)).waveform());

}

void TBplotengine::SaveAs(TString output) {
	
	TFile* outoutFile = new TFile(output, "RECREATE");
	
	outoutFile->cd();
	for ( int i = 0; i < fCIDtoPlot.size(); i++ )
		for ( int j = 0; j < fPlotSet.at(fCIDtoPlot.at(i)).size(); j++ )
			fPlotSet.at(fCIDtoPlot.at(i)).at(j).second->GetHisto()->Write();

	outoutFile->Close();
}