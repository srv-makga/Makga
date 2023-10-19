# Makga
범용 서버 엔진 제작
=============

ServerGame.h
```C++
class ServerGame : public ServerBase
{
  // ...
};
```

Game.cpp
```C++
ServerGame server(&ConfigGame::Instance());
server.Initialize();

if (false == server.Start())
{
  LOG_FATAL << "server start fail";
  return 0;
}

server.Wait(); // 서버가 종료 이벤트를 받기 전까지 함수 내에서 대기
server.Finallize();
```
