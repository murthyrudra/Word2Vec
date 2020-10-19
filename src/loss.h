/*
 * loss.h
 *
 *  Created on: Jun 11, 2014
 *      Author: rudramurthy
 */

#ifndef LOSS_H_
#define LOSS_H_

#include <math.h>
namespace neuralNetwork
{

	class loss
	{
		public:
		loss();
		virtual ~loss();
		virtual double calculateLoss(double) = 0;
		virtual double calculateLoss(double, double) = 0;

		virtual double dLoss(double) = 0;
	};

	class sigmoidLoss: public loss
	{
	public:
		double calculateLoss(double val)
		{
			return (1.0 / (1.0 + exp(-val)));
		}

		double dLoss(double val)
		{
			return val * (1.0 - val);
		}

		double calculateLoss(double a, double b)
		{
			return 1.0;
		}
	};

	class squaredLoss: public loss
	{
	public:
		double calculateLoss(double val)
		{
			return val;
		}

		double calculateLoss(double val, double correct)
		{
			return (val - correct) * (val - correct);
		}

		double dLoss(double val)
		{
			return val;
		}
	};

	class coshLoss: public loss
	{
	public:
		double calculateLoss(double val)
		{
			return log(cosh(val));
		}

		double dLoss(double val)
		{
			return tanh(val);
		}

		double calculateLoss(double a, double b)
		{
			return 1.0;
		}
	};

	class crossEntropy: public loss
	{
	public:
		double calculateLoss(double val)
		{
			return val * val;
		}

		double dLoss(double val)
		{
			return val;
		}

		double calculateLoss(double a, double b)
		{
			return b * log(a);
		}
	};



} /* namespace neuralNetwork */

#endif /* LOSS_H_ */
