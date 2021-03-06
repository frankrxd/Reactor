#pragma once

#include "Socket.h"
#include "InetAddress.h"
#include "SocketIO.h"
#include <functional>
#include <memory>
using std::pair;
using std::function;
using std::shared_ptr;

class TcpConnection
:public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionCallback = std::function<void(const std::shared_ptr<TcpConnection>&)>;    
    
    TcpConnection(int peerfd); 
    ~TcpConnection() {}

    string receive();
    void send(const string & msg);
    void shutdown();
    bool isClosed();
    string connectionInfo() const;

	void setConnectionCallback(TcpConnectionCallback && cb)
	{	_onConnectionCallback = std::move(cb);	}

	void setMessageCallback(TcpConnectionCallback && cb)
	{	_onMessageCallback = std::move(cb);	}

	void setCloseCallback(TcpConnectionCallback && cb)
	{	_onCloseCallback = std::move(cb);	}


    void handleConnectionCallback();
	void handleMessageCallback();
	void handleCloseCallback();
private:
    pair<InetAddress,InetAddress> pairInetAddress();
private:

    int _fd;
    //Socket _sock;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    SocketIO _socketIO;
    bool _isShutdownWrite;
    
    TcpConnectionCallback _onConnectionCallback;
	TcpConnectionCallback _onMessageCallback;
	TcpConnectionCallback _onCloseCallback;
};

