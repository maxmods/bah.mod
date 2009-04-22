/// \file
/// \brief Adds networking to AllGamesRoomsContainer. Lets you create, join, search, and destroy matchmaking rooms for players
///
/// This file is part of RakNet Copyright 2003 Jenkins Software LLC
///
/// Usage of RakNet is subject to the appropriate license agreement.


#ifndef __ROOMS_PLUGIN_H
#define __ROOMS_PLUGIN_H

class RakPeerInterface;
#include "RakNetTypes.h"
#include "PluginInterface2.h"
#include "DS_OrderedList.h"
#include "Export.h"
#include "ConnectionGraph.h"
#include "RoomsContainer.h"
#include "PacketPriority.h"

/// \defgroup ROOMS_GROUP RoomsPlugin
/// \ingroup PLUGINS_GROUP

namespace RakNet
{

/// Base class for rooms functionality
/// Operations performed on rooms are not in the RoomsPlugin - instead, each structure encapsulates one operation
struct RoomsPluginFunc {
	RoomsPluginFunc() {}
	virtual ~RoomsPluginFunc() {}
	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream)=0;
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream)=0;
	void PrintResult(void);
	// Result of this operation
	RoomsErrorCode resultCode;
	// Who is performing this operation. Input parameter
	RakNet::RakString userName;
};
/// Create a room. Each user can be in at most one room, so will fail if the user is already in a room
struct CreateRoom_Func : public RoomsPluginFunc {
	// Input parameters
	NetworkedRoomCreationParameters networkedRoomCreationParameters;
	GameIdentifier gameIdentifier;
	// Output parameters
	RoomID roomId;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Joins a room if possible. If not, creates a room.
struct EnterRoom_Func : public RoomsPluginFunc {
	
	// Input parameters
	NetworkedRoomCreationParameters networkedRoomCreationParameters;
	RoomMemberMode roomMemberMode;
	RoomQuery query;
	GameIdentifier gameIdentifier;

	// Output parameters
	JoinedRoomResult joinedRoomResult;
	bool createdRoom;
	RoomID roomId;

	void PrintResult(void)
	{
		if (resultCode!=REC_SUCCESS)
			printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
		else if (createdRoom)
			printf("%s created a new room\n", userName.C_String());
		else
			printf("%s entered room with %i members\n", userName.C_String(), joinedRoomResult.roomDescriptor.roomMemberList.Size());
	}

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Joins a room given the filter parameters and desired room member mode.
/// Room joins may fail if the room is locked, or does not have slots of the particular type
/// Room joins may succeed where they would otherwise fail if the user has an invitation to the room
struct JoinByFilter_Func : public RoomsPluginFunc {
	// Input parameters
	GameIdentifier gameIdentifier;
	RoomMemberMode roomMemberMode;
	RoomQuery query;

	// Output parameters
	JoinedRoomResult joinedRoomResult;

	void PrintResult(void)
	{
		if (resultCode!=REC_SUCCESS)
			printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
		else
		{
			printf("Joined room %s with id %i and %.0f used slots\n",
				joinedRoomResult.roomDescriptor.GetProperty(DefaultRoomColumns::TC_ROOM_NAME)->c, joinedRoomResult.roomDescriptor.lobbyRoomId,
				joinedRoomResult.roomDescriptor.GetProperty(DefaultRoomColumns::TC_USED_SLOTS)->i);
			for (unsigned int i=0; i < joinedRoomResult.roomDescriptor.roomMemberList.Size(); i++)
			{
				printf("%i. %s (%s)\n", i+1, joinedRoomResult.roomDescriptor.roomMemberList[i].name.C_String(), joinedRoomResult.roomDescriptor.roomMemberList[i].systemAddress.ToString());
			}
		}
	}


	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Leaves a room. You can leave at any time, even if the room is locked.
struct LeaveRoom_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters
	RemoveUserResult removeUserResult;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Gets all invitations to you to various rooms.
struct GetInvitesToParticipant_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters
	DataStructures::List<InvitedUser> invitedUsers;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Sends an invitation to someone.
/// Each user may have at most one invitation to the same room, although they have have invitations to multiple rooms
/// This may fail depending on the room settings - the moderator may not allow other users to send invitations
/// Invitations may be cleared when the moderator changes, depending on the room settings
struct SendInvite_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString inviteeName;
	bool inviteToSpectatorSlot;
	RakNet::RakString subject;
	RakNet::RakString body;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Accept an invitation from a user to a room.
/// Invitations are the only way to join reserved slots. If all reserved slots are full, will join a public slot if possible.
struct AcceptInvite_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString inviteSender;
	RoomID roomId;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Begin spectating. Spectators are considered room members that are not playing the game, only watching
/// Spectators do not count towards room ready states. The moderator can lock the room, preventing users from spectating, or not allow spectators at all
struct StartSpectating_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Stop spectating. This will rejoin the room as a player, using a reserved slot if we were invited, and a public slot if not or if there are no reserved slots
/// This may fail if the moderator locked the room, or if no slots are available.
struct StopSpectating_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Give moderator to another player. Moderators cannot be spectators.
struct GrantModerator_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString newModerator;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Change the allowed slot counts for the room. Setting fewer slot counts than the number of players does not kick out players, though it may prevent changing spectator status
struct ChangeSlotCounts_Func : public RoomsPluginFunc {
	// Input parameters
	Slots slots;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Sets a table of user-defined room properties
/// Fill out the table with the column name, type, and value.
/// These properties are read when searching for rooms, and can be used as query filters to only join rooms with specified properties
/// See RoomTypes.h for default room properties.
struct SetCustomRoomProperties_Func : public RoomsPluginFunc {
	// Input parameters
	DataStructures::Table table;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Given a named room, return the properties of that room, including member list
struct GetRoomProperties_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString roomName;
	// Output parameters
	RoomDescriptor roomDescriptor;

