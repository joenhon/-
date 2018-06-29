#include "OrderTradeThread.h"



OrderTradeThread::OrderTradeThread()
{
}


OrderTradeThread::~OrderTradeThread()
{
}
bool OrderTradeThread:: eq(O o1,O o2) {
	if (o1.getOrderId().length()>0 && o1.getOrderId().compare(o2.getOrderId())==0)
	{
		return true;
	}
	return false;
	//cout << "������" << o.getOrderId() << " ��" << o.getPrice().str() << " ʱ�䣺" << o.getTime() << "��ȷ:" << o.getPrice().str().length() << "   " << ss << endl;
}
void OrderTradeThread::addOrder()
{
	Config_ configSettings("config.txt");
	string url;
	url = configSettings.Read("addOrder.url", url);

	Json::Reader readerinfo;
	Json::Value root;
	std::cout << "-------------------addOrder�߳�����-----------------------" << std::endl;
	//ָ��zmq ����I/0�¼���thread pool Ϊ1
	void* context = zmq_init(1);
	//��������
	void* z_socket = zmq_socket(context, ZMQ_PULL);

	//Redis* red((redis.getObj()).get());
	//Redis redis_ = *red;

	

	zmq_bind(z_socket, url.c_str());    // accept connections on a socket
	
	//  Wait for start of batch
	//����
	O order;
	//ѭ������
	while (1)
	{
		//���ܲ���
		zmq_msg_t recv_msg;
		zmq_msg_t send_msg;
		zmq_msg_init(&recv_msg);
		zmq_msg_recv(&recv_msg, z_socket, 0);//0��ʾ������
		string strData = (char*)zmq_msg_data(&recv_msg);
		zmq_msg_close(&recv_msg);
		if (!strData.empty())
		{
			if (!readerinfo.parse(strData, root))
			{
				cout << (TEXT("json error!")) << endl;
			}
			if (!root["orderId"].isNull())
			{
				try
				{
					order = toOrder(root);
					if (order.getOrderType().compare("sell")==0)
					{
						EnterCriticalSection(&sellQuene_CS_a1);
						sellQuene_a1.push(order);//��ʱ�����������
						LeaveCriticalSection(&sellQuene_CS_a1);
						/*תjson����*/
						//ToJson<SellQuene> toJson;
						/*��Redis���Ӳ���������*/
						//redis_.open();
						//redis_.setRedis("sellQuene",toJson.OToJson(sellQuene));
						//toJson.~ToJson();
					}
					else
					{
						EnterCriticalSection(&buyQuene_CS_a1);
						buyQuene_a1.push(order);//��ʱ�򵥶������
						LeaveCriticalSection(&buyQuene_CS_a1);
						/*תjson����*/
						//ToJson<BuyQuene> toJson;
						/*��Redis���Ӳ���������*/
						//redis_.open();
						//redis_.setRedis("buyQuene", toJson.OToJson(buyQuene));
						//toJson.~ToJson();
					}
					//���Ͳ���
					//zmq_msg_init_size(&send_msg, 2);
					//memcpy(zmq_msg_data(&send_msg), "ok", 2);
					//zmq_sendmsg(z_socket, &send_msg, 0);
					//zmq_msg_close(&send_msg);
				}
				catch (const std::exception&)
				{
					//���Ͳ���
					//zmq_msg_init_size(&send_msg, 5);
					//memcpy(zmq_msg_data(&send_msg), "error", 5);
					//zmq_sendmsg(z_socket, &send_msg, 0);
					//zmq_msg_close(&send_msg);
					//BOOST_LOG(log) << "����json����" << root; 
					cout << "����json����" << root << endl;
				}

			}
			//std::cout << "��\t" << recvn++ << "\t���յ�client��Ϣ��\t";
			//std::cout << (char*)zmq_msg_data(&recv_msg) << std::endl;
			
		}
		

	}
	zmq_close(z_socket);
	zmq_term(context);
}
O OrderTradeThread::toOrder(Json::Value root)
{
	O order;
	if (!root["orderId"].isNull())
	{
		string orId = root["orderId"].asString();
		order.setOrderId(orId);

	}
	if (!root["orderType"].isNull())
	{
		order.setOrderType(root["orderType"].asCString());

	}
	if (!root["freezeAmt"].isNull())
	{
		cpp_dec_float_50 freezeAmt = cpp_dec_float_50(root["freezeAmt"].asString());
		order.setFreezeAmt(freezeAmt);

	}
	if (!root["cancel"].isNull())
	{
		order.setIsCancel(boost::lexical_cast<int>(root["cancel"].asString()));

	}
	if (!root["price"].isNull())
	{
		cpp_dec_float_50 price = cpp_dec_float_50(root["price"].asString());
		order.setPrice(price);

	}
	if (!root["quantity"].isNull())
	{
		cpp_dec_float_50 quantity = cpp_dec_float_50(root["quantity"].asString());
		order.setQuantity(quantity);

	}
	if (!root["symbol"].isNull())
	{
		order.setSymbol(root["symbol"].asCString());

	}
	if (!root["time"].isNull())
	{
		try
		{
			order.setTime(root["time"].asCString());
		}
		catch (const std::exception&)
		{
			order.setTime("");
		}
		

	}
	if (!root["tradeType"].isNull())
	{
		order.setTradeType(root["tradeType"].asCString());

	}
	if (!root["userUuid"].isNull())
	{
		order.setUserUuid(root["userUuid"].asCString());

	}

	return order;
}