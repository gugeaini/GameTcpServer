#pragma once
#include<list>
#include<vector>
/*玩家坐标类*/
class Player
{
public:
	virtual int GetX() = 0;
	virtual int GetY() = 0;

};


/*格子类*/
class Grid
{
public:
	std::list<Player*> m_Players_list;
};

/*地图类*/
class AOIWrold
{
	int m_X_begin = 0;
	int m_Y_begin = 0;
	int m_X_end = 0;	
	int m_Y_end = 0;
	int m_X_count = 0;
	int m_Y_count = 0;
	int m_X_width = 0;
	int m_Y_width = 0;

public:
	std::vector<Grid> m_Grids;
	AOIWrold(int _xbegin, int _xend, int _ybegin, int _yend, int _xcount, int _ycount);

	/*获取周围玩家*/
	std::list<Player*> GetsrdPlayers(Player* _player);

	/*添加玩家*/
	bool AddPlayer(Player* _player);
	/*摘除玩家*/
	void delPlayer(Player* _player);
};

