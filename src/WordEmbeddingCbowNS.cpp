/*
 * WordEmbeddingCbowNS.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: rudramurthy
 */

#include "WordEmbeddingCbowNS.h"
#include <string>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <boost/regex.hpp>
#include <stdio.h>
#include <functional>
#include <math.h>
extern "C"
{
		#include "gnuplot_i.h"
}

namespace neuralNetwork
{

	WordEmbeddingCbowNS::WordEmbeddingCbowNS()
	{
		minCount = 0;
		dimension = 0;

		eta = 0.05;
		starting_eta = 0.05;
		regularizationPenalty = 0.001;
		momentum = 0.09;

		unknownWord = "<UNK>";
		sentenceDelimiter = "<s>";
		nGramWindow = 5;
		numberOfPaddings  = 0;

		totalNumberOfWords = 0;

		negativeSamples = 100;
		totalNumberOflines = 0;
		maxEpoch = 1;

	}

	WordEmbeddingCbowNS::~WordEmbeddingCbowNS()
	{
		dictionary.clear();
		dictionaryHistogram.clear();

		prunedDictionary.clear();
		prunedDictionaryReverse.clear();
		prunedUnigramCount.clear();
		unigramTable.clear();
		weights.clear();
		conceptWeights.clear();
	}

	void WordEmbeddingCbowNS::initializeVocabulary(char* wordFile)
	{
		if(!wordFile)
			printf("\nCorpus filename is null\n"),exit(-1);

		std::cout << "Reading file " << wordFile << std::endl;
		sprintf(trainFilename,wordFile);

		std::ifstream file(wordFile);

		if (!file.is_open()) return;

		long index = 0;

		std::string spaces(" ");

		boost::regex leading_spaces("[[:space:]]+");

		for( std::string line; getline( file, line ); )
		{
			totalNumberOflines++;

			std::string field;
			index = 0;

			std::string cleanedString = boost::regex_replace(line,leading_spaces,spaces);

			std::stringstream s(cleanedString);

			while (getline(s, field,' '))
			{
				totalNumberOfWords++;
				std::transform(field.begin(), field.end(), field.begin(), WordEmbeddingCbowNS::easytolower);

				std::unordered_map<std::string,long>::const_iterator got = dictionary.find (field);

				if ( got == dictionary.end() )
				{
					dictionary[field] = 1;
//					dictionaryHistogram[index] = 1;
					index++;
				}
				else
				{
					dictionary[got->first] = dictionary[got->first] + 1;
				}
			}
		}
		printf("Read %ld number of words in file containing %lld number of tokens\n",dictionary.size(), totalNumberOfWords);
	}

	void WordEmbeddingCbowNS::initializeStopWords(char* wordFile)
	{
		if(!wordFile)
			printf("\nCorpus filename is null\n"),exit(-1);

		std::ifstream file(wordFile);

		if (!file.is_open()) return;

		long index = 0;

		for( std::string line; getline( file, line ); )
		{
			std::string field;
			index = 0;

			std::stringstream s(line);

			while (getline(s, field,' '))
			{
				std::transform(field.begin(), field.end(), field.begin(), WordEmbeddingCbowNS::easytolower);

				std::unordered_map<std::string,int>::const_iterator got = stopWords.find (field);

				if ( got == stopWords.end() )
				{
					stopWords[field] = index;
					index++;
				}
			}
		}
		printf("Stop words initialized\n");
	}


	void WordEmbeddingCbowNS::pruneDictionary()
	{
		long index = 0;

		std::multimap<long,std::string> frequencySortedMap;

		for( auto it = dictionary.begin(); it != dictionary.end(); ++it )
		{
			if(dictionary[it->first] > minCount)
			{
				frequencySortedMap.insert(std::pair<long,std::string>(dictionary[it->first],it->first));
			}
		}

//		dictionary.clear();
		dictionaryHistogram.clear();
		prunedDictionary.clear();
		prunedDictionaryReverse.clear();
		prunedUnigramCount.clear();

		prunedDictionary[sentenceDelimiter] = index;
		prunedDictionaryReverse.push_back(sentenceDelimiter);
		prunedUnigramCount[index] = totalNumberOflines;
		index++;

		for (std::multimap<long,std::string>::reverse_iterator it=frequencySortedMap.rbegin(); it!=frequencySortedMap.rend(); ++it)
		{
			prunedDictionary[it->second] = index;
		    prunedDictionaryReverse.push_back(it->second);
		    prunedUnigramCount[index] = it->first;

		    index++;
		}

		frequencySortedMap.clear();

		printf("Pruned to %ld number of words and %ld unigramCount size \n", prunedDictionary.size(), prunedUnigramCount.size());
	}

