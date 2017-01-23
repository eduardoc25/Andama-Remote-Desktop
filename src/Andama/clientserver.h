/* ***********************************************************************
 * Andama
 * (C) 2014 by Yiannis Bourkelis (hello@andama.org)
 *
 * This file is part of Andama.
 *
 * Andama is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Andama is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Andama.  If not, see <http://www.gnu.org/licenses/>.
 * ***********************************************************************/

#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

#ifdef WIN32
#define NOMINMAX
#include <stdio.h>
#include "winsock2.h"
#include <ws2tcpip.h>
#define bzero(b,len) (memset((b), '\0', (len)), (void) 0)
#define bcopy(b1,b2,len) (memmove((b2), (b1), (len)), (void) 0)
#define in_addr_t uint32_t
#pragma comment(lib, "Ws2_32.lib")

#else
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h> //gia to TCP_NODELAY
#endif

#include "helperfuncs.h"
#include <iostream> //std::cout
#include "vector"
#include <mutex>
#include <atomic>
#include <QThread>
#include "qdebug.h"
#include <errno.h>
#include <thread>
#include <array>
#include <random>

#include "PasswordProtection.h"
#include "clientserverprotocol.h"

using namespace helperfuncs;

const unsigned int PASSWORD_LENGTH = 4; // default 6

class clientserver : public QThread
{
        Q_OBJECT

public:
    clientserver();
    //const int PORT_NUMBER = 8099;

    std::string remoteIpAddress;
    int remotePort;
    bool isClientConnected = false; //prepei na orizetai i arxiki timi giati alliws einai random i timi tis

    enum msgType{ MSG_ID = 0,
                  MSG_CONNECTION_ACCEPTED = 1, //to remote pc me enimerwse oti to aitima syndesis egine apodekto
                  MSG_REMOTE_CLIENT_ACCEPTED = 2, //apodextika to aitima syndesis pou ekane to remote pc.ksekinw na stelnw screenshots
                  MSG_SCREENSHOT = 3,
                  MSG_SCREENSHOT_DIFF = 4,
                  MSG_SCREENSHOT_REQUEST = 5,
                  MSG_SCREENSHOT_DIFF_REQUEST = 6,
                  MSG_MOUSE = 7,
                  MSG_KEYBOARD = 8,
                  MSG_CONNECT_ID_NOT_FOUND = 9, // epistrefetai apo ton server otan to Remote ID pou zitithike apo to connect den vrethike
                  MSG_NO_INTERNET_CONNECTION = 10,
                  MSG_NO_PROXY_CONNECTION = 11,
                  MSG_REMOTE_COMPUTER_DISCONNECTED = 12,
                  MSG_LOCAL_PASSWORD_GENERATED = 13,
                  MSG_BAN_IP_WRONG_ID = 14, // synolika 5 bytes
                  MSG_WARNING_BAN_IP_WRONG_ID = 15, // synolika 2 bytes
                  MSG_CONNECT_PASSWORD_NOT_CORRECT = 16,
                  MSG_BAN_IP_WRONG_PWD = 17, // synolika 1 byte
                  MSG_WARNING_BAN_IP_WRONG_PWD = 18, // synolika 2 byte
                  MSG_ERROR_CANNOT_CONNECT_SAME_ID = 19, // synolika 2 byte
                  MSG_ERROR_APP_VERSION_NOT_ACCEPTED = 20, // (mono command)
                  MSG_ERROR_PROTOCOL_VERSION_NOT_ACCEPTED = 21 // (mono command)
                 };

    //password brute force attack protection
    static const int  MAX_WRONG_PWD_TRIES         = 10;  //10; //default: diplasio apo to MAX_WRONG_ID_TRIES_WARNING
    static const int  MAX_WRONG_PWD_TRIES_WARNING = 5;  //5;

#ifdef WIN32
static bool addWrongPasswordIPProtection(in_addr_t clientIP, SOCKET socketfd);
#else
static bool addWrongPasswordIPProtection(in_addr_t clientIP, int socketfd);
#endif
    static void resetWrongPasswordIPProtection(in_addr_t clientIP);

#ifdef WIN32
static bool isIPBannedForWrongPasswords(in_addr_t clientIP, SOCKET socketfd);
#else
static bool isIPBannedForWrongPasswords(in_addr_t clientIP, int socketfd);
#endif

    void start_protocol();
    void Connect(const std::vector<char> &remoteID, const std::vector<char> &remotePassword);
    void ConnectP2P(const std::vector<char> remotePassword);
    void RequestScreenshot();
    void RequestScreenshotDiff();
    void setConnectionState(connectionState state);
    connectionState getConnectionState();
    void sendDisconnectFromRemoteComputer();
    void sendHeartBeat();
    void setRemoteComputerOS(OS os);
    OS getRemoteComputerOS();

    std::string password;

#ifdef WIN32
    void setActiveSocket(const SOCKET socket);
    SOCKET getActiveSocket();

    static void cleanup(const SOCKET socketfd);
#else
    int getActiveSocket();
    void setActiveSocket(const int socket);

    static void cleanup(const int socketfd);
#endif
    void sendMouse(int x, int y, int button, int mouseEvent, int wheelDelta, int wheelDeltaSign, int wheelOrientation);
    void sendKeyboard(int portableVKey, int portableModifiers, int keyEvent);

    static void setRemotePassword(std::string password);

signals:
    void sig_messageRecieved(const clientServerProtocol *client, const int msgType,const std::vector<char> &vdata = std::vector<char>());
    void sig_exception(QString ex);

protected:
    void run(void);

private:
#ifdef WIN32
    SOCKET _activeSocket = INVALID_SOCKET;
#else
    int _activeSocket;
#endif

    std::atomic<long> diffRequestCounter;

    void error(const char *msg);
    void proccesCommand(const std::array<char, 1> &command);
    void createConnectCommandData(std::vector<char> &all_data, const std::vector<char> &remoteComputerID, const std::vector<char> &remoteComputerPassword);
    void createConnectP2PCommandData(std::vector<char> &all_data, const std::vector<char> remoteComputerPassword);
    OS _remoteComputerOS;

    std::string generateRandomPassword(int length);

    connectionState m_connection_state = disconnected;
    std::mutex connection_state_mutex;

    //random password generator
    //random ID generator
    std::string passwordCharset = "123456789123456789abcdefgkmnprstxz123456789"; //52 chars. Thelw na emfanizontai oi arithmpoi me megalyteri syxnotita. Exw aferaisei grammata poy moiazoun me arithoums p.x. o. Exw aferesei grammata poy mporei na mperdepsoun to xristi
    std::random_device rd;
    std::mt19937 mt = std::mt19937(rd());
    std::uniform_int_distribution<unsigned int> distribution = std::uniform_int_distribution<unsigned int>(0, passwordCharset.length() - 1);

    std::vector<char> cachedID; //apothikevei to cachedID pou stalthike apo ton server
};

#endif // CLIENTSERVER_H

