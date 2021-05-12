#if !defined( _SSLSOCKET_MGR_H____INCLUDED_ )
#define _SSLSOCKET_MGR_H____INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif	  // _MSC_VER > 1000

class CSSLSocket;

class CSSLSocketMgr
{
public:
	// constructor
	CSSLSocketMgr( void );

	// distructor
	~CSSLSocketMgr( void );

	/// <summary>
	/// Init the network channel
	/// </summary>
	/// <param name="szRemoteHostIP">remote host IP</param>
	/// <param name="nRemoteHostPort">remote port name</param>
	/// <param name="bSecure">use SSL/TLS or not</param>
	/// <param name="nProtocols">SSL/TLS communication protocols</param>
	/// <returns>Channel handle, handle >= 0</returns>
	INT InitNetworkChannel( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL bSecure, INT nProtocols );

	/// <summary>
	/// Data available on channel
	/// </summary>
	/// <param name="nChHandle">Socket handle</param>
	BOOL DataAvailable( INT nChHandle );

	/// <summary>
	/// Receive data from the network channel with a specified timeout
	/// </summary>
	/// <param name="nChHandle">Socket handle</param>
	/// <param name="buffer">Data buffer for receiving data</param>
	/// <param name="Length">Data buffer length</param>
	/// <param name="timeout">Timeout on receiving (in milliseconds) Note: note support now</param>
	/// <returns>Number of bytes received</returns>
	INT Receive( INT nChHandle, BYTE buffer[], INT Length, INT timeout = 0 );

	/// <summary>
	/// Send data on the network channel to the broker
	/// </summary>
	/// <param name="nChHandle">Socket handle</param>
	/// <param name="buffer">Data buffer to send</param>
	/// <param name="Length">Data buffer length</param>
	/// <returns>Number of byte sent</returns>
	INT Send( INT nChHandle, BYTE buffer[], INT Length );

	/// <summary>
	/// Close the network channel
	/// </summary>
	/// <param name="nChHandle">Socket handle</param>
	void Close( INT nChHandle );

	/// <summary>
	/// Connect to remote server
	/// </summary>
	/// <param name="nChHandle">Socket handle</param>
	void Connect( INT nChHandle );

private:
	// flag of winsock is init or not
	static BOOL m_bInitWinSocket;

	// SSL socket list
	std::vector<CSSLSocket *> m_SSLSocketList;

private:
	/// <summary>
	/// registry socket
	/// </summary>
	/// <param name="pSSLSocket">SSL socket want to registry</param>
	/// <returns>socket handle</returns>
	INT RegistrySocket( CSSLSocket *pSSLSocket );

};

#endif	  // !defined( _SSLSOCKET_MGR_H____INCLUDED_ )
