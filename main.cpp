#include "mbed.h"
#include <MQTTClientMbedOs.h>
#include "CellularContext.h"
#include "CellularInformation.h"
// main() runs in its own thread in the OS

const char test_cert[] = "\
-----BEGIN CERTIFICATE-----\n\
MIIEAzCCAuugAwIBAgIUBY1hlCGvdj4NhBXkZ/uLUZNILAwwDQYJKoZIhvcNAQELBQAwgZAxCzAJBgNVBAYTAkdCMRcwFQYDVQQIDA5Vbml0ZWQgS2luZ2RvbTEOMAwGA1UEBwwFRGVyYnkxEjAQBgNVBAoMCU1vc3F1aXR0bzELMAkGA1UECwwCQ0ExFjAUBgNVBAMMDW1vc3F1aXR0by5vcmcxHzAdBgkqhkiG9w0BCQEWEHJvZ2VyQGF0Y2hvby5vcmcwHhcNMjAwNjA5MTEwNjM5WhcNMzAwNjA3MTEwNjM5WjCBkDELMAkGA1UEBhMCR0IxFzAVBgNVBAgMDlVuaXRlZCBLaW5nZG9tMQ4wDAYDVQQHDAVEZXJieTESMBAGA1UECgwJTW9zcXVpdHRvMQswCQYDVQQLDAJDQTEWMBQGA1UEAwwNbW9zcXVpdHRvLm9yZzEfMB0GCSqGSIb3DQEJARYQcm9nZXJAYXRjaG9vLm9yZzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAME0HKmIzfTOwkKLT3THHe+ObdizamPgUZmD64Tf3zJdNeYGYn4CEXbyP6fy3tWc8S2boW6dzrH8SdFf9uo320GJA9B7U1FWTe3xda/Lm3JFfaHjkWw7jBwcauQZjpGINHapHRlpiCZsquAthOgxW9SgDgYlGzEAs06pkEFiMw+qDfLo/sxFKB6vQlFekMeCymjLCbNwPJyqyhFmPWwio/PDMruBTzPH3cioBnrJWKXc3OjXdLGFJOfj7pP0j/dr2LH72eSvv3PQQFl90CZPFhrCUcRHSSxoE6yjGOdnz7f6PveLIB574kQORwt8ePn0yidrTC1ictikED3nHYhMUOUCAwEAAaNTMFEwHQYDVR0OBBYEFPVV6xBUFPiGKDyo5V3+Hbh4N9YSMB8GA1UdIwQYMBaAFPVV6xBUFPiGKDyo5V3+Hbh4N9YSMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAGa9kS21N70ThM6/Hj9D7mbVxKLBjVWe2TPsGfbl3rEDfZ+OKRZ2j6AC6r7jb4TZO3dzF2p6dgbrlU71Y/4K0TdzIjRj3cQ3KSm41JvUQ0hZ/c04iGDg/xWf+pp58nfPAYwuerruPNWmlStWAXf0UTqRtg4hQDWBuUFDJTuWuuBvEXudz74eh/wKsMwfu1HFvjy5Z0iMDU8PUDepjVolOCue9ashlS4EB5IECdSR2TItnAIiIwimx839LdUdRudafMu5T5Xma182OC0/u/xRlEm+tvKGGmfFcN0piqVl8OrSPBgIlb+1IKJEm/XriWr/Cq4h/JfB7NTsezVslgkBaoU=\n\
-----END CERTIFICATE-----\n";


int main()
{
    // Open MQTT connection to thingsboard, and send a message every five seconds
    //mbed_trace_init();
    CellularInterface  *net = CellularInterface::get_default_instance();
    CellularDevice * dev = CellularDevice::get_default_instance();
    //CellularInformation * devinf = CellularInformation::get_default_instance();
    ATHandler * devat = dev->get_at_handler();
    devat->
    TLSSocket socket;
    MQTTClient client(&socket);
    SocketAddress addr;
    nsapi_error_t e = 0;
    char iccid[64];
    //socket.set_timeout(1000);
    printf("socket.set_root_ca = %i\r\n", socket.set_root_ca_cert(test_cert));
    socket.set_hostname("test.mosquitto.org");

    if (!net) {
        printf("Could not get NetworkInterface.\r\n");
        while(1);
    }


    printf("net->connect       = %i\r\n", net->connect());

    do {
        e = net->get_connection_status();
        printf("net->status    = %i\r\n", e);
        ThisThread::sleep_for(1000);
    } while(e != 1);

    if (!dev->open_information()->get_iccid(iccid, 64))
        printf("dev->iccid         = %s\r\n",iccid);
    else
        printf("dev->iccid         = ?\r\n");

    printf("net->gethostbyname = %i\r\n", net->gethostbyname("test.mosquitto.org", &addr));
    addr.set_port(8883);
    printf("                IP = %s:%u\r\n", addr.get_ip_address(),addr.get_port());
    printf("socket.open        = %i\r\n", socket.open(net));
    do {
        e = socket.connect(addr);
        printf("socket.connect = %i\r\n", e);
        ThisThread::sleep_for(1000);
    } while(e);

    MQTTPacket_connectData opts = MQTTPacket_connectData_initializer;
    //opts.username.cstring = (char *)"sc5cHP201FcNc4eS5e6R";
    opts.MQTTVersion = 3;
    opts.clientID.cstring = (char *)"foo";
    do {
        e = client.connect(opts);
        printf("client.connect = %i\r\n", e);
        ThisThread::sleep_for(1000);
    } while(e);
    while(1) {

        MQTT::Message message;
        // QoS 0
        char buf[100];
        sprintf(buf, "t = %u", time(NULL));
        message.qos = MQTT::QOS0;
        message.retained = false;
        message.dup = false;
        message.payload = (void*)buf;
        message.payloadlen = strlen(buf)+1;

        printf("client.publish = %i\r\n", client.publish("jtest/FOO",message));

        for (int i = 0; i < (12*5); i++) {
            ThisThread::sleep_for(5000);
            client.yield();
        }

    }
}

