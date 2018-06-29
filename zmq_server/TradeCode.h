#pragma once
#include <iostream>
#include <process.h>
#include <thread> 
#include <stdio.h>
#include <boost/lexical_cast.hpp>
#include <boost\lexical_cast.hpp>
#include "O.h"
#include <queue>
#include <cstdlib>
#include "Tx.h"
#include "json/json.h"
#include "amqp_tcp_socket.h"  
#include "amqp.h"  
#include "amqp_framing.h" 
//#include "windows.h"��RMQ��ͻ
#include "OrderTradeThread.h"
#include "RabbitMQ.h"
#include "ObjPool.h"
#include "Redis.h"
#include "cs_zmq.h"

using namespace std;
 struct BuyQuene
{
	 bool operator () ( O &o1,  O &o2)const {
		cpp_dec_float_50 bol = o2.getPrice().compare(o1.getPrice());
		if (bol>0)
		{
			return true;
		}
		else if (bol == 0)
		{
			return o1.getTime() < o2.getTime();
		}

		return  false;
	}
};
 struct SellQuene
{
	 bool operator () ( O &o1,  O &o2)const {
		cpp_dec_float_50 bol = o1.getPrice().compare(o2.getPrice());
		if (bol>0)
		{
			return true;
		}
		else if (bol == 0)
		{
			return o1.getTime() < o2.getTime();
		}

		return  false;
	}
};
 //�������ȼ�����
 extern priority_queue<O, vector<O>, BuyQuene> buyQuene;
 //�������ȼ�����
 extern priority_queue<O, vector<O>, SellQuene> sellQuene;
 //���򳷵����ȼ�����
 extern priority_queue<O, vector<O>, BuyQuene> removeBuyQuene;
 //���۳������ȼ�����
 extern priority_queue<O, vector<O>, SellQuene> removeSellQuene;

 /*��ʱͬ��Quene��Quene_a1*/
 //�ɶ�ʱ���ƵĶ���/*��ʱ�������緱æ��ͨ���㷨�ĳ���һ��ͬ���ļ��*/
 extern queue<O> buyQuene_a1;
 extern queue<O> sellQuene_a1;
 //�ɶ�ʱ���ƵĶ���/*��ʱ��RabbitMQ��������*/
 extern queue<Tx> RMQ_Quene;
 //�߳�����������ͬ���ķ��������ִ�У����������ĵ��ã�ֱ�����õķ�����ɵ���
 extern CRITICAL_SECTION buyQuene_CS,sellQuene_CS, removeBuyQuene_CS, removeSellQuene_CS, RabbitMQ_Send_CS, buyQuene_CS_a1, sellQuene_CS_a1, RMQ_Quene_CS;

 extern ObjPool<RabbitMQ> RMQ;
 extern ObjPool<Redis> redis;
 static cs_zmq c;
 static RabbitMQ rmq;

 class TradeCode
{
public:
	TradeCode();
	~TradeCode();
	
	
	
	/**
	* ���׺��ķ����������������
	*/
	static void core();
	static bool eq(O &o1, O &o2);
	static void remaveSell(priority_queue<O, vector<O>, SellQuene>& p);
	static void remaveBuy(priority_queue<O, vector<O>, BuyQuene>& p);
	static void delete_BuyQueue();
	static void delete_SellQueue();
	static O top_SellQueue();
	static O top_BuyQueue();
	static void delete_RemoveBuyQueue();
	static void delete_RemoveSellQueue();
	static O top_RemoveSellQueue();
	static O top_RemoveBuyQueue();
	static void Redis_();
};