	void PrintResult(void)
	{
		
		if (resultCode!=REC_SUCCESS)
		{
			printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
		}
		else
		{
//			char out[8096];
			printf("room %s has %i columns and %.0f used slots\n", roomName.C_String(),
				roomDescriptor.roomProperties.GetColumnCount(), roomDescriptor.roomMemberList.Size());
			RakAssert(roomDescriptor.GetProperty(DefaultRoomColumns::TC_USED_SLOTS)->i==roomDescriptor.roomMemberList.Size());
	//		roomDescriptor.roomProperties.PrintColumnHeaders(out,8096,',');
	//		printf(out);
	//		roomDescriptor.roomProperties.PrintRow(out,8096,',',false,roomDescriptor.roomProperties.GetRowByIndex(0,0));
	//		printf(out);
		}
	}

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};

/// Change the name of the room
struct ChangeRoomName_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString newRoomName;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Set or unset the room hidden from searches. If a room is hidden from searches, it can only be joined through invitations
struct SetHiddenFromSearches_Func : public RoomsPluginFunc {
	// Input parameters
	bool hiddenFromSearches;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Sets or unsets to destroy the room when the moderator leaves the room
/// If false, the next moderator will be the oldest member that is not a spectator.
struct SetDestroyOnModeratorLeave_Func : public RoomsPluginFunc {
	// Input parameters
	bool destroyOnModeratorLeave;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Sets or unsets the user as flagged 'ready'
/// Ready means ready to play the game. This flag can be set or unset by each room member
/// When all players are ready, the can will presumably start (although you can use this flag however you want).
struct SetReadyStatus_Func : public RoomsPluginFunc {
	// Input parameters
	bool isReady;
	// Output parameters
	DataStructures::List<RakNet::RakString> readyUsers;
	DataStructures::List<RakNet::RakString> unreadyUsers;

	void PrintResult(void)
	{
		if (resultCode!=REC_SUCCESS)
		{
			printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
		}
		else
		{
			printf("SetReadyStatus_Func member ready states:\n");
			if (readyUsers.Size()>0)
			{
				printf("Ready: ");
				for (unsigned int i=0; i < readyUsers.Size(); i++)
				{
					printf("%s ", readyUsers[i].C_String());
				}

				printf("\n");
			}

			if (unreadyUsers.Size()>0)
			{
				printf("Unready: ");
				for (unsigned int i=0; i < unreadyUsers.Size(); i++)
				{
					printf("%s ", unreadyUsers[i].C_String());
				}

				printf("\n");
			}
			
		}
	}

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Gets the ready states for every user in the room, excluding spectators
struct GetReadyStatus_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters
	DataStructures::List<RakNet::RakString> readyUsers;
	DataStructures::List<RakNet::RakString> unreadyUsers;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Lock or unlock the room
/// If the room is locked, no further players can join regardless of the available room slots. This includes invited players.
/// Rooms default to unlocked
struct SetRoomLockState_Func : public RoomsPluginFunc {
	// Input parameters
	RoomLockState roomLockState;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Gets the lock state of the room
struct GetRoomLockState_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters
	RoomLockState roomLockState;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// If all members have been set to ready using SetReadyStatus_Func, this operation will set allReady to true.
struct AreAllMembersReady_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters
	bool allReady;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Kick a member out of the room. This will also automatically ban that member from rejoining as long as the moderator does not change, or the member is unbanned
struct KickMember_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString kickedMember;
	RakNet::RakString reason;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Allow a member previously kicked out of the room to rejoin.
struct UnbanMember_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString bannedMemberName;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// For a given room, get the \a reason parameter of KickMember_Func when we were kicked out.
struct GetBanReason_Func : public RoomsPluginFunc {
	// Input parameters
	RoomID roomId;
	// Output parameters
	RakNet::RakString reason;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Enter quick join mode
/// Quick join mode will wait until the specified timeout to try to automatically join a room each second.
/// A room will only be joined if enough quick join members can join at once to fill all playable slots in the room
/// Older rooms are given priority to quick join
/// If no rooms are available to join, but enough quick join members are present to create a room instead, this will be done. The room custom properties will be any equality comparisons with networkedQuickJoinUser.query
struct AddUserToQuickJoin_Func : public RoomsPluginFunc {
	// Input parameters
	NetworkedQuickJoinUser networkedQuickJoinUser;
	GameIdentifier gameIdentifier;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Leave quick join mode
struct RemoveUserFromQuickJoin_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Returns if you are waiting in quick join mode
struct IsInQuickJoin_Func : public RoomsPluginFunc {
	// Input parameters
	// Output parameters
	bool isInQuickJoin;

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Return all rooms that pass the \a roomQuery filter
/// Use onlyJoinable to further filter by rooms that you can join (not locked, not banned, public or invited slots)
struct SearchByFilter_Func : public RoomsPluginFunc {
	SearchByFilter_Func() {}
	~SearchByFilter_Func();
	// Input parameters
	GameIdentifier gameIdentifier;
	RoomQuery roomQuery;
	bool onlyJoinable;
	// Output parameters
	DataStructures::List<RoomDescriptor*> roomsOutput;

