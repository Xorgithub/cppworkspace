#pragma once

#include <iostream>
#include <string>
#include <list>
using namespace std;

class cloneable
{
public:
	virtual cloneable* clone() = 0;
};

class work_experience:public cloneable
{
public:
	cloneable* clone()
	{
		work_experience *obj = new work_experience;
		obj->work_date = this->work_date;
		obj->company = this->company;
		return obj;
	}
private:
	string work_date;
	string company;
};

class resume:public cloneable
{
public:
	void set_personal_info(string name, string age, string sex,string phone)
	{
		name_ = name;
		age_ = age;
		sex_ = sex;
		phone_ = phone;
	}
	void set_work_experience(string date,string company)
	{
		work_exp_list_.push_back(make_pair(date,company));
	}
	cloneable* clone()
	{
		resume *obj = new resume;
		obj->name_ = this->name_;
		obj->age_ = this->age_;
		obj->sex_ = this->sex_;
		obj->phone_ = this->phone_;
		obj->work_exp_list_ = this->work_exp_list_;
		return obj;
	}
	void print()
	{
		cout<<"info"<<endl;
		cout<<"name:"<<name_<<" age:"<<age_<<" sex:"
			<<sex_<<" phone:"<<phone_<<endl;
		cout<<"exper"<<endl;
		for (list< pair<string,string> >::iterator itr = work_exp_list_.begin();
			itr!=work_exp_list_.end();++itr)
		{
			cout<<"date:"<<itr->first<<" company:"<<itr->second<<endl;
		}
		cout<<endl;
	}
private:
	string name_;
	string age_;
	string sex_;
	string phone_;
	list< pair<string,string> > work_exp_list_;
};
