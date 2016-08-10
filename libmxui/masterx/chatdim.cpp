#include "chatdim.h"
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/stat.h>
#include<unistd.h>
#include<signal.h>

namespace masterx {

	chatSocketConnection::chatSocketConnection(string ip)
	{
			connectTo(ip);
	}

	bool chatSocketConnection::connectComplete(string ip)
	{
		connectTo(ip);
		return connect();
	}

	void chatSocketConnection::connectTo(string ip)
	{
		sockfd = socket(AF_INET,SOCK_STREAM, 0);
		addy.sin_family = AF_INET;
		addy.sin_port =  htons(1028);
		addy.sin_addr.s_addr = inet_addr(ip.c_str());
		bind(sockfd, (struct sockaddr*)&addy, 0);
	}


	bool chatSocketConnection::connect() {

		signal(SIGPIPE, SIG_IGN);

		if( ::connect(sockfd, (struct sockaddr*)&addy, sizeof(struct sockaddr_in)) < 0 ) {
			throw   mx::mxException<std::string>(" Error could not connect ");
		}



		return true;
	}
	string chatSocketConnection::recv()
	{
		char  buffer[100];
		int len = ::recv(sockfd, buffer, sizeof(buffer), 0);
		//if(len == -1 || len == 0) return "";
		if(len == -1) throw mx::mxException<std::string>("Error disconnected");

		buffer[len] = 0;
		return  buffer;
	}

	int chatSocketConnection::send(string data) {

		int len = 0;
		len = ::send(sockfd, data.c_str(), data.length(), 0);

		return len;
	}

	void chatSocketConnection::closeConnection() {
		close(sockfd);
	}

	void chatDimension::Send() {
		std::string str = edit_line->getText();
		edit_line->setText("");
		online.send(str);
	}

	void chatDimension::Login() {

		try {

			if( online.connectComplete(TEMP_IP) == true) {
				connect_status->setText(Color(255,0,0), "Connected");
				sign_in->setTitle("Disconnect");
				char data[7];
				int sockfd = (int)(online.getsockfd());
				recv(sockfd, data, 7, 0);

				std::string strshake = "*" + handle_edit->getText();
				send(sockfd, strshake.c_str(), strshake.length(), 0);
				chat_window->show();
				login_window->hide();
				setFocus(chat_window->getID());

				socketListener listener(this);
				boost::thread lt(listener);
			}

		}
		catch(mx::mxException<std::string> &e)
		{
			connect_status->setText(Color(0,0,0), e.data);
		}

	}

	void chatDimension::Disconnect()
	{
		online.closeConnection();
		connect_status->setText(Color(255,255,255), "Disconnected");
		sign_in->setTitle("Sign in");
		chat_window->hide();
		img->show();
		login_window->show();

	}

}