	void WordEmbeddingCbowNS::initUnigramProbabilityTable()
	{
		unigramTable.resize(1e8);
		fill(unigramTable.begin(), unigramTable.end(), 0);

		unsigned int a, i = 0;
		long long train_words_pow = 0;

		double d1, power = 0.75;

		for(std::vector<std::string>::iterator it = prunedDictionaryReverse.begin() ; it != prunedDictionaryReverse.end(); it++)
		{
			auto pos = std::distance(prunedDictionaryReverse.begin(), it);

			train_words_pow += pow(prunedUnigramCount[pos], power);
		}

		//do sum of word_count to the power of 0.75

		//probability of number_of_lines i.e,
		d1 = pow(prunedUnigramCount[0], power) / (train_words_pow * 1.0);

		for (a = 0; a < unigramTable.size(); a++)
		{
			unigramTable[a] = i;
			//if current index/table_size is greater than probability of number_of_lines
			if ((a * 1.0) / (unigramTable.size() * 1.0) > d1)
			{
				i++;
				d1 += pow(prunedUnigramCount[i], power) / (train_words_pow * 1.0);
			}

			if (i >= prunedUnigramCount.size())
				i = prunedUnigramCount.size() - 1;
		}
	}

	void WordEmbeddingCbowNS::train()
	{
		printf("Beginning training\n");

		long long totalWordCount = 0;
		long now_word_count = 0;

		std::cout << "Running " << maxEpoch << " number of iterations" << std::endl;

		clock_t start = clock();

		for(int epoch = 0; epoch < maxEpoch; epoch++)
		{
			std::ifstream file(trainFilename);

			if (!file.is_open()) return;

			std::string word;

			std::string spaces(" ");

			boost::regex leading_spaces("[[:space:]]+");
			boost::regex punctuations("[[:space:]]+[[:punct:]]+[[:space:]]+");

			std::default_random_engine numberGenerator;

			std::bernoulli_distribution distributionBernoulli(0.5);

			long lineCount = 0;
			long long wordCount = 0;
			long long last_word_count = 0;

			 clock_t now;


			for( std::string line; getline( file, line ); )
			{
				lineCount++;

//				if(lineCount % 1000 == 0)
				if((wordCount - last_word_count) > 1000)
				{
					now=clock();
					now_word_count = wordCount - last_word_count;
					last_word_count = wordCount;

					printf("%cAlpha: %f  Progress: %.2f%% Words/sec: %.4fk   ", 13, eta,
							wordCount / (double)(totalNumberOfWords + 1) * 100, (now_word_count * 1.0) / ((float)(now - start + 1) / (float)CLOCKS_PER_SEC * 1000));
					fflush(stdout);

					double progress = double(totalWordCount) / (double)(totalNumberOfWords * (maxEpoch) + 1);

					eta = starting_eta * (1 - progress);
					if (eta < starting_eta * 0.0001)
						eta = starting_eta * 0.0001;
				}


				std::string field;

				std::vector<long> wordIndices;

				std::vector<std::vector<long>> charIndices;

				std::string cleanedString = boost::regex_replace(line,leading_spaces,spaces);

				std::stringstream s(cleanedString);

				int i = 0;

//				for every word in the line
				while (getline(s, field,' '))
				{
//					convert to lowercase
					std::transform(field.begin(), field.end(), field.begin(), WordEmbeddingCbowNS::easytolower);

//					check if the word is present in d vocabulary
					std::unordered_map<std::string,long>::const_iterator got = prunedDictionary.find (field);

					wordCount ++;
					totalWordCount++;

					if ( got == prunedDictionary.end() )
					{
						continue;
					}
					else
					{
						wordIndices.push_back(prunedDictionary[got->first]);
					}
					i++;
				}

				if(i == 0)
					continue;

				trainModelSkipGram(wordIndices);

				wordIndices.clear();
			}
			file.close();
		}
	}

