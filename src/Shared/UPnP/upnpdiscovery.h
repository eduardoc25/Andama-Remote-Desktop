/* ***********************************************************************
 * Andama
 * (C) 2017 by Yiannis Bourkelis (hello@andama.org)
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

#ifndef UPNPDISCOVERY_H
#define UPNPDISCOVERY_H

#include <QUrl>
#include <vector>
#include <string>

#ifdef WIN32
#include "winsock2.h"
#endif

class UPnPDiscovery
{
public:
    UPnPDiscovery();

    //public variables

    //public methods
    std::vector<std::string> discoverDevices(const std::string &searchTarget, const char * localIPv4);//searchTarget can be: ssdp:all, upnp:rootdevice, etc

    void stopThread();

private:
    //private variables
    bool _stopThread = false;

#ifdef WIN32
    SOCKET udpSocket;
#else
    int udpSocket;
#endif



};

#endif // UPNPDISCOVERY_H