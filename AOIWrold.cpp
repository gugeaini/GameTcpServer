#include "AOIWrold.h"

AOIWrold::AOIWrold(int _xbegin, int _xend, int _ybegin, int _yend, int _xcount, int _ycount):
	m_X_begin(_xbegin),m_X_end(_xend),m_Y_begin(_ybegin),m_Y_end(_yend),m_X_count(_xcount),m_Y_count(_ycount)
{
	//网格宽度=(x_end - x_begin)/x_count
	m_X_width = (m_X_end - m_X_begin) / m_X_count;
	m_Y_width = (m_Y_end - m_Y_begin) / m_Y_count;

	for (int i = 0; i < m_X_count * m_Y_count; i++)
	{
		Grid _grid;
		m_Grids.push_back(_grid);
	}
}

std::list<Player*> AOIWrold::GetsrdPlayers(Player* _player)
{
	/*返回周围玩家的格子*/
	std::list<Player*> ret;

	/*计算当前格子位置*/
	int grid_id = (_player->GetX() - m_X_begin) / m_X_width + (_player->GetY() - m_Y_begin) / m_Y_width * m_X_count;
	/*计算当前格子在X轴 和 Y轴 的位置*/
	int x_index = grid_id % m_X_count;
	int y_index = grid_id / m_X_count;

	/*正中*/
	auto& cur_list = m_Grids[grid_id].m_Players_list;
	ret.insert(ret.begin(), cur_list.begin(), cur_list.end());

	/*左上*/
	if (x_index > 0 && y_index > 0)
	{
		auto& cur_list = m_Grids[grid_id - 1 - m_X_count].m_Players_list;
		ret.insert(ret.begin(),cur_list.begin(),cur_list.end());
	}
	/*正上*/
	if (y_index > 0)
	{
		auto& cur_list = m_Grids[grid_id - m_X_count].m_Players_list;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	/*右上*/
	if (x_index < m_X_count - 1 && y_index>0)
	{
		auto& cur_list = m_Grids[grid_id + 1 - m_X_count].m_Players_list;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	/*正左*/
	if (x_index > 0)
	{
		auto& cur_list = m_Grids[grid_id - 1].m_Players_list;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	/*正右*/
	if (x_index < m_X_count - 1)
	{
		auto& cur_list = m_Grids[grid_id + 1].m_Players_list;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	/*左下*/
	if (x_index > 0 && y_index < m_Y_count - 1)
	{
		auto& cur_list = m_Grids[grid_id - 1 + m_X_count].m_Players_list;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	/*正下*/
	if (y_index < m_Y_count - 1)
	{
		auto& cur_list = m_Grids[grid_id + m_X_count].m_Players_list;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}
	/*右下*/
	if (x_index < m_X_count - 1 && y_index < m_Y_count - 1)
	{
		auto& cur_list = m_Grids[grid_id + 1 + m_X_count].m_Players_list;
		ret.insert(ret.begin(), cur_list.begin(), cur_list.end());
	}


	return ret;
}

bool AOIWrold::AddPlayer(Player* _player)
{
	bool bRet = true;
	// 网格编号=(x - x_begin)/x_wigth + (y - y_begin)/y_wight * x_count
	int grid_id = (_player->GetX() - m_X_begin) / m_X_width + (_player->GetY() - m_Y_begin) / m_Y_width * m_X_count;
	m_Grids[grid_id].m_Players_list.push_back(_player);

	return bRet;
}

void AOIWrold::delPlayer(Player* _player)
{
	int grid_id = (_player->GetX() - m_X_begin) / m_X_width + (_player->GetY() - m_Y_begin) / m_Y_width * m_X_count;

	m_Grids[grid_id].m_Players_list.remove(_player);

}
