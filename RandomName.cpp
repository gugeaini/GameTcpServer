#include "RandomName.h"
#include<random>
#include<fstream>

using namespace std;
static default_random_engine random_engine(time(NULL));

RandomName::RandomName()
{
}

RandomName::~RandomName()
{
}

void RandomName::LoadFile()
{
	/*打开文件*/
	ifstream first("random_first.txt");
	ifstream last("random_last.txt");

	/*读名文件*/
	string last_name;
	vector<string> tmp;
	while (getline(last,last_name))
	{
		tmp.push_back(last_name);
	}

	/*读姓文件*/
	string first_name;
	while (getline(first, first_name))
	{
		auto first_name_list = new FirstName();
		first_name_list->m_first = first_name;
		first_name_list->m_last_list = tmp;
		m_pool.push_back(first_name_list);
	}

}

std::string RandomName::GetName()
{
	/*取随机姓*/
	auto num = random_engine() % m_pool.size();
	string first = m_pool[num]->m_first;
	/*取随机名*/
	auto lastNum = random_engine() % m_pool[num]->m_last_list.size();
	string last = m_pool[num]->m_last_list[lastNum];
	/*从池子中摘除取好的名字*/
	if (m_pool[num]->m_last_list.size() <= 0)//表示这是该姓的最后一个元素
	{
		delete m_pool[num];
		m_pool.erase(m_pool.begin() + num);
	}

	return first + " " + last;
}

void RandomName::Release(std::string _name)
{
	/*拆分姓和名*/
	int num = _name.find(" ", 0);
	string first = _name.substr(0, num);
	string last = _name.substr(num + 1, _name.size() - num - 1);

	/*查找该姓的位置，并插入名*/
	bool falg = false;

	for (auto single : m_pool)
	{
		if (first == single->m_first)
		{
			falg = true;
			single->m_last_list.push_back(last);
			break;
		}
	}

	/*没有找到姓，添加到姓名池*/
	if (false == falg)
	{
		auto fristName = new FirstName();
		fristName->m_first = first;
		fristName->m_last_list.push_back(last);
		m_pool.push_back(fristName);
	}

}
