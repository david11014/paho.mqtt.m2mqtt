#include "StdAfx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "OpenSSLAdapter.h"
#include "SSLSocket.h"
#include "CENetworkChannel.h"

// init static member data
static BOOL g_bInitWinSocket = FALSE;

CSSLSocket *CENETAPI InitNetworkChannel( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL bSecure, INT nProtocols )
/// <summary>
/// Init the network channel
/// </summary>
/// <param name="szRemoteHostIP">remote host name</param>
/// <param name="nRemoteHostPort">remote port name</param>
/// <param name="bSecure">use SSL/TLS or not</param>
/// <param name="nProtocols">SSL/TLX communication protocols</param>
/// <returns>Channel handle, handle >= 0</returns>
{
	if( g_bInitWinSocket == FALSE ) {
		WSADATA wsaData;
		::WSAStartup( WINSOCK_VERSION, &wsaData );
		g_bInitWinSocket = TRUE;
	}

	CSSLSocket *pChHandle = new CSSLSocket( szRemoteHostIP, nRemoteHostPort, bSecure, ( SSLProtocols )nProtocols );

	return pChHandle;
}

void CENETAPI DeinitNetworkChannel( CSSLSocket *pChHandle )
/// <summary>
/// Deinit the channel and release the resource
/// </summary>
/// <param name="pChHandle">channel handle</param>
{
	if( pChHandle == NULL ) {
		return;
	}

	// make sure socket is closed
	pChHandle->Close();

	// free the socket
	delete pChHandle;
}

INT CENETAPI Receive( CSSLSocket *pChHandle, BYTE buffer[], INT Length, INT timeout )
/// <summary>
/// Receive data from the network channel with a specified timeout
/// </summary>
/// <param name="pChHandle">Channel handle</param>
/// <param name="buffer">Data buffer for receiving data</param>
/// <param name="Length">Data buffer length</param>
/// <param name="timeout">Timeout on receiving (in milliseconds) Note: note support now</param>
/// <returns>Number of bytes received</returns>
{
	if( pChHandle == NULL ) {
		return 0;
	}

	return pChHandle->Receive( buffer, Length, timeout );
}

INT CENETAPI Send( CSSLSocket *pChHandle, BYTE buffer[], INT Length )
/// <summary>
/// Send data on the network channel to the broker
/// </summary>
/// <param name="pChHandle">Channel handle</param>
/// <param name="buffer">Data buffer to send</param>
/// <returns>Number of byte sent</returns>
{
	if( pChHandle == NULL ) {
		return 0;
	}

	return pChHandle->Send( buffer, Length );
}

void CENETAPI Close( CSSLSocket *pChHandle )
/// <summary>
/// Close the network channel
/// </summary>
/// <param name="pChHandle">Channel handle</param>
{
	if( pChHandle == NULL ) {
		return;
	}

	pChHandle->Close();
}

BOOL CENETAPI Connect( CSSLSocket *pChHandle )
/// <summary>
/// Connect to remote server
/// </summary>
/// <param name="pChHandle">Channel handle</param>
{
	if( pChHandle == NULL ) {
		return FALSE;
	}

	return pChHandle->Connect();
}

void CENETAPI SetThreadProcessor( UINT nProcessorNumber )
/// <summary>
/// Set current thread processor number
/// </summary>
/// <param name="nProcessorNumber">processer number, if bigger then actual process count, then set 1</param>
{
	// check process number
	if( nProcessorNumber > CeGetTotalProcessors() ) {
		nProcessorNumber = 1;
	}

	CeSetThreadAffinity( GetCurrentThread(), nProcessorNumber );
}
