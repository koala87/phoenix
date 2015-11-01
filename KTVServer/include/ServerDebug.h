/**
 * Server Debug , Telnet Cmd
 * @author YuChun Zhang
 * @date 2014.01.22
 */
#pragma once

#include "net/TelNet.h"

namespace yiqiding{namespace ktv{


int showAllConnection(yiqiding::net::tel::ServerSend *srv);

int showAllVirtualConnection(yiqiding::net::tel::ServerSend *srv);

int showServerInfo(yiqiding::net::tel::ServerSend *srv);
}}





