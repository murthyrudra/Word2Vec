/*
 * WordEmbeddingCbowNS.h
 *
 *  Created on: Feb 21, 2015
 *      Author: rudramurthy
 */

#ifndef WORDEMBEDDINGCBOWNS_H_
#define WORDEMBEDDINGCBOWNS_H_

#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <boost/regex.hpp>

namespace neuralNetwork
{
	class WordEmbeddingCbowNS
	{
		std::unordered_map<std::string,long> dictionary;
		std::unordered_map<long,long> dictionaryHistogram;

		std::unordered_map<std::string,long> prunedDictionary;
		std::vector<std::string> prunedDictionaryReverse;
		std::unordered_map<long, long> prunedUnigramCount;

		std::unordered_map<std::string, int> stopWords;

		std::vector<long> unigramTable;

		std::string eow = "<EOW>";

		int dimension;
		int nGramWindow;
		int numberOfPaddings;

		int negativeSamples;
		int maxEpoch;

		long long totalNumberOfWords;
		long long totalNumberOflines;

		std::vector<double> weights;
		std::vector<double> conceptWeights;

		double eta;
		double starting_eta;
		double regularizationPenalty;
		double momentum;

		std::string unknownWord;
		std::string sentenceDelimiter;

		char trainFilename[256];
		int minCount;

		public:
			WordEmbeddingCbowNS();
			virtual ~WordEmbeddingCbowNS();

			void initializeVocabulary(char* wordFile);
			void initializeStopWords(char* wordFile);
			void pruneDictionary();

			char static easytolower(char in)
				{
					if(in<='Z' && in>='A')
					    return in-('Z'-'z');
					  return in;
				}

			void train();

			void trainModel(const std::vector<long> indices);

			void trainModelSkipGram(const std::vector<long> indices);

			void trainSkipGramSyllables(const std::vector<long> wordIndices, std::vector<std::vector<long>> charIndex);

			void initUnigramProbabilityTable();

			void dumpWordVectors(const char* filename);
			void dumpWordVectorsSG(const char* filename);

			void dumpWordVectorsBinary(const char* filename);

			void saveVocabulary(const char* filename);

			void dumpWordVectorsVocab(const char* filename);

			void interactiveNearestNeighbours();

			void dumpParameters(char* fileName);
			void readParameters(char* fileName);

	int getMinCount() const
	{
		return minCount;
	}

	void setMinCount(int minCount)
	{
		this->minCount = minCount;
	}

	void setTrainFileName(char* fileCorpus)
	{
		sprintf(trainFilename,fileCorpus);
	}

	int getDimension() const
	{
		return dimension;
	}

	void setDimension(int dimension)
	{
		this->dimension = dimension;
	}

	void initializeParameters()
	{
		weights.resize(dimension * prunedDictionary.size());

		//Initializing weights randomly
		double temp = (1.0)/ (dimension  * 1.0);

		std::uniform_real_distribution<> distr(-temp, temp); // define the range
		std::random_device rd; // obtain a random number from hardware
		std::mt19937 engine(rd()); // seed the generator

		auto generator = std::bind(distr, engine);
		std::generate_n(weights.begin(), dimension * prunedDictionary.size() , generator);

		conceptWeights.resize( dimension * prunedDictionary.size());
		fill(conceptWeights.begin(), conceptWeights.end(), 0.0);
	}

	int getGram() const
	{
		return nGramWindow;
	}

	void setGram(int gram)
	{
		nGramWindow = gram;
		numberOfPaddings  = nGramWindow / 2;
	}

	int getNegativeSamples() const
	{
		return negativeSamples;
	}

	void setNegativeSamples(int negativeSamples)
	{
		this->negativeSamples = negativeSamples;
	}

	int getGramWindow() const {
		return nGramWindow;
	}

	void setGramWindow(int gramWindow) {
		nGramWindow = gramWindow;
	}

	int getMaxEpoch() const {
		return maxEpoch;
	}

	void setMaxEpoch(int maxEpoch)
	{
		this->maxEpoch = maxEpoch;
	}

	double getStartingEta() const {
		return starting_eta;
	}

	void setStartingEta(double startingEta) {
		starting_eta = startingEta;
	}
};

} /* namespace neuralNetwork */

#endif /* WORDEMBEDDINGCBOWNS_H_ */
