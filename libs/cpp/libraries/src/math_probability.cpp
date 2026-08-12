module;

#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

module makga.math.probability;

namespace makga::math {

// ProbabilityDistribution 구현
ProbabilityDistribution::ProbabilityDistribution()
    : gen_(rd_())
{
}

float ProbabilityDistribution::UniformRandom()
{
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(gen_);
}

float ProbabilityDistribution::UniformRange(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen_);
}

int ProbabilityDistribution::UniformInt(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen_);
}

float ProbabilityDistribution::NormalDistribution(float mean, float stddev)
{
    std::normal_distribution<float> dist(mean, stddev);
    return dist(gen_);
}

float ProbabilityDistribution::ExponentialDistribution(float lambda)
{
    std::exponential_distribution<float> dist(lambda);
    return dist(gen_);
}

int ProbabilityDistribution::BinomialDistribution(int trials, float probability)
{
    std::binomial_distribution<int> dist(trials, probability);
    return dist(gen_);
}

bool ProbabilityDistribution::BernoulliTrial(float probability)
{
    std::bernoulli_distribution dist(probability);
    return dist(gen_);
}

int ProbabilityDistribution::WeightedChoice(const std::vector<float>& weights)
{
    std::discrete_distribution<int> dist(weights.begin(), weights.end());
    return dist(gen_);
}

// Statistics 구현
float Statistics::Mean(const std::vector<float>& data)
{
    if (data.empty()) return 0.0f;
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    return sum / data.size();
}

float Statistics::Median(std::vector<float> data)
{
    if (data.empty()) return 0.0f;
    
    std::sort(data.begin(), data.end());
    size_t n = data.size();
    
    if (n % 2 == 0)
        return (data[n/2 - 1] + data[n/2]) / 2.0f;
    else
        return data[n/2];
}

float Statistics::Variance(const std::vector<float>& data)
{
    if (data.empty()) return 0.0f;
    
    float mean = Mean(data);
    float sum_squared_diff = 0.0f;
    
    for (float value : data) {
        float diff = value - mean;
        sum_squared_diff += diff * diff;
    }
    
    return sum_squared_diff / data.size();
}

float Statistics::StandardDeviation(const std::vector<float>& data)
{
    return std::sqrt(Variance(data));
}

float Statistics::Min(const std::vector<float>& data)
{
    if (data.empty()) return 0.0f;
    return *std::min_element(data.begin(), data.end());
}

float Statistics::Max(const std::vector<float>& data)
{
    if (data.empty()) return 0.0f;
    return *std::max_element(data.begin(), data.end());
}

float Statistics::Range(const std::vector<float>& data)
{
    if (data.empty()) return 0.0f;
    return Max(data) - Min(data);
}

float Statistics::Percentile(std::vector<float> data, float percentile)
{
    if (data.empty()) return 0.0f;
    
    std::sort(data.begin(), data.end());
    float index = (percentile / 100.0f) * (data.size() - 1);
    size_t lower = static_cast<size_t>(std::floor(index));
    size_t upper = static_cast<size_t>(std::ceil(index));
    
    if (lower == upper)
        return data[lower];
    
    float weight = index - lower;
    return data[lower] * (1.0f - weight) + data[upper] * weight;
}

// ProbabilityTable 구현
void ProbabilityTable::AddItem(int id, float weight)
{
    items_[id] = weight;
}

int ProbabilityTable::SelectRandom()
{
    if (items_.empty()) return -1;
    
    std::vector<float> weights;
    std::vector<int> ids;
    
    for (const auto& [id, weight] : items_) {
        ids.push_back(id);
        weights.push_back(weight);
    }
    
    int index = dist_.WeightedChoice(weights);
    return ids[index];
}

void ProbabilityTable::Clear()
{
    items_.clear();
}

float ProbabilityTable::GetTotalWeight() const
{
    float total = 0.0f;
    for (const auto& [id, weight] : items_) {
        total += weight;
    }
    return total;
}

} // namespace makga::math