	void PrintResult(void)
	{
		if (resultCode!=REC_SUCCESS)
		{
			printf("Result for user %s: %s\n", userName.C_String(), RoomsErrorCodeDescription::ToEnglish(resultCode));
		}
		else
		{
			printf("Found %i rooms\n", roomsOutput.Size());
			for (unsigned int i=0; i < roomsOutput.Size(); i++)
			{
				// Default types such as room name are in RoomTypes.cpp see defaultRoomColumns
				// Also can be user defined
				printf("%i. %s\n", i+1,roomsOutput[i]->GetProperty(DefaultRoomColumns::TC_ROOM_NAME)->c);
			}
		}
	}

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Change your handle
struct ChangeHandle_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString newHandle;
	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
struct Chat_Func : public RoomsPluginFunc {
	// Input parameters
	RakNet::RakString chatMessage;
	// Leave recipient blank for all in room
	RakNet::RakString privateMessageRecipient;
	/// If true, only sends the chat message if the user is in the same room.
	/// If false, privateMessageRecipient must also be filled out
	bool chatDirectedToRoom;

	// Output parameters

	virtual void SerializeIn(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void SerializeOut(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// Base class for notification callbacks
/// recipient is always filled out so you know which user this callback applies to
struct RoomsPluginNotification {
	virtual ~RoomsPluginNotification() {}
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream)=0;
	virtual void PrintResult(void)=0;
	RakNet::RakString recipient;
};
/// The quick join duration has expired without joining or creating any rooms
struct QuickJoinExpired_Notification : public RoomsPluginNotification {
	NetworkedQuickJoinUser networkedQuickJoinUser;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("QuickJoinExpired_Notification to %s\n", recipient.C_String());}
};
/// Quick join succeeded, and you are now in a room
struct QuickJoinEnteredRoom_Notification : public RoomsPluginNotification {
	JoinedRoomResult joinedRoomResult;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("QuickJoinEnteredRoom_Notification to %s. roomId=%i\n", recipient.C_String(), joinedRoomResult.roomDescriptor.lobbyRoomId);}
};
/// Another room member has started spectating
struct RoomMemberStartedSpectating_Notification : public RoomsPluginNotification {
	RoomID roomId;
	RakNet::RakString userName;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomMemberStartedSpectating_Notification to %s\n", recipient.C_String());}
};
/// Another room member has stopped spectating
struct RoomMemberStoppedSpectating_Notification : public RoomsPluginNotification {
	RakNet::RakString userName;
	RoomID roomId;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomMemberStoppedSpectating_Notification to %s\n", recipient.C_String());}
};
/// The room has a new moderator (possibly you)
struct ModeratorChanged_Notification : public RoomsPluginNotification {
	RakNet::RakString newModerator;
	RakNet::RakString oldModerator;
	RoomID roomId;
	virtual void PrintResult(void) {printf("ModeratorChanged_Notification to %s\n", recipient.C_String());}
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
};
/// The slot counts in the room has changed
struct SlotCountsSet_Notification : public RoomsPluginNotification {
	Slots slots;
	RoomID roomId;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("SlotCountsSet_Notification to %s\n", recipient.C_String());}
};
/// The custom properties for the room has changed
struct CustomRoomPropertiesSet_Notification : public RoomsPluginNotification {
	RoomID roomId;
	// If tablePtr is set, that will be serialized. Otherwise, table will be serialized.
	// Deserialization is written to table
	DataStructures::Table *tablePtr;
	DataStructures::Table table;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("CustomRoomPropertiesSet_Notification to %s\n", recipient.C_String());}
};
/// The name of the room has been changed
struct RoomNameSet_Notification : public RoomsPluginNotification {
	RoomID roomId;
	RakNet::RakString oldName;
	RakNet::RakString newName;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomNameSet_Notification to %s\n", recipient.C_String());}
};
/// The room is now hidden, or no longer hidden, from searches
struct HiddenFromSearchesSet_Notification : public RoomsPluginNotification {
	RoomID roomId;
	bool hiddenFromSearches;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("HiddenFromSearchesSet_Notification to %s\n", recipient.C_String());}
};
/// Another room member has changed their ready status
struct RoomMemberReadyStatusSet_Notification : public RoomsPluginNotification {
	RoomID roomId;
	bool isReady;
	RakNet::RakString roomMember;

