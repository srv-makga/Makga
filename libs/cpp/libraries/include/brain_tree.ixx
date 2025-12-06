module;

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>

export module makga.lib.ai;

export namespace makga::lib {
class Node
{
public:
	enum class Status
	{
		Invalid,
		Success,
		Failure,
		Running,
	};

	Node() = default;
	virtual ~Node() = default;

	virtual Status update() = 0;
	virtual void initialize() {}
	virtual void terminate(Status s) {}

	Status tick()
	{
		if (status != Status::Running) {
			initialize();
		}

		status = update();

		if (status != Status::Running) {
			terminate(status);
		}

		return status;
	}

	bool isSuccess() const { return status == Status::Success; }
	bool isFailure() const { return status == Status::Failure; }
	bool isRunning() const { return status == Status::Running; }
	bool isTerminated() const { return isSuccess() || isFailure(); }

	void reset() { status = Status::Invalid; }

	using Ptr = std::shared_ptr<Node>;

protected:
	Status status = Status::Invalid;
};

// @brief Composite Node
// @detail: 분기의 루트와 그 분기가 실행되는 바탕 규칙을 정의
class Composite : public Node
{
public:
	Composite() : it(children.begin()) {}
	virtual ~Composite() {}

	void addChild(Node::Ptr child) { children.push_back(child); }
	bool hasChildren() const { return !children.empty(); }

protected:
	std::vector<Node::Ptr> children;
	std::vector<Node::Ptr>::iterator it;
};

class Decorator : public Node
{
public:
	virtual ~Decorator() {}

	void setChild(Node::Ptr node) { child = node; }
	bool hasChild() const { return child != nullptr; }

protected:
	Node::Ptr child = nullptr;
};

/*
* Task(Node)의 의사결정을 위한 정보 저장
* 다수의 AI가 하나의 Blackboard 공유 가능 (통신)
* Task와 Data를 분리
*/
class Blackboard
{
public:
	void setBool(std::string key, bool value) { bools[key] = value; }
	bool getBool(std::string key)
	{
		if (bools.find(key) == bools.end()) {
			bools[key] = false;
		}
		return bools[key];
	}
	bool hasBool(std::string key) const { return bools.find(key) != bools.end(); }

	void setInt(std::string key, int value) { ints[key] = value; }
	int getInt(std::string key)
	{
		if (ints.find(key) == ints.end()) {
			ints[key] = 0;
		}
		return ints[key];
	}
	bool hasInt(std::string key) const { return ints.find(key) != ints.end(); }

	void setFloat(std::string key, float value) { floats[key] = value; }
	float getFloat(std::string key)
	{
		if (floats.find(key) == floats.end()) {
			floats[key] = 0.0f;
		}
		return floats[key];
	}
	bool hasFloat(std::string key) const { return floats.find(key) != floats.end(); }

	void setDouble(std::string key, double value) { doubles[key] = value; }
	double getDouble(std::string key)
	{
		if (doubles.find(key) == doubles.end()) {
			doubles[key] = 0.0f;
		}
		return doubles[key];
	}
	bool hasDouble(std::string key) const { return doubles.find(key) != doubles.end(); }

	void setString(std::string key, std::string value) { strings[key] = value; }
	std::string getString(std::string key)
	{
		if (strings.find(key) == strings.end()) {
			strings[key] = "";
		}
		return strings[key];
	}
	bool hasString(std::string key) const { return strings.find(key) != strings.end(); }

	using Ptr = std::shared_ptr<Blackboard>;

protected:
	std::unordered_map<std::string, bool> bools;
	std::unordered_map<std::string, int> ints;
	std::unordered_map<std::string, float> floats;
	std::unordered_map<std::string, double> doubles;
	std::unordered_map<std::string, std::string> strings;
};

class Leaf : public Node
{
public:
	Leaf() {}
	virtual ~Leaf() {}
	Leaf(Blackboard::Ptr blackboard) : blackboard(blackboard) {}

