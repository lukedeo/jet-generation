#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "TError.h"
#include "TParticle.h"
#include "TString.h"
#include "TSystem.h"

#include "fastjet/ClusterSequence.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"

#include "Pythia8/Pythia.h"

#include "JetImageBuffer.h"
#include "JetImageProperties.h"

#include "parser.hh"

#include "spdlog/spdlog.h"

int get_seed(int seed) {
    if (seed > -1) {
        return seed;
    }
    int timeSeed = time(NULL);
    return abs(((timeSeed * 181) * ((getpid() - 83) * 359)) % 104729);
}

int main(int argc, const char *argv[]) {

    spdlog::set_level(spdlog::level::info);
    // argument parsing  ------------------------
    std::cout << "Called as: ";

    for (int ii = 0; ii < argc; ++ii) {
        std::cout << argv[ii] << " ";
    }
    std::cout << std::endl;

    // agruments
    std::string outName = "Mediator.root";
    int pileup = 0;
    int nb_events = 0;
    int pixels = 25;
    bool fDebug = false;
    float pThatmin = 100;
    float pThatmax = 500;
    float boson_mass = 1500;
    float image_range = 1.25;
    int proc = 1;
    int seed = -1;

    optionparser::parser parser("Allowed options");

    parser.add_option("--nb-events", "-n")
        .mode(optionparser::store_value)
        .default_value(10)
        .help("Number of Events");
    parser.add_option("--nb-pixels")
        .mode(optionparser::store_value)
        .default_value(25)
        .help("Number of pixels per dimension");
    parser.add_option("--image-range", "-R")
        .mode(optionparser::store_value)
        .default_value(1.25)
        .help("Image captures [-w, w] x [-w, w], where w is the value passed.");
    parser.add_option("--debug")
        .mode(optionparser::store_true)
        .help("Debug flag");
    parser.add_option("--pileup")
        .mode(optionparser::store_value)
        .default_value(0)
        .help("Number of Additional Interactions");
    parser.add_option("--out-file", "-o")
        .mode(optionparser::store_value)
        .default_value("test.root")
        .help("output file name");
    parser.add_option("--process", "-p")
        .mode(optionparser::store_value)
        .default_value(2)
        .help("Process: 1=ZprimeTottbar, 2=WprimeToWZ_lept, 3=WprimeToWZ_had, "
              "4=QCD");
    parser.add_option("--seed")
        .mode(optionparser::store_value)
        .default_value(-1)
        .help("seed. -1 means random seed");
    parser.add_option("--pt-hat-min")
        .mode(optionparser::store_value)
        .default_value(100)
        .help("pThat Min for QCD");
    parser.add_option("--pt-hat-max")
        .mode(optionparser::store_value)
        .default_value(500)
        .help("pThat Max for QCD");
    parser.add_option("--boson-mass")
        .mode(optionparser::store_value)
        .default_value(800)
        .help("Z' or W' mass in GeV");

    parser.eat_arguments(argc, argv);

    nb_events = parser.get_value<int>("nbevents");
    pixels = parser.get_value<int>("nbpixels");
    image_range = parser.get_value<float>("imagerange");
    fDebug = parser.get_value("debug");
    pileup = parser.get_value<int>("pileup");
    outName = parser.get_value<std::string>("outfile");
    proc = parser.get_value<int>("process");
    seed = parser.get_value<int>("seed");
    pThatmin = parser.get_value<float>("pthatmin");
    pThatmax = parser.get_value<float>("pthatmax");
    boson_mass = parser.get_value<float>("bosonmass");

    // seed
    seed = get_seed(seed);

    auto pythia_xml = "../share/Pythia8/xmldoc";

    // Configure and initialize pythia
    Pythia8::Pythia *pythia8 = new Pythia8::Pythia(pythia_xml, false);

    pythia8->readString("Init:showProcesses = off");
    pythia8->readString("Init:showMultipartonInteractions = off");
    pythia8->readString("Init:showChangedSettings = off");
    pythia8->readString("Init:showChangedParticleData = off");

    
    pythia8->readString("Random:setSeed = on");
    std::stringstream ss;
    ss << "Random:seed = " << seed;
    pythia8->readString(ss.str());

    pythia8->readString("Next:numberShowInfo = 0");
    pythia8->readString("Next:numberShowEvent = 0");
    pythia8->readString("Next:numberShowLHA = 0");
    pythia8->readString("Next:numberShowProcess = 0");
    pythia8->readString("Next:numberCount = 0");

    if (proc == 1) {
        std::stringstream bosonmass_str;
        bosonmass_str << "32:m0=" << boson_mass;
        pythia8->readString(bosonmass_str.str());
        pythia8->readString("NewGaugeBoson:ffbar2gmZZprime= on");
        pythia8->readString("Zprime:gmZmode=3");
        pythia8->readString("32:onMode = off");
        pythia8->readString("32:onIfAny = 6");
        pythia8->readString("24:onMode = off");
        pythia8->readString("24:onIfAny = 1 2 3 4");
        pythia8->init();
    } else if (proc == 2) {
        std::stringstream bosonmass_str;
        bosonmass_str << "34:m0=" << boson_mass;
        pythia8->readString(bosonmass_str.str());
        pythia8->readString("NewGaugeBoson:ffbar2Wprime = on");
        pythia8->readString("Wprime:coup2WZ=1");
        pythia8->readString("34:onMode = off");
        pythia8->readString("34:onIfAny = 23 24");
        pythia8->readString("24:onMode = off");
        pythia8->readString("24:onIfAny = 1 2 3 4");
        pythia8->readString("23:onMode = off");
        pythia8->readString("23:onIfAny = 12");
        pythia8->init();
    } else if (proc == 3) {
        std::stringstream bosonmass_str;
        bosonmass_str << "34:m0=" << boson_mass;
        pythia8->readString(bosonmass_str.str());
        pythia8->readString("NewGaugeBoson:ffbar2Wprime = on");
        pythia8->readString("Wprime:coup2WZ=1");
        pythia8->readString("34:onMode = off");
        pythia8->readString("34:onIfAny = 23 24");
        pythia8->readString("24:onMode = off");
        pythia8->readString("24:onIfAny = 11 12");
        pythia8->readString("23:onMode = off");
        pythia8->readString("23:onIfAny = 1 2 3 4 5");
        pythia8->init();
    } else if (proc == 4) {
        pythia8->readString("HardQCD:all = on");
        std::stringstream ptHatMin;
        std::stringstream ptHatMax;
        ptHatMin << "PhaseSpace:pTHatMin  =" << pThatmin;
        ptHatMax << "PhaseSpace:pTHatMax  =" << pThatmax;
        pythia8->readString(ptHatMin.str());
        pythia8->readString(ptHatMax.str());
        pythia8->init();
    } else {
        throw std::invalid_argument("received invalid 'process'");
    }

    // Setup the pileup
    Pythia8::Pythia *pythia_MB = new Pythia8::Pythia(pythia_xml, false);
    pythia_MB->readString("Random:setSeed = on");
    pythia_MB->readString("Init:showProcesses = off");
    pythia_MB->readString("Init:showMultipartonInteractions = off");
    pythia_MB->readString("Init:showChangedSettings = off");
    pythia_MB->readString("Init:showChangedParticleData = off");
    
    ss.clear();
    ss.str("");
    ss << "Random:seed = " << seed + 1;
    pythia_MB->readString(ss.str());
    pythia_MB->readString("SoftQCD:nonDiffractive = on");
    pythia_MB->readString("HardQCD:all = off");
    pythia_MB->readString("PhaseSpace:pTHatMin  = .1");
    pythia_MB->readString("PhaseSpace:pTHatMax  = 20000");
    pythia_MB->init();

    JetImageBuffer *analysis = new JetImageBuffer(pixels, fDebug);
    // analysis->Debug(fDebug);
    analysis->SetOutName(outName);
    analysis->Begin();
    

    // Event loop
    for (int iev = 0; iev < nb_events; iev++) {
        analysis->AnalyzeEvent(iev, pythia8, pythia_MB, pileup, pixels,
                               image_range);
    }

    analysis->End();

    // that was it
    delete pythia8;
    delete analysis;

    return 0;
}