	// Current status of all room members
	DataStructures::List<RakNet::RakString> readyUsers;
	DataStructures::List<RakNet::RakString> unreadyUsers;

	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomMemberReadyStatusSet_Notification to %s\n", recipient.C_String());}
};
/// The room is now, or is no longer, locked
struct RoomLockStateSet_Notification : public RoomsPluginNotification {
	RoomID roomId;
	RoomLockState roomLockState;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomLockStateSet_Notification to %s\n", recipient.C_String());}
};
/// A room member has been kicked out of the room (possibly you)
struct RoomMemberKicked_Notification : public RoomsPluginNotification {
	RoomID roomId;
	RakNet::RakString kickedMember;
	RakNet::RakString moderator;
	RakNet::RakString reason;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomMemberKicked_Notification to %s\n", recipient.C_String());}
};
/// A room member has changed their handle
struct RoomMemberHandleSet_Notification : public RoomsPluginNotification {
	RoomID roomId;
	RakNet::RakString oldName;
	RakNet::RakString newName;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomMemberHandleSet_Notification to %s\n", recipient.C_String());}
};
/// A room member has left the room
struct RoomMemberLeftRoom_Notification : public RoomsPluginNotification {
	RoomID roomId;
	RakNet::RakString roomMember;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomMemberLeftRoom_Notification to %s\n", recipient.C_String());}
};
/// A room member has joined the room
struct RoomMemberJoinedRoom_Notification : public RoomsPluginNotification {
	RoomMemberJoinedRoom_Notification() {joinedRoomResult=0;}
	~RoomMemberJoinedRoom_Notification() {if (joinedRoomResult!=0) RakNet::OP_DELETE(joinedRoomResult, __FILE__, __LINE__);}
	RoomID roomId;
	JoinedRoomResult *joinedRoomResult;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void)
	{
		printf("RoomMemberJoinedRoom_Notification to %s: %s (%s) has joined the room.\n", recipient.C_String(), joinedRoomResult->joiningMemberName.C_String(), joinedRoomResult->joiningMemberAddress.ToString());
	}
};
/// You have received an invitation to a room
struct RoomInvitationSent_Notification : public RoomsPluginNotification {
	RakNet::RakString invitorName;
	RakNet::RakString inviteeName;
	bool inviteToSpectatorSlot;
	RakNet::RakString subject;
	RakNet::RakString body;
	RoomID roomId;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomInvitationSent_Notification to %s\n", recipient.C_String());}
};
/// A previous room invitation is no longer valid (possibly due to moderator change, or the room no longer exists)
struct RoomInvitationWithdrawn_Notification : public RoomsPluginNotification {
	InvitedUser invitedUser;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomInvitationWithdrawn_Notification to %s\n", recipient.C_String());}
};
/// The moderator has left the room, and everyone left is a spectator, or the room was set to be destroyed when the moderator left
/// You are no longer in the room
struct RoomDestroyedOnModeratorLeft_Notification : public RoomsPluginNotification {
	RakNet::RakString oldModerator;
	RoomID roomId;
	RoomDescriptor roomDescriptor;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("RoomDestroyedOnModeratorLeft_Notification to %s\n", recipient.C_String());}
};
/// You got a chat message from another user.
/// If you want to support ignoring chat messages from specific users, use Lobby2Client_PC::IsInIgnoreList
struct Chat_Notification : public RoomsPluginNotification {
	RakNet::RakString sender;
	// If filled in, this was directed to you. Otherwise it was directed to the room
	RakNet::RakString privateMessageRecipient;
	// The chat message
	RakNet::RakString chatMessage;
	// The chat message with profanity filtered, if you want that instead
	RakNet::RakString filteredChatMessage;
	virtual void Serialize(bool writeToBitstream, RakNet::BitStream *bitStream);
	virtual void PrintResult(void) {printf("Chat_Notification to %s\n", recipient.C_String());}
};
struct RoomsCallback
{
	// Results of calls
	virtual void CreateRoom_Callback( SystemAddress senderAddress, CreateRoom_Func *callResult) {}
	virtual void EnterRoom_Callback( SystemAddress senderAddress, EnterRoom_Func *callResult) {}
	virtual void JoinByFilter_Callback( SystemAddress senderAddress, JoinByFilter_Func *callResult) {}
	virtual void LeaveRoom_Callback( SystemAddress senderAddress, LeaveRoom_Func *callResult) {}
	virtual void GetInvitesToParticipant_Callback( SystemAddress senderAddress, GetInvitesToParticipant_Func *callResult) {}
	virtual void SendInvite_Callback( SystemAddress senderAddress, SendInvite_Func *callResult) {}
	virtual void AcceptInvite_Callback( SystemAddress senderAddress, AcceptInvite_Func *callResult) {}
	virtual void StartSpectating_Callback( SystemAddress senderAddress, StartSpectating_Func *callResult) {}
	virtual void StopSpectating_Callback( SystemAddress senderAddress, StopSpectating_Func *callResult) {}
	virtual void GrantModerator_Callback( SystemAddress senderAddress, GrantModerator_Func *callResult) {}
	virtual void ChangeSlotCounts_Callback( SystemAddress senderAddress, ChangeSlotCounts_Func *callResult) {}
	virtual void SetCustomRoomProperties_Callback( SystemAddress senderAddress, SetCustomRoomProperties_Func *callResult) {}
	virtual void GetRoomProperties_Callback( SystemAddress senderAddress, GetRoomProperties_Func *callResult) {}
	virtual void ChangeRoomName_Callback( SystemAddress senderAddress, ChangeRoomName_Func *callResult) {}
	virtual void SetHiddenFromSearches_Callback( SystemAddress senderAddress, SetHiddenFromSearches_Func *callResult) {}
	virtual void SetDestroyOnModeratorLeave_Callback( SystemAddress senderAddress, SetDestroyOnModeratorLeave_Func *callResult) {}
	virtual void SetReadyStatus_Callback( SystemAddress senderAddress, SetReadyStatus_Func *callResult) {}
	virtual void GetReadyStatus_Callback( SystemAddress senderAddress, GetReadyStatus_Func *callResult) {}
	virtual void SetRoomLockState_Callback( SystemAddress senderAddress, SetRoomLockState_Func *callResult) {}
	virtual void GetRoomLockState_Callback( SystemAddress senderAddress, GetRoomLockState_Func *callResult) {}
	virtual void AreAllMembersReady_Callback( SystemAddress senderAddress, AreAllMembersReady_Func *callResult) {}
	virtual void KickMember_Callback( SystemAddress senderAddress, KickMember_Func *callResult) {}
	virtual void UnbanMember_Callback( SystemAddress senderAddress, UnbanMember_Func *callResult) {}
	virtual void GetBanReason_Callback( SystemAddress senderAddress, GetBanReason_Func *callResult) {}
	virtual void AddUserToQuickJoin_Callback( SystemAddress senderAddress, AddUserToQuickJoin_Func *callResult) {}
	virtual void RemoveUserFromQuickJoin_Callback( SystemAddress senderAddress, RemoveUserFromQuickJoin_Func *callResult) {}
	virtual void IsInQuickJoin_Callback( SystemAddress senderAddress, IsInQuickJoin_Func *callResult) {}
	virtual void SearchByFilter_Callback( SystemAddress senderAddress, SearchByFilter_Func *callResult) {}
	virtual void ChangeHandle_Callback( SystemAddress senderAddress, ChangeHandle_Func *callResult) {}
	virtual void Chat_Callback( SystemAddress senderAddress, Chat_Func *callResult) {}
	// Notifications due to other room members
	virtual void QuickJoinExpired_Callback( SystemAddress senderAddress, QuickJoinExpired_Notification *notification) {}
	virtual void QuickJoinEnteredRoom_Callback( SystemAddress senderAddress, QuickJoinEnteredRoom_Notification *notification) {}
	virtual void RoomMemberStartedSpectating_Callback( SystemAddress senderAddress, RoomMemberStartedSpectating_Notification *notification) {}
	virtual void RoomMemberStoppedSpectating_Callback( SystemAddress senderAddress, RoomMemberStoppedSpectating_Notification *notification) {}
	virtual void ModeratorChanged_Callback( SystemAddress senderAddress, ModeratorChanged_Notification *notification) {}
	virtual void SlotCountsSet_Callback( SystemAddress senderAddress, SlotCountsSet_Notification *notification) {}
	virtual void CustomRoomPropertiesSet_Callback( SystemAddress senderAddress, CustomRoomPropertiesSet_Notification *notification) {}
	virtual void RoomNameSet_Callback( SystemAddress senderAddress, RoomNameSet_Notification *notification) {}
	virtual void HiddenFromSearchesSet_Callback( SystemAddress senderAddress, HiddenFromSearchesSet_Notification *notification) {}
	virtual void RoomMemberReadyStatusSet_Callback( SystemAddress senderAddress, RoomMemberReadyStatusSet_Notification *notification) {}
	virtual void RoomLockStateSet_Callback( SystemAddress senderAddress, RoomLockStateSet_Notification *notification) {}
	virtual void RoomMemberKicked_Callback( SystemAddress senderAddress, RoomMemberKicked_Notification *notification) {}
	virtual void RoomMemberHandleSet_Callback( SystemAddress senderAddress, RoomMemberHandleSet_Notification *notification) {}
	virtual void RoomMemberLeftRoom_Callback( SystemAddress senderAddress, RoomMemberLeftRoom_Notification *notification) {}
	virtual void RoomMemberJoinedRoom_Callback( SystemAddress senderAddress, RoomMemberJoinedRoom_Notification *notification) {}
	virtual void RoomInvitationSent_Callback( SystemAddress senderAddress, RoomInvitationSent_Notification *notification) {}
	virtual void RoomInvitationWithdrawn_Callback( SystemAddress senderAddress, RoomInvitationWithdrawn_Notification *notification) {}
	virtual void RoomDestroyedOnModeratorLeft_Callback( SystemAddress senderAddress, RoomDestroyedOnModeratorLeft_Notification *notification) {}
	virtual void Chat_Callback( SystemAddress senderAddress, Chat_Notification *notification) {}
};

/// \ingroup ROOMS_GROUP
/// \brief Used to create rooms for players where they can matchmake
/// A room is similar to the rooms you see in other lobby systems, where groups of players can join together in order to start a game match
/// Each player can be in at most one room
/// Each player name must be unique
/// Each room has one moderator, which can perform operations on the room such as kicking out members
/// This plugin networks the AllGamesRoomsContainer class, which performs the actual functionality
///
/// To use as a client:
/// 1. Connect to the server and attach the plugin as normal.
/// 2. Call SetServerAddress to tell the system where the server is.
/// 3. Call RoomsPlugin::SetRoomsCallback() with a pointer to a callback structure
/// 4. Fill in the input parameters of the desired structure(s)
/// 5. Call RoomsPlugin::ExecuteFunc with a pointer to the structure.
/// 6. Process the callback, which will contain the original input parameters, plus the new output parameters. All structures contain resultCode, which indicates if the operation was successful (REC_SUCCESS) or not (Anything else)
///
/// To use as a server:
/// 1. Start RakNet as usual, accepting connections and attaching the plugin
/// 2. Call RoomsPlugin::SetProfanityFilter() with the ProfanityFilter class, if desired
/// 3. Call RoomsPlugin::AddTitle() for each title (game) you want to support
/// 4. If you want other systems to be able to call RoomsPlugin::LoginRoomsParticipant(), call RoomsPlugin::AddLoginServerAddress() with the addresses of those systems
/// 5. As users go online, call RoomsPlugin::LoginRoomsParticipant(). Login and Logoff is up to you to implement (or rely on other systems, such as Lobby2)
/// 6. As users go offline, call RoomsPlugin::LogoffRoomsParticipant();
/// \sa AllGamesRoomsContainer
class RAK_DLL_EXPORT RoomsPlugin : public PluginInterface2, public RoomsCallback
{
public:
	RoomsPlugin();
	virtual ~RoomsPlugin();