	virtual Status update() = 0;

protected:
	Blackboard::Ptr blackboard;
};

class BehaviorTree : public Node
{
public:
	BehaviorTree() : blackboard(std::make_shared<Blackboard>()) {}
	BehaviorTree(const Node::Ptr& rootNode) : BehaviorTree() { root = rootNode; }

	Status update() { return root->tick(); }

	void setRoot(const Node::Ptr& node) { root = node; }
	Blackboard::Ptr getBlackboard() const { return blackboard; }

private:
	Node::Ptr root = nullptr;
	Blackboard::Ptr blackboard = nullptr;
};

template <class Parent>
class DecoratorBuilder;

template <class Parent>
class CompositeBuilder
{
public:
	CompositeBuilder(Parent* parent, Composite* node) : parent(parent), node(node) {}

	template <class NodeType, typename... Args>
	CompositeBuilder<Parent> leaf(Args... args)
	{
		auto child = std::make_shared<NodeType>((args)...);
		node->addChild(child);
		return *this;
	}

	template <class CompositeType, typename... Args>
	CompositeBuilder<CompositeBuilder<Parent>> composite(Args... args)
	{
		auto child = std::make_shared<CompositeType>((args)...);
		node->addChild(child);
		return CompositeBuilder<CompositeBuilder<Parent>>(this, (CompositeType*)child.get());
	}

	template <class DecoratorType, typename... Args>
	DecoratorBuilder<CompositeBuilder<Parent>> decorator(Args... args)
	{
		auto child = std::make_shared<DecoratorType>((args)...);
		node->addChild(child);
		return DecoratorBuilder<CompositeBuilder<Parent>>(this, (DecoratorType*)child.get());
	}

	Parent& end()
	{
		return *parent;
	}

private:
	Parent* parent;
	Composite* node;
};

template <class Parent>
class DecoratorBuilder
{
public:
	DecoratorBuilder(Parent* parent, Decorator* node) : parent(parent), node(node) {}

	template <class NodeType, typename... Args>
	DecoratorBuilder<Parent> leaf(Args... args)
	{
		auto child = std::make_shared<NodeType>((args)...);
		node->setChild(child);
		return *this;
	}

	template <class CompositeType, typename... Args>
	CompositeBuilder<DecoratorBuilder<Parent>> composite(Args... args)
	{
		auto child = std::make_shared<CompositeType>((args)...);
		node->setChild(child);
		return CompositeBuilder<DecoratorBuilder<Parent>>(this, (CompositeType*)child.get());
	}

	template <class DecoratorType, typename... Args>
	DecoratorBuilder<DecoratorBuilder<Parent>> decorator(Args... args)
	{
		auto child = std::make_shared<DecoratorType>((args)...);
		node->setChild(child);
		return DecoratorBuilder<DecoratorBuilder<Parent>>(this, (DecoratorType*)child.get());
	}

	Parent& end()
	{
		return *parent;
	}

private:
	Parent* parent;
	Decorator* node;
};

/*
Builder 클래스는 행동 트리 생성 과정을 단순화합니다.트리를 생성하기 위해 세 가지 방법을 사용합니다.

leaf<NodeType>()
composite<CompositeType>()
decorator<DecoratorType>()

composite()와 decorator()는 end()를 호출해야 합니다.이는 복합체에 자식을 추가하거나 데코레이터에 자식을 추가하는 작업이 완료되었음을 나타냅니다.마지막으로 build()를 호출하면 완성된 행동 트리를 얻을 수 있습니다.

leaf()
leaf()는 단순히 노드의 종류를 지정합니다.예를 들어, leaf<Seek>()는 Seek 노드를 생성합니다.

composite()
composite()는 복합 노드를 생성합니다.복합 노드는 자식 노드의 집합입니다.가장 일반적인 유형의 복합 노드는 SEQUENCE, ALTERNATE, PRIORITY입니다.

build()
build()는 행동 트리를 생성합니다. build()를 호출하기 전에 모든 노드를 생성하고 연결해야 합니다. build()는 트리의 모든 노드를 순회하고 각 노드의 동작을 실행합니다.
*/
class Builder
{
public:
	template <class NodeType, typename... Args>
	Builder leaf(Args... args)
	{
		root = std::make_shared<NodeType>((args)...);
		return *this;
	}

