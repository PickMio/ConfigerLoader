#include "Define.h"
#include "Common/Common.h"
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include "pugiconfig.hpp"
#include "pugixml.hpp"
#include "msg_PublicProto.h"
using namespace std;
using namespace pugi;


class FShipChatServer; 
class ConfigerMgr;
class CGameActivityBOXRewardMgr
{
    SINGLETON_CLASS_DEFINE(CGameActivityBOXRewardMgr);

public:
    enum {COMMON_DROP_ID = 160000, //100盘以前正常掉落
          MAX_DROP_ID = 160100,   //100盘以后开到积分和喇叭
         };
    void Init(FShipChatServer *server);
    void LoadConfig();
    int GetGameActivityBOXReward(int userid, int gettimes, int condition, int activityid, vector<pbproto::BoxAwardInfo>& award);
    int GetBOXReward(int gettimes, int condition, vector<pbproto::BoxAwardInfo>& award);
    int GetAward(int id, vector<pbproto::BoxAwardInfo>& award);
    int GetRandomID(int id, vector<int>& idList);
    int SetAward(pbproto::BoxAwardInfo& info, int id);


private:
	FShipChatServer *m_pFShipServer;
	ConfigerMgr *m_ConfigerMgr;

   
};



typedef struct tagDetailRate
{
	int itemID;
	int count;
}DetailRate;

class Utils
{
public:
	/************************************************************************/
	/* 输入const字符串指针str和分隔符s,返回用s隔开的字符串vector                                                                     */
	/************************************************************************/
	static int split(const char* str, char s,vector<string>&ret)
	{
		char buff[1024];
		char tmp[1024];
		memset(buff, 0, sizeof(buff));
		memset(tmp, 0, sizeof(buff));
		strncpy(buff, str, 1024);
		int len = strlen(str);
		if(buff[len - 1] != s)
		{
			buff[len++] = s;//这里可能会溢出,最后的分号忘记写了
		}
		for (int c = 0,k = 0; c <len ;c++)
		{
			if (buff[c] != s)
			{
				tmp[k++] = buff[c];
				continue;
			}
			tmp[k] = '\0';
			ret.push_back(tmp);
			k = 0;
		}

		return 0;
	}

	static int parse_detail_rate(const char *src, vector<DetailRate>&ret)
	{

		vector<string> tmp;
		vector<string> tmp2;
		split(src, ';', tmp);
		DetailRate dr = {0};
		for(vector<string>::iterator it=tmp.begin(); it != tmp.end(); ++it)
		{
			split(it->c_str(), ',', tmp2);
			dr.itemID = atoi(tmp2.at(0).c_str());
			dr.count = atoi(tmp2.at(1).c_str());

			ret.push_back(dr);
		}


	}
	static int parse_count_range(const char *src, vector<int>&ret)
	{

		vector<string> tmp;
		split(src, ',', tmp);
		for(vector<string>::iterator it=tmp.begin(); it != tmp.end(); ++it)
		{
		        ret.push_back(atoi(it->c_str()));

		}


	}


};


class BaseItem
{
public:
	BaseItem()
	{
		m_ItemID = 0;
		m_AwardFrom = 0;
		m_AwardType = 0;
		m_AwardVal = 0;
		m_AwardCount = 0;
		m_AwardReserve = 0;
		m_Desc = ""; //物品描述
	}
	virtual int parse_xml(const xml_node& data)
	{
		if(0 == strcmp(data.name(), "id")){
			this->m_ItemID = atoi(data.child_value());
		}else if(0 == strcmp(data.name(), "des"))
		{
			this->m_Desc = data.child_value();
		}else if(0 == strcmp(data.name(), "award_from"))
		{
			this->m_AwardFrom = atoi(data.child_value());
		}else if(0 == strcmp(data.name(), "award_type"))
		{
			this->m_AwardType = atoi(data.child_value());
		}else if(0 == strcmp(data.name(), "award_val"))
		{
			this->m_AwardVal = atoll(data.child_value());
		}
		else if(0 == strcmp(data.name(), "award_count"))
		{
			this->m_AwardCount = atoi(data.child_value());
		}else if(0 == strcmp(data.name(), "award_reserve"))
		{
			this->m_AwardReserve = atoi(data.child_value());
		}
		else
		{
			this->parse_more_xml(data);
		}

		return 0;

	}         