	/// Ordering channel to send messages on
	void SetOrderingChannel(char oc);
	/// Send priority to send messages on
	void SetSendPriority(PacketPriority pp);

	// --------------------------------------------------------------------------------------------
	// Client room functions. Will transmit the command to the server specified with SetServerAddress();
	// --------------------------------------------------------------------------------------------

	/// Set the callback to get notification and ExecuteFunc() results
	void SetRoomsCallback(RoomsCallback *_roomsCallback);

	/// Execute a function, using the system address passed to SetServerAddress();
	void ExecuteFunc(RoomsPluginFunc *func);

	/// Execute a function, with a specific address
	void ExecuteFunc(RoomsPluginFunc *func, SystemAddress remoteAddress);

	/// Sets the remote server address that is running RoomsPlugin. Send calls will go to this function
	/// \param[in] systemAddress The remote system, which should be connected while calling client functions
	void SetServerAddress( SystemAddress systemAddress );

	// --------------------------------------------------------------------------------------------
	// Server functions.
	// --------------------------------------------------------------------------------------------

	/// Add a participant to the system
	/// Only participants can perform operations
	/// \param[in] userName A unique string identifying the user
	/// \param[in] roomsParticipantAddress The address of the user
	/// \param[in] guid The guid of the user
	/// \param[in] loginServerAddress The server adding this user. Use UNASSIGNED_SYSTEM_ADDRESS for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
	bool LoginRoomsParticipant(RakNet::RakString userName, SystemAddress roomsParticipantAddress, RakNetGUID guid, SystemAddress loginServerAddress);

