# Makga
범용 서버 엔진 제작

예제

.h
class ServerGame : public ServerBase
{
  // todo
};



.cpp
ServerGame server(&ConfigGame::Instance());
server.Initialize();

if (false == server.Start())
{
  LOG_FATAL << "server start fail";
  return 0;
}

server.Wait();
server.Finallize();
