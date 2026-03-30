module;

#include <random>
#include <vector>
#include <map>

export module makga.math.probability;

export namespace makga::math {

// 확률 분포 클래스
export class ProbabilityDistribution
{
public:
    ProbabilityDistribution();
    ~ProbabilityDistribution() = default;

    // 균등 분포 (0.0 ~ 1.0)
    float UniformRandom();
    
    // 범위 내 균등 분포
    float UniformRange(float min, float max);
    
    // 정수 범위 균등 분포
    int UniformInt(int min, int max);
    
    // 정규 분포 (가우시안)
    float NormalDistribution(float mean, float stddev);
    
    // 지수 분포
    float ExponentialDistribution(float lambda);
    
    // 이항 분포
    int BinomialDistribution(int trials, float probability);
    
    // 베르누이 시행 (참/거짓)
    bool BernoulliTrial(float probability);
    
    // 가중치 기반 선택
    int WeightedChoice(const std::vector<float>& weights);

private:
    std::random_device rd_;
    std::mt19937 gen_;
};

// 통계 계산 클래스
export class Statistics
{
public:
    // 평균
    static float Mean(const std::vector<float>& data);
    
    // 중앙값
    static float Median(std::vector<float> data);
    
    // 분산
    static float Variance(const std::vector<float>& data);
    
    // 표준편차
    static float StandardDeviation(const std::vector<float>& data);
    
    // 최소값
    static float Min(const std::vector<float>& data);
    
    // 최대값
    static float Max(const std::vector<float>& data);
    
    // 범위
    static float Range(const std::vector<float>& data);
    
    // 백분위수
    static float Percentile(std::vector<float> data, float percentile);
};

// 확률 테이블 (룻 테이블)
export class ProbabilityTable
{
public:
    // 아이템 추가
    void AddItem(int id, float weight);
    
    // 가중치 기반 랜덤 선택
    int SelectRandom();
    
    // 테이블 초기화
    void Clear();
    
    // 총 가중치
    float GetTotalWeight() const;

private:
    std::map<int, float> items_;
    ProbabilityDistribution dist_;
};

} // namespace makga::math
