/*
 * P2GMain.cpp
 *
 *  Created on: Nov 21, 2014
 *      Author: rudramurthy
 */

#include <cstdlib>
#include <vector>
#include <iostream>
#include <random>
#include <math.h>
#include <algorithm>
#include "WordEmbeddingCbowNS.h"
#include <pthread.h>
#include "boost/program_options.hpp"

using namespace std;
using namespace neuralNetwork;

namespace
{
  const size_t ERROR_IN_COMMAND_LINE = 1;
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 2;

} // namespace

int main (int argc, char** argv)
{
	try
	{
		namespace po = boost::program_options;

		 po::options_description desc("Options");
		    desc.add_options()
		      ("help,h", "Print help messages")
		      ("corpus,i",po::value<std::string>(), "path to corpus file")
		      ("minCount,m", po::value<int>()->required(), "words with frequency less than mincount would be dropped")
			  ("dimension,d", po::value<int>()->required(), "dimension for word embeddings")
			  ("window,w", po::value<int>()->required(), "number of context words to consider")
			  ("negSamples,n", po::value<int>()->required(), "number of negative samples to consider")
			  ("maxEpoch,e", po::value<int>()->required(), "maximum Epoch")
			  ("learningRate,l", po::value<double>()->required(), "learning rate")
			  ("outputDirectory,o", po::value<std::string>(), "dump word vectors for these words");

		po::variables_map vm;

		try
		{
			po::store(po::parse_command_line(argc, argv, desc), vm);

			if (vm.count("help")) {
				cout << desc << "\n";
				return 1;
			}
			po::notify(vm);
		}
		catch(po::error& e)
		{
		  std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
		  std::cerr << desc << std::endl;
		  return ERROR_IN_COMMAND_LINE;
		}

		char filename[300];

		if(vm.count("corpus"))
		{
			std::string inputFile = vm["corpus"].as<std::string>();

			std::cout << inputFile << std::endl;
			strcpy(filename, inputFile.c_str());
		}

		WordEmbeddingCbowNS* cbow_ns = new WordEmbeddingCbowNS();

		cbow_ns->initializeVocabulary(filename);
		std::cout << "Pruning words with frequency less than " << vm["minCount"].as<int>() << std::endl;
		cbow_ns->setMinCount(vm["minCount"].as<int>());

		cbow_ns->pruneDictionary();

		std::cout << "Word dimension set to " << vm["dimension"].as<int>() << std::endl;
		cbow_ns->setDimension(vm["dimension"].as<int>());

		std::cout << "Context window set to " << vm["window"].as<int>() << std::endl;
		cbow_ns->setGram(vm["window"].as<int>());

		std::cout << "Negative samples per word set to " << vm["negSamples"].as<int>() << std::endl;
		cbow_ns->setNegativeSamples(vm["negSamples"].as<int>());
		cbow_ns->initUnigramProbabilityTable();

		std::cout << "Maximum number of iterations set to " << vm["maxEpoch"].as<int>() << std::endl;
		cbow_ns->setMaxEpoch(vm["maxEpoch"].as<int>());

		std::cout << "Using learning rate " << vm["learningRate"].as<double>() << std::endl;
		cbow_ns->setStartingEta(vm["learningRate"].as<double>());

		cbow_ns->initializeParameters();


		cbow_ns->train();

		cout << "Training Done" << std::endl;

		char outputFile[200];
		if(vm.count("outputDirectory"))
		{
			std::string inputFile = vm["outputDirectory"].as<std::string>();
			strcpy(outputFile, inputFile.c_str());

			cout << "Saving word vectors" << std::endl;
			cbow_ns->dumpWordVectorsSG(outputFile);
			cout << "Finished Saving word vectors" << std::endl;

			cbow_ns->dumpParameters(outputFile);
		}

		delete cbow_ns;
		return 0;

	}
	catch(std::exception& e)
	{
		std::cerr << "Unhandled Exception reached the top of main: "
			  << e.what() << ", application will now exit" << std::endl;
		return ERROR_UNHANDLED_EXCEPTION;
	}
}
