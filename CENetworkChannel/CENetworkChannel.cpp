#include "StdAfx.h"
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SSLSocketMgr.h"
#include "CENetworkChannel.h"

CSSLSocketMgr *g_pCSSSocket = NULL;

INT CENETAPI InitNetworkChannel( CHAR szRemoteHostIP[], INT nRemoteHostPort, BOOL secure, INT nProtocols )
/// <summary>
/// Init the network channel
/// </summary>
/// <param name="szRemoteHostIP">remote host name</param>
/// <param name="nRemoteHostPort">remote port name</param>
/// <param name="bSecure">use SSL/TLS or not</param>
/// <param name="nProtocols">SSL/TLX communication protocols</param>
/// <returns>Channel handle, handle >= 0</returns>
{
	if( g_pCSSSocket == NULL ) {
		g_pCSSSocket = new CSSLSocketMgr();

		// check new result
		if( g_pCSSSocket == NULL ) {
			return INVALID_SOCKET;
		}
	}

	return g_pCSSSocket->InitNetworkChannel( szRemoteHostIP, nRemoteHostPort, secure, nProtocols );
}

void CENETAPI DeinitNetworkChannel( INT nChHandle )
/// <summary>
/// Deinit the channel and release the resource
/// </summary>
/// <param name="nChHandle">channel handle</param>
{
	if( g_pCSSSocket == NULL ) {
		return;
	}

	g_pCSSSocket->DeinitNetworkChannel( nChHandle );
}

INT CENETAPI Receive( INT nChHandle, BYTE buffer[], INT Length, INT timeout )
/// <summary>
/// Receive data from the network channel with a specified timeout
/// </summary>
/// <param name="nChHandle">Socket handle</param>
/// <param name="buffer">Data buffer for receiving data</param>
/// <param name="Length">Data buffer length</param>
/// <param name="timeout">Timeout on receiving (in milliseconds) Note: note support now</param>
/// <returns>Number of bytes received</returns>
{
	if( g_pCSSSocket == NULL ) {
		return 0;
	}

	return g_pCSSSocket->Receive( nChHandle, buffer, Length, timeout );
}

INT CENETAPI Send( INT nChHandle, BYTE buffer[], INT Length )
/// <summary>
/// Send data on the network channel to the broker
/// </summary>
/// <param name="nChHandle">Socket handle</param>
/// <param name="buffer">Data buffer to send</param>
/// <returns>Number of byte sent</returns>
{
	if( g_pCSSSocket == NULL ) {
		return 0;
	}

	return g_pCSSSocket->Send( nChHandle, buffer, Length );
}

void CENETAPI Close( INT nChHandle )
/// <summary>
/// Close the network channel
/// </summary>
/// <param name="nChHandle">Socket handle</param>
{
	if( g_pCSSSocket == NULL ) {
		return;
	}

	g_pCSSSocket->Close( nChHandle );
}

void CENETAPI Connect( INT nChHandle )
/// <summary>
/// Connect to remote server
/// </summary>
/// <param name="nChHandle">Socket handle</param>
{
	if( g_pCSSSocket == NULL ) {
		return;
	}

	g_pCSSSocket->Connect( nChHandle );
}

