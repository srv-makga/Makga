#include "pch.h"
#include "CppUnitTest.h"
#include <vector>

import makga.math.probability;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(MathProbability)
	{
	public:
		TEST_METHOD(UniformRandomTest)
		{
			try
			{
				makga::math::ProbabilityDistribution dist;
				
				float value = dist.UniformRandom();
				Assert::IsTrue(value >= 0.0f && value <= 1.0f, L"Uniform random should be between 0 and 1");
			}
			catch (...)
			{
				Assert::Fail(L"UniformRandomTest exception");
			}
		}

		TEST_METHOD(UniformRangeTest)
		{
			try
			{
				makga::math::ProbabilityDistribution dist;
				
				float min = 10.0f;
				float max = 20.0f;
				float value = dist.UniformRange(min, max);
				Assert::IsTrue(value >= min && value <= max, L"Value should be within specified range");
			}
			catch (...)
			{
				Assert::Fail(L"UniformRangeTest exception");
			}
		}

		TEST_METHOD(UniformIntTest)
		{
			try
			{
				makga::math::ProbabilityDistribution dist;
				
				int min = 1;
				int max = 10;
				int value = dist.UniformInt(min, max);
				Assert::IsTrue(value >= min && value <= max, L"Integer should be within specified range");
			}
			catch (...)
			{
				Assert::Fail(L"UniformIntTest exception");
			}
		}

		TEST_METHOD(BernoulliTrialTest)
		{
			try
			{
				makga::math::ProbabilityDistribution dist;
				
				// Test with 100% probability
				bool alwaysTrue = dist.BernoulliTrial(1.0f);
				Assert::IsTrue(alwaysTrue, L"Bernoulli with p=1.0 should always return true");
				
				// Test with 0% probability
				bool alwaysFalse = dist.BernoulliTrial(0.0f);
				Assert::IsFalse(alwaysFalse, L"Bernoulli with p=0.0 should always return false");
			}
			catch (...)
			{
				Assert::Fail(L"BernoulliTrialTest exception");
			}
		}

		TEST_METHOD(WeightedChoiceTest)
		{
			try
			{
				makga::math::ProbabilityDistribution dist;
				std::vector<float> weights = { 1.0f, 2.0f, 3.0f, 4.0f };
				
				int choice = dist.WeightedChoice(weights);
				Assert::IsTrue(choice >= 0 && choice < static_cast<int>(weights.size()), 
					L"Choice should be within valid range");
			}
			catch (...)
			{
				Assert::Fail(L"WeightedChoiceTest exception");
			}
		}

		TEST_METHOD(StatisticsMeanTest)
		{
			try
			{
				std::vector<float> data = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f };
				float mean = makga::math::Statistics::Mean(data);
				Assert::AreEqual(3.0f, mean, 0.01f, L"Mean of 1,2,3,4,5 should be 3.0");
			}
			catch (...)
			{
				Assert::Fail(L"StatisticsMeanTest exception");
			}
		}

		TEST_METHOD(StatisticsMedianTest)
		{
			try
			{
				std::vector<float> data = { 1.0f, 3.0f, 2.0f, 5.0f, 4.0f };
				float median = makga::math::Statistics::Median(data);
				Assert::AreEqual(3.0f, median, 0.01f, L"Median should be 3.0");
			}
			catch (...)
			{
				Assert::Fail(L"StatisticsMedianTest exception");
			}
		}

		TEST_METHOD(StatisticsMinMaxTest)
		{
			try
			{
				std::vector<float> data = { 5.0f, 2.0f, 8.0f, 1.0f, 9.0f };
				
				float min = makga::math::Statistics::Min(data);
				float max = makga::math::Statistics::Max(data);
				float range = makga::math::Statistics::Range(data);
				
				Assert::AreEqual(1.0f, min, 0.01f, L"Min should be 1.0");
				Assert::AreEqual(9.0f, max, 0.01f, L"Max should be 9.0");
				Assert::AreEqual(8.0f, range, 0.01f, L"Range should be 8.0");
			}
			catch (...)
			{
				Assert::Fail(L"StatisticsMinMaxTest exception");
			}
		}

		TEST_METHOD(StandardDeviationTest)
		{
			try
			{
				std::vector<float> data = { 2.0f, 4.0f, 4.0f, 4.0f, 5.0f, 5.0f, 7.0f, 9.0f };
				
				float variance = makga::math::Statistics::Variance(data);
				float stddev = makga::math::Statistics::StandardDeviation(data);
				
				Assert::IsTrue(variance > 0.0f, L"Variance should be positive");
				Assert::IsTrue(stddev > 0.0f, L"Standard deviation should be positive");
			}
			catch (...)
			{
				Assert::Fail(L"StandardDeviationTest exception");
			}
		}

		TEST_METHOD(ProbabilityTableTest)
		{
			try
			{
				makga::math::ProbabilityTable table;
				
				table.AddItem(1, 1.0f);
				table.AddItem(2, 2.0f);
				table.AddItem(3, 3.0f);
				
				float totalWeight = table.GetTotalWeight();
				Assert::AreEqual(6.0f, totalWeight, 0.01f, L"Total weight should be 6.0");
				
				int selected = table.SelectRandom();
				Assert::IsTrue(selected >= 1 && selected <= 3, L"Selected item should be valid");
				
				table.Clear();
				float emptyWeight = table.GetTotalWeight();
				Assert::AreEqual(0.0f, emptyWeight, 0.01f, L"Weight should be 0 after clear");
			}
			catch (...)
			{
				Assert::Fail(L"ProbabilityTableTest exception");
			}
		}
	};
}
