#include <iostream>
#include <string>
#include "pugixml/pugixml.hpp"
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <map>

using namespace std;
using namespace pugi;

void parse_xml(const string name)
{
	xml_document doc;
	string filename = "config/" + name + ".xml";
        cout << "��ȡ���ļ�Ϊ" << filename << endl;
	xml_parse_result rst2 = doc.load_file(filename.c_str());
	if(status_ok == rst2.status)
	{
	    cout << "�����ɹ�!" << name << endl;

	}else {
	    cout << "����ʧ��, error code is" << rst2.status << endl;
	}
	xml_node node = doc.child("root");
	node = node.child(name.c_str());

	for(xml_node data=node.first_child();data; data=node.next_sibling()){
	    cout << data.name() << endl;
	    cout << data.child_value() << endl;
	}


}
void test_plugin()
{
    parse_xml("drop");
    parse_xml("items_box");
}


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
	m_Desc = ""; //��Ʒ����
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
                this->m_AwardVal = atoi(data.child_value());
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
	int m_ItemID;  //��ƷID
	int m_AwardFrom;
	int m_AwardType;
	int m_AwardVal;
	int m_AwardCount;
	int m_AwardReserve;
	string m_Desc; //��Ʒ����



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
            //��ӡ������Ϣ
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
        cout << "��ȡ���ļ�Ϊ" << filename << endl;
	xml_parse_result rst2 = doc.load_file(filename.c_str());
	if(status_ok == rst2.status)
	{
	    cout << "��ȡ" << this->m_FileName << "�ɹ�!" << endl;

	}else {
	    cout << "��ȡʧ��, error code is" << rst2.status << endl;
            return -1;
	}
	xml_node node = doc.child("root");
	node = node.child(this->m_FileName.c_str());

	for(xml_node data=node.first_child();data; data=data.next_sibling())
        {
            cout << __LINE__ << "set m_ItemInfo" << endl;
            m_ItemInfo = this->OnCreate(); //����Ϊʲôû��������ִ��?
            m_ItemInfo->parse_xml(data);
            cout << __LINE__ <<"parse ok" << endl;
            m_ConfigerMgr->add(m_ItemInfo);
        }
    }       
    virtual BaseItem* OnCreate(){cout << "base OnCreate called!" << endl;}
};

class ItemBoxInfo : public BaseItem
{
public:
    virtual void parse_more_xml(const xml_node& data)
    {
    }   

};
class DropInfo : public BaseItem
{
public:
    virtual void parse_more_xml(const xml_node& data)
    {
        if(0 == strcmp(data.name(), "rate"))
        {
            m_Rate = atoi(data.child_value());
        }
        if(0 == strcmp(data.name(), "detail_rate"))
        {
            m_DetailRate = data.child_value();
        }

    }   
    int m_Rate;
    string m_DetailRate;

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
        cout << "create drop info success!" << endl;
        return new DropInfo();
    }

};


class Utils
{
public:
/************************************************************************/
/* ����const�ַ���ָ��str�ͷָ���s,������s�������ַ���vector                                                                     */
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
        buff[len++] = s;//������ܻ����,���ķֺ�����д��
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

};

void test()
{
/*
    vector<string> vs;
    Utils::split("game,1;age,2;sex,3", ';', vs);
    vector<string> de;
    for(vector<string>::iterator v= vs.begin(); v!= vs.end(); v++)
    {
        Utils::split((*v).c_str(), ',', de);
        cout << "name: " << de[0] << " value: " << atoi(de[1].c_str()) << endl;
        de.clear();
    }
*/
    ConfigerMgr *mgr = new ConfigerMgr();
    BaseLoader *dropLoader = new DropLoader(mgr, "drop");
    dropLoader->LoadXml();
}








int main(int argc, char *argv[])
{

    //test_plugin();
    test();
    return 0;
}
