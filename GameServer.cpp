#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "DBServerPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "MonsterTable.h"
#include "PlayerLevel.h"
#include "DBSession.h"
#include "ItemUtils.h"
#include "MapManager.h"
#include "RoomManager.h"
#include "GlobalQueue.h"
#include "LoginManager.h"
#include <chrono>
#include "Utils.h"
enum {
	WORKER_TICK = 64,

};
// 콘솔 커서 위치 이동 함수
void MoveCursor(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void ClearLine(int y) {
    MoveCursor(0, y);
    cout << string(150, ' ');  // 80칸 비움 (화면 너비에 따라 조절)
    MoveCursor(0, y);
}

void DoDBWorkerJob(ClientServiceRef& service)
{
    while (true) {
        LEndTickCount = ::GetTickCount64() + WORKER_TICK;

        service->GetIocpCore()->Dispatch(10);

        ThreadManager::DistributeReservedJobs();

        ThreadManager::DoGlobalQueueWork();
    }
}


void DoWorkerJob(ServerServiceRef& service, int index)
{
    uint64 i = 0;
	while (true) {
        //MoveCursor(index + 7, 0);
        //ClearLine(index + 7);
        //LOG(index << " : " << i++);
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DistributeReservedJobs();

		ThreadManager::DoGlobalQueueWork();
	}
}


int main()
{
    SetConsoleOutputCP(CP_UTF8);

	PlayerLevel::Init();				// 레벨테이블
    ClientPacketHandler::Init();
    DBServerPacketHandler::Init();
    GLoginManager = MakeShared<LoginManager>();
    GLoginManager->Init();
    bool bMonsterLoad = MonsterTable::LoadMonstersFromXML("MonsterInfo.xml");	// 몬스터 기본정보를 불러옴
    bool bItemLoad = ItemUtils::LoadItemsFromXML("items.xml");
    if (!bMonsterLoad) return -1; // 몬스터 정보 로드 실패
    if (!bItemLoad) return -1; // 아이템 로드 실패

    MonsterTable::PrintMonsters();  // 몬스터 정보를 불러옴;
	std::this_thread::sleep_for(std::chrono::microseconds(1000));

    
    string connection = "Driver={MySQL ODBC 9.2 Unicode Driver};"
        "Server=" + std::string(DB_HOST) + ";"
        "Database=" + std::string(DB_NAME) + ";"
        "UID=" + std::string(DB_USER) + ";"
        "PWD=" + std::string(DB_PASS) + ";"
        "CHARSET=utf8mb4;"
        "OPTION=3;";
	ASSERT_CRASH(GDBConnectionPool->Connect(10, connection)); 

	
	// DB 서버 접속
	ClientServiceRef DBservice = MakeShared<ClientService>(
		NetAddress(L"127.0.0.1", 7788),
		MakeShared<IocpCore>(),
		MakeShared<DBSession>, // TODO : SessionManager 등
		1);

	ASSERT_CRASH(DBservice->Start());
	
	for (int32 i = 0; i < 1; i++)
	{
		GThreadManager->Launch([&DBservice]()
			{
                DoDBWorkerJob(DBservice);

			});
	}


	ServerServiceRef service = MakeShared<ServerService>(
		//
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, // TODO : SessionManager 등
		10);

	ASSERT_CRASH(service->Start());
	for (int32 i = 0; i < 20; i++)
	{
		GThreadManager->Launch([&service, i]()
			{
				DoWorkerJob(service, i);
			});
	}
    
	
    while (true) {
        MoveCursor(0,0);
        ClearLine(0);
        int i = 1;

        cout << u8"[접속자 수] : " << service->GetCurrentSessionCount() << endl;
        for (auto& map : GMapManager->GetLevelNames()) {
            uint64 index = map.first;
            if (const auto& roomManager = GMapManager->GetRoomManager(index)) {
                ClearLine(i++);
                cout << u8"[맵 이름] : " << roomManager->GetLevelName() << u8" [생성된 채널] : " << roomManager->GetRoomCnt() << u8" [접속인원] : " << roomManager->GetJoinCnt() << endl;
            }
        }

        Protocol::S_PING pkt;
        uint64 ms = Utils::Millis();
        pkt.set_timestamp(ms);
        SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
        service->PingBroadcast(sendBuffer, ms,10000);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	//DoWorkerJob(service);
	GThreadManager->Join();

	
}