	/// Removes a participant from the system
	/// \param[in] userName A unique string identifying the user
	/// \param[in] loginServerAddress The server removing. Use UNASSIGNED_SYSTEM_ADDRESS for not applicable. Otherwise, the address must previously have been added using AddLoginServerAddress() or the function will fail.
	bool LogoffRoomsParticipant(RakNet::RakString userName, SystemAddress loginServerAddress);

	/// Clear all users
	void ClearRoomMembers();

	static void SerializeLogin(RakNet::RakString userName, SystemAddress userAddress, RakNetGUID guid, RakNet::BitStream *bs);
	static void SerializeLogoff(RakNet::RakString userName, RakNet::BitStream *bs);

	static void SerializeChangeHandle(RakNet::RakString oldHandle, RakNet::RakString newHandle, RakNet::BitStream *bs);
	void ChangeHandle(RakNet::RakString oldHandle, RakNet::RakString newHandle);

	/// Add a SystemAddress to a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
	/// \param[in] systemAddress The address to add
	void AddLoginServerAddress(SystemAddress systemAddress);

	/// Remove a SystemAddress from a list that will be checked when LoginRoomsParticipant() and LogoffRoomsParticipant() is called
	/// \param[in] systemAddress The address to remove
	void RemoveLoginServerAddress(SystemAddress systemAddress);

