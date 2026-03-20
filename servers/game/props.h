#pragma once
#include <memory>
#include <functional>

class Actor;
// @brief Actor 생성 팩토리. Akka Props 개념에 대응. ActorSystem::Spawn() 등에서 Actor 인스턴스 생성에 사용.
class Props
{
public:
    using Creator = std::function<std::unique_ptr<Actor>()>;
	Props(Creator& c);
	Props(Creator&& c);
	std::unique_ptr<Actor> Create() const;

private:
    Creator creator_;
};
