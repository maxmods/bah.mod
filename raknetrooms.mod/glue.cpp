#include "RoomsPlugin.h"


extern "C" {

	RakNet::RoomsPlugin * bmx_raknetroomsplugin_new();
	void bmx_raknetroomsplugin_SetOrderingChannel(RakNet::RoomsPlugin * rooms, int oc);
	void bmx_raknetroomsplugin_SetSendPriority(RakNet::RoomsPlugin * rooms, PacketPriority pp);
	void bmx_raknetroomsplugin_ClearRoomMembers(RakNet::RoomsPlugin * rooms);
	void bmx_raknetroomsplugin_ClearLoginServerAdddresses(RakNet::RoomsPlugin * rooms);

}

RakNet::RoomsPlugin * bmx_raknetroomsplugin_new() {
	return new RakNet::RoomsPlugin();
}

void bmx_raknetroomsplugin_SetOrderingChannel(RakNet::RoomsPlugin * rooms, int oc) {
	rooms->SetOrderingChannel(static_cast<char>(oc));
}

void bmx_raknetroomsplugin_SetSendPriority(RakNet::RoomsPlugin * rooms, PacketPriority pp) {
	rooms->SetSendPriority(pp);
}

void bmx_raknetroomsplugin_ClearRoomMembers(RakNet::RoomsPlugin * rooms) {
	rooms->ClearRoomMembers();
}

void bmx_raknetroomsplugin_ClearLoginServerAdddresses(RakNet::RoomsPlugin * rooms) {
	rooms->ClearLoginServerAdddresses();
}