	void WordEmbeddingCbowNS::trainModelSkipGram(const std::vector<long> indices)
	{
		int label;

		unsigned long long next_random = (long long)10000l;
		std::minstd_rand rng(1);

//		we have a list of word indices
		for(unsigned int current_word = 0; current_word < indices.size(); current_word++)
		{
			//for each word in the context of current word
			long l1 = indices[current_word] * dimension;

			std::vector<double> currentWordError;
			currentWordError.resize(dimension);
			fill(currentWordError.begin(), currentWordError.end(), 0.0);

			std::vector<double> currentWordEmbed;
			currentWordEmbed.resize(dimension);
			fill(currentWordEmbed.begin(), currentWordEmbed.end(), 0.0);

			for (int c = 0; c < dimension; c++)
				currentWordEmbed[c] = weights[l1 + c];

			std::uniform_int_distribution<> uniform(1, nGramWindow);

			int boundary = uniform(rng);

			for(unsigned int context_word = 0; context_word < (unsigned int)(2 * boundary + 1); context_word++)
			{
				if(context_word != boundary)
				{
					unsigned int pos = current_word - boundary + context_word;
					if(pos < 0 || pos >= indices.size() )
						continue;

					//Generate negative samples
					long target_index = -1;
					double out_error = 0.0;

					for(int negative_word = 0; negative_word < negativeSamples + 1; negative_word++)
					{
						if(negative_word == 0)
						{
							//first word is always the context word and it's label is 1
							label = 1;
							target_index = indices[pos] ;
						}
						else
						{
							//generate target word and it's label would be 0
							next_random = next_random * (unsigned long long)25214903917 + 11;
							target_index = unigramTable[(next_random >> 16) % unigramTable.size()];

							if (target_index == 0)
								target_index = next_random % (prunedUnigramCount.size() - 1) + 1;

							if (target_index == indices[pos])
								continue;

							label = 0;
						}

						long l2 = target_index * dimension;
						double score = 0.0;

						std::vector<double> contextWordEmbed;
						contextWordEmbed.resize(dimension);
						fill(contextWordEmbed.begin(), contextWordEmbed.end(), 0.0);

						for (int c = 0; c < dimension; c++)
							contextWordEmbed[c] = conceptWeights[c + l2];

						//calculate cosine similarity between hidden layer and target word generated
						for (int c = 0; c < dimension; c++)
							score += currentWordEmbed[c] * contextWordEmbed[c];

						double sig_score = 0.0;

						if (score < -6.0)
						{
						    sig_score = 0.0;
						}
						else if (score > 6.0)
						{
							sig_score = 1.0;
						}
						else
						{
							sig_score = (1.0 / ( 1.0 + exp(-score)));
						}

						out_error = eta * ((label) - sig_score);

						for (int c = 0; c < dimension; c++)
							currentWordError[c] += out_error * contextWordEmbed[c];

						for (int c = 0; c < dimension; c++)
							conceptWeights[c + l2] += out_error * currentWordEmbed[c] ;

						contextWordEmbed.clear();
					}
				}
			}

			//now update the hidden to input connection
			for(int i = 0; i < dimension; i++)
				weights[l1 + i] += currentWordError[i] ;

		}
	}

	void WordEmbeddingCbowNS::dumpWordVectors(const char* filename)
	{
		std::ofstream out;
		using namespace std;
		try
		{
			out.open(filename,ios::out|ios::binary);

			for( auto it = prunedDictionary.begin(); it != prunedDictionary.end(); ++it )
			{
				out << it->first ;

				for(int i = 0; i < dimension; i++)
				{
					out << " " << weights[i * prunedDictionary.size() + it->second];
				}
				out<< std::endl;
			}

			out.close();
		}
		catch(ofstream::failure & e)
		{
			cout << "Exception writing to file " << std::endl;
			exit(1);
		}
	}

	void WordEmbeddingCbowNS::dumpWordVectorsSG(const char* filename)
	{
		std::ofstream out;

		using namespace std;
		try
		{
			char outFileName[200];

			sprintf(outFileName,"%s_Vectors.txt", filename);

			out.open(outFileName,ios::out|ios::binary);
			out << prunedDictionary.size() << " " << dimension << std::endl;

			for( auto it = prunedDictionary.begin(); it != prunedDictionary.end(); ++it )
			{
				out << it->first ;

				for(int i = 0; i < dimension; i++)
				{
					out << " " << weights[dimension * it->second + i];
				}

				out<< std::endl;
			}

			out.close();
		}
		catch(ofstream::failure & e)
		{
			cout << "Exception writing to file " << std::endl;
			exit(1);
		}
	}

