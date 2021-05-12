#if !defined( _SSLSOCKET_H____INCLUDED_ )
#define _SSLSOCKET_H____INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif	  // _MSC_VER > 1000

#define RECV_SIZE	   256
#define MAX_IP_STR_LEN ( 16 )	 // max ip addres length( xxx.xxx.xxx.xxx\0 )

enum SSLProtocols : INT {
	SSLP_None	 = 0,	 // non SSL
	SSLP_SSLv3	 = 1,	 // SSLv3
	SSLP_TLSv1_0 = 2,	 // TLS1.0
	SSLP_TLSv1_1 = 3,	 // TLS1.1
	SSLP_TLSv1_2 = 4,	 // TLS1.2
};

class CSSLSocket
{
public:
	// constructor
	CSSLSocket( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL bSecure, SSLProtocols Protocols );

	// distructor
	~CSSLSocket( void );

	// Data available on channel
	BOOL DataAvailable( void );

	// Receive data from the network channel with a specified timeout
	// Note: NOT support timeout now, it is equal non time out version
	INT Receive( BYTE buffer[], INT nLength, INT timeout );

	// Send data on the network channel to the broker
	INT Send( BYTE buffer[], INT nLength );

	// Close the network channel
	void Close( void );

	// Connect to remote server
	void Connect( void );

private:
	CHAR m_szRemoteHostIP[ MAX_IP_STR_LEN ];

	INT m_nRemoteHostPort;

	SSLProtocols m_Protocols;

	BOOL m_bSecure;

	BOOL m_bConnectSuccess;

	SOCKET m_Socket;

	SSL *m_pSSL;

	SSL_CTX *m_pSSLContext;

private:
	// init SSL, if init fail return FALSE
	BOOL InitSSL( void );

	// free the socket resouse
	void DeinitSocket( void );

	// create normal socket
	SOCKET CreateSocket( CHAR *ip, INT port );

	// create ssl context
	SSL_CTX *CreateSSLContext();
};

#endif	  // !defined( _SSLSOCKET_H____INCLUDED_ )
