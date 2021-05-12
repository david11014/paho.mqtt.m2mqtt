using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;

namespace uPLibrary.Networking.M2Mqtt
{

	/// <summary>
	/// MQTT SSL utility class
	/// </summary>
	public static class MqttSslUtility
	{
#if (!MF_FRAMEWORK_VERSION_V4_2 && !MF_FRAMEWORK_VERSION_V4_3 && !COMPACT_FRAMEWORK)
        public static SslProtocols ToSslPlatformEnum(MqttSslProtocols mqttSslProtocol)
        {
            switch (mqttSslProtocol)
            {
                case MqttSslProtocols.None:
                    return SslProtocols.None;
                case MqttSslProtocols.SSLv3:
                    return SslProtocols.Ssl3;
                case MqttSslProtocols.TLSv1_0:
                    return SslProtocols.Tls;
                case MqttSslProtocols.TLSv1_1:
                    return SslProtocols.Tls11;
                case MqttSslProtocols.TLSv1_2:
                    return SslProtocols.Tls12;
                default:
                    throw new ArgumentException("SSL/TLS protocol version not supported");
            }
        }
#elif (MF_FRAMEWORK_VERSION_V4_2 || MF_FRAMEWORK_VERSION_V4_3)
        public static SslProtocols ToSslPlatformEnum(MqttSslProtocols mqttSslProtocol)
        {
            switch (mqttSslProtocol)
            {
                case MqttSslProtocols.None:
                    return SslProtocols.None;
                case MqttSslProtocols.SSLv3:
                    return SslProtocols.SSLv3;
                case MqttSslProtocols.TLSv1_0:
                    return SslProtocols.TLSv1;
                case MqttSslProtocols.TLSv1_1:
                case MqttSslProtocols.TLSv1_2:
                default:
                    throw new ArgumentException("SSL/TLS protocol version not supported");
            }
        }
#elif ( COMPACT_FRAMEWORK )
		enum SSLProtocols {
			SSLP_None = 0,		// non SSL
			SSLP_SSLv3 = 1,		// SSLv3
			SSLP_TLSv1_0 = 2,	// TLS1.0
			SSLP_TLSv1_1 = 3,	// TLS1.1
			SSLP_TLSv1_2 = 4,	// TLS1.2
		};

		public static int ToSslPlatformEnum( MqttSslProtocols mqttSslProtocol )
		{
			switch( mqttSslProtocol ) {
				case MqttSslProtocols.None:
					return (int)SSLProtocols.SSLP_None;
				case MqttSslProtocols.SSLv3:
					return (int)SSLProtocols.SSLP_SSLv3;
				case MqttSslProtocols.TLSv1_0:
					return (int)SSLProtocols.SSLP_TLSv1_0;
				case MqttSslProtocols.TLSv1_1:
					return (int)SSLProtocols.SSLP_TLSv1_1;
				case MqttSslProtocols.TLSv1_2:
					return (int)SSLProtocols.SSLP_TLSv1_2;
				default:
					throw new ArgumentException( "SSL/TLS protocol version not supported" );
			}
		}
#endif
	}
}