	template <class CompositeType, typename... Args>
	CompositeBuilder<Builder> composite(Args... args)
	{
		root = std::make_shared<CompositeType>((args)...);
		return CompositeBuilder<Builder>(this, (CompositeType*)root.get());
	}

	template <class DecoratorType, typename... Args>
	DecoratorBuilder<Builder> decorator(Args... args)
	{
		root = std::make_shared<DecoratorType>((args)...);
		return DecoratorBuilder<Builder>(this, (DecoratorType*)root.get());
	}

	Node::Ptr build()
	{
		assert(root != nullptr && "The Behavior Tree is empty!");
		auto tree = std::make_shared<BehaviorTree>();
		tree->setRoot(root);
		return tree;
	}

private:
	Node::Ptr root;
};

// The Selector composite ticks each child node in order.
// If a child succeeds or runs, the selector returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children fails, only then does the selector fail.
// Selector composite는 자식 노드가 Success를 반환할 때까지 자식 노드를 실행한다.
// 자식 노드 중 1개만 선택하는 것
class Selector : public Composite
{
public:
	void initialize() override
	{
		it = children.begin();
	}

	Status update() override
	{
		assert(hasChildren() && "Composite has no children");

		while (it != children.end()) {
			auto status = (*it)->tick();

			if (status != Status::Failure) {
				return status;
			}

			++it;
		}

		return Status::Failure;
	}
};

// The Sequence composite ticks each child node in order.
// If a child fails or runs, the sequence returns the same status.
// In the next tick, it will try to run each child in order again.
// If all children succeeds, only then does the sequence succeed.
// Sequence composite는 자식 노드가?Fail?를 반환할 때까지 자식 노드들을 실행한다.
// 모든 자식 노드를 실행한다
class Sequence : public Composite
{
public:
	void initialize() override
	{
		it = children.begin();
	}

	Status update() override
	{
		assert(hasChildren() && "Composite has no children");

		while (it != children.end()) {
			auto status = (*it)->tick();

			if (status != Status::Success) {
				return status;
			}

			++it;
		}

		return Status::Success;
	}
};

// The StatefulSelector composite ticks each child node in order, and remembers what child it prevously tried to tick.
// If a child succeeds or runs, the stateful selector returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children fails, only then does the stateful selector fail.
// StatefulSelector 복합 노드는 각 하위 노드를 순서대로 실행하고, 이전에 시도한 하위 노드를 기억합니다.
// 하위 노드 중 하나가 성공하거나 실행 중이면, StatefulSelector는 동일한 상태를 반환합니다.
// 다음 틱에서는 다음 하위 노드를 실행하거나 처음부터 다시 시작하려고 시도합니다.
// 모든 하위 노드가 실패한 경우에만 StatefulSelector가 실패합니다.
class StatefulSelector : public Composite
{
public:
	Status update() override
	{
		assert(hasChildren() && "Composite has no children");

		while (it != children.end()) {
			auto status = (*it)->tick();

			if (status != Status::Failure) {
				return status;
			}

			++it;
		}

		it = children.begin();
		return Status::Failure;
	}
};

// The StatefulSequence composite ticks each child node in order, and remembers what child it prevously tried to tick.
// If a child fails or runs, the stateful sequence returns the same status.
// In the next tick, it will try to run the next child or start from the beginning again.
// If all children succeeds, only then does the stateful sequence succeed.
// MemSequence는 순서대로 각 자식 노드를 검사한다.
// 이전에 검사한 자식 노드의 인덱스를 가져와서 검사하고, 검사가 실패하면 종료된다.
class MemSequence : public Composite
{
public:
	Status update() override
	{
		assert(hasChildren() && "Composite has no children");

		while (it != children.end()) {
			auto status = (*it)->tick();

			if (status != Status::Success) {
				return status;
			}

			++it;
		}

		it = children.begin();
		return Status::Success;
	}
};

class ParallelSequence : public Composite
{
public:
	ParallelSequence(bool successOnAll = true, bool failOnAll = true) : useSuccessFailPolicy(true), successOnAll(successOnAll), failOnAll(failOnAll) {}
	ParallelSequence(int minSuccess, int minFail) : minSuccess(minSuccess), minFail(minFail) {}

