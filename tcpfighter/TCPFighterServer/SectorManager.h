#pragma once
#define SECTOR_X 100
#define SECTOR_Y 100
#define SECTOR_WIDTH 300
#define SECTOR_DEPTH 200
#define SECTOR_MANAGER SectorManager::GetInstance()

struct Player;
struct Session;

struct SECTOR_POS
{
	int x;
	int y;
};

class SectorManager
{
	static SectorManager * _instance;
	list<Player *> _sector[SECTOR_X][SECTOR_Y];
	SectorManager();
	~SectorManager();
protected:
	bool InsertPlayer(Player * player);
	void NotifyAroundSector(Player * player);
public:
	static SectorManager & GetInstance() 
	{
		if (_instance == nullptr)
			_instance = new SectorManager();
		return *_instance;
	}
	SECTOR_POS CalculateSector(Player * player);
	bool DeletePlayer(Player * player);
	bool UpdateSector(Player * player);
	bool ChangeSector(Player * player);
	bool AddSectorUserPacket(Player * player, SECTOR_POS pos);
	bool DeleteSectorUserPacket(Player * player, SECTOR_POS pos);
	bool AddUserPacket_ForSector(Player * player, SECTOR_POS pos);
	bool DeleteUserPacket_ForSector(Player * player, SECTOR_POS pos);
	bool NewPlayer(Player * player);
	bool BroadCast_Sector(MinLib::StreamBuffer * stream, Session * session, bool dontSend, SECTOR_POS pos);
};