	virtual void parse_more_xml(const xml_node& data){};

public:
	int m_ItemID;  //物品ID
	int m_AwardFrom;
	int m_AwardType;
	long long m_AwardVal;
	int m_AwardCount;
	int m_AwardReserve;
	string m_Desc; //物品描述



};
typedef map<int, BaseItem*> ITEM_MAP;
typedef map<int, BaseItem*>::iterator ITEM_MAP_ITERATOR;

class ConfigerMgr
{
public:
	ITEM_MAP m_ItemMap;
	ConfigerMgr()
	{

	}
	int add(BaseItem* item)
	{
		ITEM_MAP_ITERATOR iter = m_ItemMap.find(item->m_ItemID);
		if(iter != m_ItemMap.end())
		{
			delete iter->second;
			iter->second = item;
			return 0;
		}
		m_ItemMap.insert(make_pair(item->m_ItemID, item));
		return 0;
	}
	BaseItem *find(int itemID)
	{
		ITEM_MAP_ITERATOR iter = m_ItemMap.find(itemID);
		if(iter == m_ItemMap.end())
		{
			//打印错误信息
			//cout << "mei zhaodao a!" << endl;
			return NULL;
		}

		return iter->second;


	}

};

class BaseLoader
{
public:
	ConfigerMgr *m_ConfigerMgr;
	BaseItem *m_ItemInfo;
	string m_FileName;
	BaseLoader(){}
	BaseLoader(ConfigerMgr *mgr, char* file)
	{
		m_ConfigerMgr = mgr;
		m_FileName = file; 
	}
	int LoadXml()
	{
		xml_document doc;
		string filename = "config/" + this->m_FileName + ".xml";
		//cout << "读取的文件为" << filename << endl;
		xml_parse_result rst2 = doc.load_file(filename.c_str());
		if(status_ok == rst2.status)
		{
			//cout << "读取" << this->m_FileName << "成功!" << endl;

		}else {
			//cout << "读取失败, error code is" << rst2.status << endl;
			return -1;
		}
		xml_node node = doc.child("root");
		node = node.child(this->m_FileName.c_str());
		xml_node parent = node;
		for(;parent;parent = parent.next_sibling()){
			m_ItemInfo = this->OnCreate();
			for(xml_node data=parent.first_child();data; data=data.next_sibling())
			{
				m_ItemInfo->parse_xml(data);
			}
			m_ConfigerMgr->add(m_ItemInfo);
		}
	}       
	virtual BaseItem* OnCreate(){/*cout << "base OnCreate called!" << endl;*/}
};

class ItemBoxInfo : public BaseItem
{
public:
        vector<int> m_AwardCountRange;
	virtual void parse_more_xml(const xml_node& data)
	{
            if(0 == strcmp(data.name(), "reward_count_range"))
            {
                     Utils::parse_count_range(data.child_value(), m_AwardCountRange);
            }
	}   

};
class DropInfo : public BaseItem
{
public:
	virtual void parse_more_xml(const xml_node& data)
	{
		if(0 == strcmp(data.name(), "detail_rate"))
		{
                        
			Utils::parse_detail_rate(data.child_value(), m_DetailRate);
		}

	}   
	vector<DetailRate> m_DetailRate;

};

class ItemBoxLoader : public BaseLoader
{
public:
	ItemBoxLoader(ConfigerMgr *mgr, char* file):BaseLoader(mgr, file){}

	virtual BaseItem* OnCreate()
	{
		return new ItemBoxInfo();
	}

};
class DropLoader : public BaseLoader
{
public:
	DropLoader(ConfigerMgr *mgr, char* file):BaseLoader(mgr, file){}
	virtual BaseItem* OnCreate()
	{
		return new DropInfo();
	}

};