	Status update() override
	{
		assert(hasChildren() && "Composite has no children");

		int minimumSuccess = minSuccess;
		int minimumFail = minFail;

		if (useSuccessFailPolicy) {
			if (successOnAll) {
				minimumSuccess = static_cast<int>(children.size());
			}
			else {
				minimumSuccess = 1;
			}

			if (failOnAll) {
				minimumFail = static_cast<int>(children.size());
			}
			else {
				minimumFail = 1;
			}
		}

		int total_success = 0;
		int total_fail = 0;

		for (auto& child : children) {
			auto status = child->tick();
			if (status == Status::Success) {
				total_success++;
			}
			if (status == Status::Failure) {
				total_fail++;
			}
		}

		if (total_success >= minimumSuccess) {
			return Status::Success;
		}
		if (total_fail >= minimumFail) {
			return Status::Failure;
		}

		return Status::Running;
	}

private:
	bool useSuccessFailPolicy = false;
	bool successOnAll = true;
	bool failOnAll = true;
	int minSuccess = 0;
	int minFail = 0;
};

// The Succeeder decorator returns success, regardless of what happens to the child.
// Succeeder decorator는 하위 노드의 결과에 관계없이 항상 성공을 반환합니다.
class Succeeder : public Decorator
{
public:
	Status update() override
	{
		child->tick();
		return Status::Success;
	}
};

// Failer decorator는 하위 노드의 결과에 관계없이 항상 실패를 반환합니다.
class Failer : public Decorator
{
public:
	Status update() override
	{
		child->tick();
		return Status::Failure;
	}
};

// The Inverter decorator inverts the child node's status, i.e. failure becomes success and success becomes failure.
// If the child runs, the Inverter returns the status that it is running too.
// Inverter decorator는 하위 노드의 상태를 반전시킵니다. 즉, 실패는 성공으로, 성공은 실패로 변합니다.
// 만약 하위 노드가 실행 중이면, Inverter는 실행 중인 상태를 반환합니다.
class Inverter : public Decorator
{
public:
	Status update() override
	{
		auto s = child->tick();

		if (s == Status::Success) {
			return Status::Failure;
		}
		else if (s == Status::Failure) {
			return Status::Success;
		}

		return s;
	}
};

// The Repeater decorator repeats infinitely or to a limit until the child returns success.
// Repeater decorator는 하위 노드가 성공을 반환할 때까지 무한하게 또는 한계까지 반복합니다.
class Repeater : public Decorator
{
public:
	Repeater(int limit = 0) : limit(limit) {}

	void initialize() override
	{
		counter = 0;
	}

	Status update() override
	{
		child->tick();

		if (limit > 0 && ++counter == limit) {
			return Status::Success;
		}

		return Status::Running;
	}

protected:
	int limit;
	int counter = 0;
};

// The UntilSuccess decorator repeats until the child returns success and then returns success.
// UntilSuccess decorator는 하위 노드가 성공을 반환할 때까지 반복하고, 그 후에 성공을 반환합니다.
class UntilSuccess : public Decorator
{
public:
	Status update() override
	{
		while (1) {
			auto status = child->tick();

			if (status == Status::Success) {
				return Status::Success;
			}
		}
	}
};

// The UntilFailure decorator repeats until the child returns fail and then returns success.
// UntilFailure decorator는 하위 노드가 실패를 반환할 때까지 반복하고, 그 후에 성공을 반환합니다.
class UntilFailure : public Decorator
{
public:
	Status update() override
	{
		while (1) {
			auto status = child->tick();

			if (status == Status::Failure) {
				return Status::Success;
			}
		}
	}
};
} // namespace makga::lib