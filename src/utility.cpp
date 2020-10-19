/*
 * utility.cpp
 *
 *  Created on: Aug 22, 2012
 *      Author: rudra
 */

#include <iostream>
#include <string>
#include <sstream>
#include "utility.h"
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include <iterator>
using namespace std;

namespace myLibrary
{
		int readSize(const char* file)
		{
			char temp[1024];
			strcpy(temp,"wc -l ");
			strcat(temp,file);

			//string temp = "wc -l "+file;
			int no_of_examples;
			FILE *fp;
			fp= popen(temp, "r");
			if(fp == NULL)
			{
				cout << "Error opening file " << file << endl;
				exit(1);
			}
			char buffer[128];
			string temp2,temp3;
			if (fgets(buffer, sizeof(buffer), fp))
			{
				string temp1(buffer);
				temp2 = temp1;
			}

			if (pclose (fp) != 0)
			{
			        fprintf (stderr, "Unknown error.\n");
			        exit(1);
			}

			for (unsigned int i=0; i < temp2.size(); i++)
			{
				//iterate the string to find the first "number" character
				//if found create another loop to extract it
				//and then break the current one
				//thus extracting the FIRST encountered numeric block
				if (isdigit(temp2[i]))
				{
					for (unsigned int a=i; a<temp2.size(); a++)
					{
						temp3 += temp2[a];
					}
				  //the first numeric block is extracted
				  break;
				}
			 }
			std::istringstream stream(temp3);
			stream >> no_of_examples;
			return no_of_examples ;
		}


		void textToBinary(const char* filename)
		{
			int numberOfLines =  myLibrary::readSize(filename);
			ifstream in;
			in.exceptions ( ifstream::failbit | ifstream::badbit );
			try
			{
				in.open(filename,ios::in);
				in.seekg(0,ios::beg);

				std::string line;

				std::vector<double> matrix;

				for(int i=0 ; i < numberOfLines ; i++)
				{
					getline(in,line);

					int j = 0;
					std::istringstream s(line);
					std::string field;

					while (getline(s, field,'\t'))
					{
						std::istringstream astream(field);
						double value;
						astream >> value;

						matrix.push_back(value);
						j++;
					}
				}

				in.close();

				char fileName[500];
				sprintf(fileName,"%s.bin",filename);

				std::ofstream out(fileName, std::ios::out | std::ofstream::binary);

				out.write(reinterpret_cast<const char *>(&matrix[0]), matrix.size()*sizeof(double));
				out.close();
			}
			catch(ifstream::failure & e)
			{
				std::cout << "Exception reading file " << filename << std::endl;
				exit(1);
			}
		}

/*
		void readExample(ifstream* in,Example* temp)
		{
			string line;

			getline(*in,line);

			temp->label = 0;
			temp->index.clear();
			temp->value.clear();

			string lab = line.substr(0,2);
			std::istringstream stream(lab);
			stream >> temp->label;

			int k = 0,i;
			while(line[k] != '\t')
			{
				k++;
			}
			k++;

			int index = 1;
			double value;
			string substring;
			while(line[k] != '\0')
			{
				i = k;
				while(line[k] != '\t' && line[k] != '\0')
				{
					k++;
				}
				k++;

				substring = line.substr(i,(k-i));

				if(index > DataSource::dimension)
					DataSource::dimension = index;

				std::istringstream astream(substring);
				astream >> value;

				if(value > DataSource::max)
					DataSource::max = value;

				temp->index.push_back(index);
				temp->value.push_back(value);
				index++;
			}
			double d = 0.0;
			for(unsigned int i=0; i<temp->index.size(); i++)
			{
				d += temp->value[i] * temp->value[i];
			}
			if (d > 0 && d != 1.0)
			{
				for(unsigned int i=0; i<temp->index.size(); i++)
				{
					temp->value[i] = temp->value[i]*(1.0 / sqrt(d));
				}
			}

		}
*/


		void writeExample(string file)
		{

		}
}






