#pragma once
#include<iostream>
#include<list>
#include<vector>


/*单姓名字池*/
class FirstName
{
public:
	std::string m_first;				//单姓
	std::vector<std::string> m_last_list;	//名池
};


/*姓名池*/
class RandomName
{
public:
	std::vector<FirstName*> m_pool;	//姓名池
	RandomName();
	virtual ~RandomName();

	/*读姓文件和名文件*/
	void LoadFile();
	/*取名字*/
	std::string GetName();
	/*归还名字*/
	void Release(std::string _name);
};