	/// Remove all addresses added with AddLoginServerAddress()
	void ClearLoginServerAdddresses(void);

	/// Sets the profanity filter for the system to use (optional)
	/// If set, room names and player handles will be checked for profanity.
	/// Room invitations and other messages are not checked.
	/// \param[in] pf An instance of a profanity filter
	void SetProfanityFilter(ProfanityFilter *pf);

	/// Only used on the server. Locally perform any desired functions, such as logging off players
	/// Use AllGamesRoomsContainer::AddTitle() to add titles
	AllGamesRoomsContainer roomsContainer;

	// --------------------------------------------------------------------------------------------
	// Packet handling functions
	// --------------------------------------------------------------------------------------------
	virtual void OnDetach(void);
	virtual void OnShutdown(void);
	virtual void Update(void);
	virtual PluginReceiveResult OnReceive(Packet *packet);
	virtual void OnClosedConnection(SystemAddress systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason );

protected:
	void Clear(void);
	bool IsServer(void) const;
	void OnRoomsExecuteFunc(Packet *packet);
	void OnLoginStatus(Packet *packet);
	void OnHandleChange(Packet *packet);

	// Client and server data
	char orderingChannel;
	RoomsCallback * roomsCallback;
	PacketPriority packetPriority;
	ProfanityFilter *profanityFilter;
	RakNetTime lastUpdateTime;

	// Client data
	SystemAddress serverAddress;


	// Server data
	// Servers that can remotely trigger LoginRoomMember and LogoffRoomMember
	DataStructures::List<SystemAddress> loginServers;
	// Logged in room members
	struct RoomsPluginParticipant : public RoomsParticipant
	{
		RoomsPluginParticipant() {lastRoomJoined=(RoomID)-1;}
		RoomID lastRoomJoined;
	};
	static int RoomsPluginParticipantCompByRakString( const RakNet::RakString &key, RoomsPluginParticipant* const &data );
	DataStructures::OrderedList<RakNet::RakString, RoomsPluginParticipant*, RoomsPluginParticipantCompByRakString> roomsParticipants;

	RoomsPluginParticipant* GetParticipantByHandle(RakNet::RakString handle, SystemAddress systemAddress);
	RoomsPluginParticipant* ValidateUserHandle(RoomsPluginFunc* func, SystemAddress systemAddress);
	void ProcessRemoveUserResult(RemoveUserResult *removeUserResult);