	void WordEmbeddingCbowNS::dumpWordVectorsBinary(const char* filename)
	{
		FILE *fo;

		fo = fopen(filename, "wb");

		// Save the word vectors
		fprintf(fo, "%lld %lld\n", prunedDictionary.size(), dimension);

		for( auto it = prunedDictionary.begin(); it != prunedDictionary.end(); ++it )
		{
			fprintf(fo, "%s ", it->first.c_str());

			for(int i = 0; i < dimension; i++)
			{
				fwrite(&weights[i * prunedDictionary.size() + it->second], sizeof(double), 1, fo);
			}
			fprintf(fo, "\n");
		}
		fclose(fo);
	}

	void WordEmbeddingCbowNS::dumpParameters(char * fileName)
	{
		std::ofstream out;
		using namespace std;
		try
		{
			char vocbFile[500];
			sprintf(vocbFile,"%s_%s",fileName,"vocabulary");

			out.open(vocbFile,ios::out|ios::binary);

			for(unsigned int i = 0; i < prunedDictionaryReverse.size(); i++)
			{
				out << prunedDictionaryReverse[i] << "\t" << i;
				out<< std::endl;
			}
			out.close();

			char weightsFile[500];
			sprintf(weightsFile,"%s_%s",fileName,"wordEmbeddings");
			std::ofstream out(weightsFile, std::ios::out | std::ofstream::binary);

			out.write(reinterpret_cast<const char *>(&weights[0]), weights.size()*sizeof(double));
			out.close();

			sprintf(weightsFile,"%s_%s",fileName,"conceptEmbeddings");
			out.clear();
			out.open(weightsFile, std::ios::out | std::ofstream::binary);

			out.write(reinterpret_cast<const char *>(&conceptWeights[0]), conceptWeights.size()*sizeof(double));
			out.close();
		}
		catch(ofstream::failure & e)
		{
			cout << "Exception writing to file " << std::endl;
			exit(1);
		}
	}

	void WordEmbeddingCbowNS::readParameters(char * fileName)
	{
		std::ofstream out;
		using namespace std;
		try
		{
			char vocbFile[500];
//			sprintf(vocbFile,"%s_%s",fileName,"vocabulary");
//
//			out.open(vocbFile,ios::out|ios::binary);
//
//			for(unsigned int i = 0; i < prunedDictionaryReverse.size(); i++)
//			{
//				out << prunedDictionaryReverse[i] << "\t" << i;
//				out<< std::endl;
//			}
//			out.close();

			char weightsFile[500];
			double f;
			sprintf(weightsFile,"%s_%s",fileName,"wordEmbeddings");

			std::ifstream INFILE(weightsFile, std::ios::in | std::ifstream::binary);
			{
				weights.clear();
				if(!INFILE)
				{
					cerr << "Cannot open the file" << weightsFile << endl;
					exit(1);
				}
				while( INFILE.read(reinterpret_cast<char *>(&f), sizeof(f)))
					weights.push_back(f);
				INFILE.close();
			}

			{
				sprintf(weightsFile,"%s_%s",fileName,"conceptEmbeddings");
				conceptWeights.clear();
				INFILE.open(weightsFile, std::ios::in | std::ifstream::binary);
				if(!INFILE)
				{
					cerr << "Cannot open the file" << weightsFile << endl;
					exit(1);
				}
				while( INFILE.read(reinterpret_cast<char *>(&f), sizeof(f)))
					conceptWeights.push_back(f);
				INFILE.close();

			}
		}
		catch(ofstream::failure & e)
		{
			cout << "Exception writing to file " << std::endl;
			exit(1);
		}
	}


	void WordEmbeddingCbowNS::saveVocabulary(const char* filename)
	{
		std::ofstream out;
		using namespace std;
		try
		{
			out.open(filename,ios::out|ios::binary);

			for(unsigned int i = 0; i < prunedDictionaryReverse.size(); i++)
			{
				out << prunedDictionaryReverse[i] << "\t" << prunedUnigramCount[i];
				out<< std::endl;
			}
			out.close();
		}
		catch(ofstream::failure & e)
		{
			cout << "Exception writing to file " << std::endl;
			exit(1);
		}
	}

} /* namespace neuralNetwork */
