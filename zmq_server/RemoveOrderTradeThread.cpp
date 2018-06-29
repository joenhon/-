#include "RemoveOrderTradeThread.h"



RemoveOrderTradeThread::RemoveOrderTradeThread()
{
}


RemoveOrderTradeThread::~RemoveOrderTradeThread()
{
}

void RemoveOrderTradeThread::remakeOrder()
{
	Config_ configSettings("config.txt");
	string url;
	url = configSettings.Read("remakeOrder.url", url);

	OrderTradeThread OTT;

	Json::Reader readerinfo;
	Json::Value root;
	std::cout << "-------------------remakeOrder�߳�����-----------------------" << std::endl;

	//ָ��zmq ����I/0�¼���thread pool Ϊ1
	void* context = zmq_init(1);
	//��������
	void* z_socket = zmq_socket(context, ZMQ_PULL);

	//Redis* red((redis.getObj()).get());
	//Redis redis_ = *red;

	zmq_bind(z_socket, url.c_str());    // accept connections on a socket
												   //����
	O order;
	int recvn = 1;
	//ѭ������
	while (1)
	{
		//���ܲ���
		zmq_msg_t recv_msg;
		zmq_msg_init(&recv_msg);
		zmq_msg_recv(&recv_msg, z_socket, 0);//0��ʾ������
											 //���Ͳ���
		zmq_msg_t send_msg;
		string strData = (char*)zmq_msg_data(&recv_msg);
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
					order = OTT.toOrder(root);
					if (order.getOrderType().compare("buy") == 0)
					{
						EnterCriticalSection(&removeBuyQuene_CS);
						removeBuyQuene.push(order);//���򵥶������
						LeaveCriticalSection(&removeBuyQuene_CS);

						//ToJson<BuyQuene> toJson;
						//redis_.open();
						//redis_.setRedis("removeBuyQuene", toJson.OToJson(removeBuyQuene));
						//toJson.~ToJson();
					}
					else
					{
						EnterCriticalSection(&removeSellQuene_CS);
						removeSellQuene.push(order);//�������������
						LeaveCriticalSection(&removeSellQuene_CS);

						//ToJson<SellQuene> toJson;
						//redis_.open();
						//redis_.setRedis("removeSellQuene", toJson.OToJson(removeSellQuene));
						//toJson.~ToJson();
					}
					//zmq_msg_init_size(&send_msg, 2);
					//memcpy(zmq_msg_data(&send_msg), "ok", 2);
					//zmq_sendmsg(z_socket, &send_msg, 0);
					//zmq_msg_close(&send_msg);
				}
				catch (const std::exception&)
				{
					//zmq_msg_init_size(&send_msg, 5);
					//memcpy(zmq_msg_data(&send_msg), "error", 5);
					//zmq_sendmsg(z_socket, &send_msg, 0);
					//zmq_msg_close(&send_msg);
					cout << "����json����" << root << endl;
				}

			}
			else
			{
				//zmq_msg_init_size(&send_msg, 5);
				//memcpy(zmq_msg_data(&send_msg), "error", 5);
				//zmq_sendmsg(z_socket, &send_msg, 0);
				//zmq_msg_close(&send_msg);
			}
		}
		//std::cout << "��\t" << recvn++ << "\t���յ�client��Ϣ��\t";
		//std::cout << (char*)zmq_msg_data(&recv_msg) << std::endl;
		zmq_msg_close(&recv_msg);

	}
	zmq_close(z_socket);
	zmq_term(context);
}