	virtual void CreateRoom_Callback( SystemAddress senderAddress, CreateRoom_Func *callResult);
	virtual void EnterRoom_Callback( SystemAddress senderAddress, EnterRoom_Func *callResult);
	virtual void JoinByFilter_Callback( SystemAddress senderAddress, JoinByFilter_Func *callResult);
	virtual void LeaveRoom_Callback( SystemAddress senderAddress, LeaveRoom_Func *callResult);
	virtual void GetInvitesToParticipant_Callback( SystemAddress senderAddress, GetInvitesToParticipant_Func *callResult);
	virtual void SendInvite_Callback( SystemAddress senderAddress, SendInvite_Func *callResult);
	virtual void AcceptInvite_Callback( SystemAddress senderAddress, AcceptInvite_Func *callResult);
	virtual void StartSpectating_Callback( SystemAddress senderAddress, StartSpectating_Func *callResult);
	virtual void StopSpectating_Callback( SystemAddress senderAddress, StopSpectating_Func *callResult);
	virtual void GrantModerator_Callback( SystemAddress senderAddress, GrantModerator_Func *callResult);
	virtual void ChangeSlotCounts_Callback( SystemAddress senderAddress, ChangeSlotCounts_Func *callResult);
	virtual void SetCustomRoomProperties_Callback( SystemAddress senderAddress, SetCustomRoomProperties_Func *callResult);
	virtual void GetRoomProperties_Callback( SystemAddress senderAddress, GetRoomProperties_Func *callResult);
	virtual void ChangeRoomName_Callback( SystemAddress senderAddress, ChangeRoomName_Func *callResult);
	virtual void SetHiddenFromSearches_Callback( SystemAddress senderAddress, SetHiddenFromSearches_Func *callResult);
	virtual void SetDestroyOnModeratorLeave_Callback( SystemAddress senderAddress, SetDestroyOnModeratorLeave_Func *callResult);
	virtual void SetReadyStatus_Callback( SystemAddress senderAddress, SetReadyStatus_Func *callResult);
	virtual void GetReadyStatus_Callback( SystemAddress senderAddress, GetReadyStatus_Func *callResult);
	virtual void SetRoomLockState_Callback( SystemAddress senderAddress, SetRoomLockState_Func *callResult);
	virtual void GetRoomLockState_Callback( SystemAddress senderAddress, GetRoomLockState_Func *callResult);
	virtual void AreAllMembersReady_Callback( SystemAddress senderAddress, AreAllMembersReady_Func *callResult);
	virtual void KickMember_Callback( SystemAddress senderAddress, KickMember_Func *callResult);
	virtual void UnbanMember_Callback( SystemAddress senderAddress, UnbanMember_Func *callResult);
	virtual void GetBanReason_Callback( SystemAddress senderAddress, GetBanReason_Func *callResult);
	virtual void AddUserToQuickJoin_Callback( SystemAddress senderAddress, AddUserToQuickJoin_Func *callResult);
	virtual void RemoveUserFromQuickJoin_Callback( SystemAddress senderAddress, RemoveUserFromQuickJoin_Func *callResult);
	virtual void IsInQuickJoin_Callback( SystemAddress senderAddress, IsInQuickJoin_Func *callResult);
	virtual void SearchByFilter_Callback( SystemAddress senderAddress, SearchByFilter_Func *callResult);
	virtual void ChangeHandle_Callback( SystemAddress senderAddress, ChangeHandle_Func *callResult);
	virtual void Chat_Callback( SystemAddress senderAddress, Chat_Func *callResult);

	void ExecuteNotification(RoomsPluginNotification *func, RoomsPluginParticipant *recipient);
	void ExecuteNotificationToOtherRoomMembers(Room *room, RoomsPluginParticipant* roomsPluginParticipant, RoomsPluginNotification *notification);
};


enum RoomsPluginOperations
{
	RPO_CREATE_ROOM,
	RPO_ENTER_ROOM,
	RPO_JOIN_BY_FILTER,
	RPO_LEAVE_ROOM,
	RPO_GET_INVITES_TO_PARTICIPANT,
	RPO_SEND_INVITE,
	RPO_ACCEPT_INVITE,
	RPO_START_SPECTATING,
	RPO_STOP_SPECTATING,
	RPO_GRANT_MODERATOR,
	RPO_CHANGE_SLOT_COUNTS,
	RPO_SET_CUSTOM_ROOM_PROPERTIES,
	RPO_GET_ROOM_PROPERTIES,
	RPO_CHANGE_ROOM_NAME,
	RPO_SET_HIDDEN_FROM_SEARCHES,
	RPO_SET_DESTROY_ON_MODERATOR_LEAVE,
	RPO_SET_READY_STATUS,
	RPO_GET_READY_STATUS,
	RPO_SET_ROOM_LOCK_STATE,
	RPO_GET_ROOM_LOCK_STATE,
	RPO_ARE_ALL_MEMBERS_READY,
	RPO_KICK_MEMBER,
	RPO_UNBAN_MEMBER,
	RPO_GET_BAN_REASON,
	RPO_ADD_USER_TO_QUICK_JOIN,
	RPO_REMOVE_USER_FROM_QUICK_JOIN,
	RPO_IS_IN_QUICK_JOIN,
	RPO_SEARCH_BY_FILTER,
	RPO_CHANGE_HANDLE,
	RPO_CHAT,
	RPN_QUICK_JOIN_EXPIRED,
	RPN_QUICK_JOIN_ENTERED_ROOM,
	RPN_ROOM_MEMBER_STARTED_SPECTATING,
	RPN_ROOM_MEMBER_STOPPED_SPECTATING,
	RPN_MODERATOR_CHANGED,
	RPN_SLOT_COUNTS_SET,
	RPN_CUSTOM_ROOM_PROPERTIES_SET,
	RPN_ROOM_NAME_SET,
	RPN_HIDDEN_FROM_SEARCHES_SET,
	RPN_ROOM_MEMBER_READY_STATUS_SET,
	RPN_ROOM_LOCK_STATE_SET,
	RPN_ROOM_MEMBER_KICKED,
	RPN_ROOM_MEMBER_HANDLE_SET,
	RPN_ROOM_MEMBER_LEFT_ROOM,
	RPN_ROOM_MEMBER_JOINED_ROOM,
	RPN_ROOM_INVITATION_SENT,
	RPN_ROOM_INVITATION_WITHDRAWN,
	RPN_ROOM_DESTROYED_ON_MODERATOR_LEFT,
	RPN_CHAT_NOTIFICATION,
};

} // namespace RakNet

#endif
