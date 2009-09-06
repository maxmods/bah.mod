SuperStrict

Import BaH.Raknet

Import "source.bmx"


Extern

	Function bmx_raknetroomsplugin_new:Byte Ptr()
	Function bmx_raknetroomsplugin_SetOrderingChannel(handle:Byte Ptr, oc:Int)
	Function bmx_raknetroomsplugin_SetSendPriority(handle:Byte Ptr, pp:Int)
	Function bmx_raknetroomsplugin_ClearRoomMembers(handle:Byte Ptr)
	Function bmx_raknetroomsplugin_ClearLoginServerAdddresses(handle:Byte Ptr)
	Function bmx_raknetroomsplugin_SetRoomsCallback(handle:Byte Ptr, roomsCallback:Byte Ptr)
	Function bmx_raknetroomsplugin_ExecuteFuncAddress(handle:Byte Ptr, func:Byte Ptr, remoteAddress:Byte Ptr)
	Function bmx_raknetroomsplugin_ExecuteFunc(handle:Byte Ptr, func:Byte Ptr)
	Function bmx_raknetroomsplugin_SetServerAddress(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_raknetroomsplugin_LoginRoomsParticipant:Int(handle:Byte Ptr, userName:String, roomsParticipantAddress:Byte Ptr, guid:Byte Ptr, loginServerAddress:Byte Ptr)
	Function bmx_raknetroomsplugin_LogoffRoomsParticipant:Int(handle:Byte Ptr, userName:String, loginServerAddress:Byte Ptr)
	Function bmx_raknetroomsplugin_AddLoginServerAddress(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_raknetroomsplugin_RemoveLoginServerAddress(handle:Byte Ptr, systemAddress:Byte Ptr)
	Function bmx_raknetroomsplugin_SetProfanityFilter(handle:Byte Ptr, pf:Byte Ptr)
	Function bmx_raknetroomsplugin_RoomsContainer:Byte Ptr(handle:Byte Ptr)

	Function bmx_raknetroomscallback_new:Byte Ptr(handle:Object)
	
	Function bmx_profanityfilter_new:Byte Ptr()
	Function bmx_profanityfilter_HasProfanity:Int(handle:Byte Ptr, text:String)
	Function bmx_profanityfilter_FilterProfanity:String(handle:Byte Ptr, text:String, filter:Int, ret:Int Ptr)
	Function bmx_profanityfilter_Count:Int(handle:Byte Ptr)
	Function bmx_profanityfilter_AddWord(handle:Byte Ptr, newWord:String)
	Function bmx_profanityfilter_free(handle:Byte Ptr)

	Function bmx_allgamesroomcontainer_addtitle:Int(handle:Byte Ptr, gameIdentifier:String)

	Function bmx_roomspluginfunc_getresultcode:Int(handle:Byte Ptr)
	Function bmx_roomspluginfunc_getusername:String(handle:Byte Ptr)
	Function bmx_roomspluginfunc_setusername(handle:Byte Ptr, name:String)

	Function bmx_createroomfunc_create:Byte Ptr()
	Function bmx_createroomfunc_free(handle:Byte Ptr)
	Function bmx_createroomfunc_GetNetworkedRoomCreationParameters:Byte Ptr(handle:Byte Ptr)
	Function bmx_createroomfunc_GetGameIdentifer:String(handle:Byte Ptr)
	Function bmx_createroomfunc_SetGameIdentifer(handle:Byte Ptr, gameIdentifier:String)
	Function bmx_createroomfunc_GetRoomId:Int(handle:Byte Ptr)

	Function bmx_enterroomfunc_create:Byte Ptr()
	Function bmx_enterroomfunc_free(handle:Byte Ptr)
	Function bmx_enterroomfunc_GetNetworkedRoomCreationParameters:Byte Ptr(handle:Byte Ptr)
	Function bmx_enterroomfunc_GetGameIdentifer:String(handle:Byte Ptr)
	Function bmx_enterroomfunc_SetGameIdentifer(handle:Byte Ptr, gameIdentifier:String)
	Function bmx_enterroomfunc_GetJoinedRoomResult:Byte Ptr(handle:Byte Ptr)
	Function bmx_enterroomfunc_IsCreatedRoom:Int(handle:Byte Ptr)
	Function bmx_enterroomfunc_GetRoomId:Int(handle:Byte Ptr)
	Function bmx_enterroomfunc_SetRoomMemberMode(handle:Byte Ptr, _mode:Int)
	Function bmx_enterroomfunc_GetRoomMemberMode:Int(handle:Byte Ptr)
	Function bmx_enterroomfunc_GetQuery:Byte Ptr(handle:Byte Ptr)

	Function bmx_joinbyfilterfunc_create:Byte Ptr()
	Function bmx_joinbyfilterfunc_free(handle:Byte Ptr)
	Function bmx_joinbyfilterfunc_GetJoinedRoomResult:Byte Ptr(handle:Byte Ptr)
	Function bmx_joinbyfilterfunc_SetRoomMemberMode(handle:Byte Ptr, _mode:Int)
	Function bmx_joinbyfilterfunc_GetRoomMemberMode:Int(handle:Byte Ptr)
	Function bmx_joinbyfilterfunc_GetQuery:Byte Ptr(handle:Byte Ptr)
	Function bmx_joinbyfilterfunc_GetGameIdentifer:String(handle:Byte Ptr)
	Function bmx_joinbyfilterfunc_SetGameIdentifer(handle:Byte Ptr, gameIdentifier:String)

	Function bmx_roomserrorcodedescription_ToEnglish:String(code:Int)

	Function bmx_networkedroomcreationparameters_GetSlots:Byte Ptr(handle:Byte Ptr)
	Function bmx_networkedroomcreationparameters_GetRoomName:String(handle:Byte Ptr)
	Function bmx_networkedroomcreationparameters_SetRoomName(handle:Byte Ptr, name:String)

	Function bmx_slots_GetPublicSlots:Int(handle:Byte Ptr)
	Function bmx_slots_SetPublicSlots(handle:Byte Ptr, slots:Int)
	Function bmx_slots_GetReservedSlots:Int(handle:Byte Ptr)
	Function bmx_slots_SetReservedSlots(handle:Byte Ptr, slots:Int)
	Function bmx_slots_GetSpectatorSlots:Int(handle:Byte Ptr)
	Function bmx_slots_SetSpectatorSlots(handle:Byte Ptr, slots:Int)

	Function bmx_roomdescriptor_getroommemberlistsize:Int(handle:Byte Ptr)
	Function bmx_roomdescriptor_getpropertystring:String(handle:Byte Ptr, index:Int)
	Function bmx_roomdescriptor_getpropertyvalue:Double(handle:Byte Ptr, index:Int)
	Function bmx_roomdescriptor_getlobbyroomid:Int(handle:Byte Ptr)

	Function bmx_joinedroomresult_getroomdescriptor:Byte Ptr(handle:Byte Ptr)
	Function bmx_joinedroomresult_getacceptedinvitor:Byte Ptr(handle:Byte Ptr)
	Function bmx_joinedroomresult_getacceptedinvitorname:String(handle:Byte Ptr)
	Function bmx_joinedroomresult_getacceptedinvitoraddress:Byte Ptr(handle:Byte Ptr)
	Function bmx_joinedroomresult_getjoiningmember:Byte Ptr(handle:Byte Ptr)
	Function bmx_joinedroomresult_getjoiningmembername:String(handle:Byte Ptr)
	Function bmx_joinedroomresult_getjoiningmemberaddress:Byte Ptr(handle:Byte Ptr)
	Function bmx_joinedroomresult_getroomoutput:Byte Ptr(handle:Byte Ptr)

	Function bmx_roomquery_AddQuery_NUMERIC(handle:Byte Ptr, columnName:String, numericValue:Double, op:Int)
	Function bmx_roomquery_AddQuery_STRING(handle:Byte Ptr, columnName:String, charValue:String, op:Int)
	Function bmx_roomquery_AddQuery_BINARY(handle:Byte Ptr, columnName:String, data:Byte Ptr, inputLength:Int, op:Int)

	Function bmx_defaultroomcolumns_GetColumnName:String(columnId:Int)

	Function bmx_chatfunc_create:Byte Ptr()
	Function bmx_chatfunc_SetChatMessage(handle:Byte Ptr, message:String)
	Function bmx_chatfunc_SetPrivateMessageRecipient(handle:Byte Ptr, name:String)
	Function bmx_chatfunc_SetChatDirectedToRoom(handle:Byte Ptr, value:Int)
	Function bmx_chatfunc_free(handle:Byte Ptr)

	Function bmx_leaveroomfunc_create:Byte Ptr()
	Function bmx_leaveroomfunc_free(handle:Byte Ptr)

	Function bmx_getinvitestoparticipantfunc_create:Byte Ptr()
	Function bmx_getinvitestoparticipantfunc_free(handle:Byte Ptr)

	Function bmx_sendinvitefunc_create:Byte Ptr()
	Function bmx_sendinvitefunc_free(handle:Byte Ptr)

	Function bmx_acceptinvitefunc_create:Byte Ptr()
	Function bmx_acceptinvitefunc_free(handle:Byte Ptr)

	Function bmx_startspectatingfunc_create:Byte Ptr()
	Function bmx_startspectatingfunc_free(handle:Byte Ptr)

	Function bmx_stopspectatingfunc_create:Byte Ptr()
	Function bmx_stopspectatingfunc_free(handle:Byte Ptr)

	Function bmx_grantmoderatorfunc_create:Byte Ptr()
	Function bmx_grantmoderatorfunc_free(handle:Byte Ptr)

	Function bmx_changeslotcountsfunc_create:Byte Ptr()
	Function bmx_changeslotcountsfunc_free(handle:Byte Ptr)


	Function bmx_roomspluginnotification_GetRecipient:String(handle:Byte Ptr)

	Function bmx_chatnotification_GetSender:String(handle:Byte Ptr)
	Function bmx_chatnotification_GetPrivateMessageRecipient:String(handle:Byte Ptr)
	Function bmx_chatnotification_GetChatMessage:String(handle:Byte Ptr)
	Function bmx_chatnotification_GetFilteredChatMessage:String(handle:Byte Ptr)

End Extern



Const RPO_CREATE_ROOM:Int = 0
Const RPO_ENTER_ROOM:Int = 1
Const RPO_JOIN_BY_FILTER:Int = 2
Const RPO_LEAVE_ROOM:Int = 3
Const RPO_GET_INVITES_TO_PARTICIPANT:Int = 4
Const RPO_SEND_INVITE:Int = 5
Const RPO_ACCEPT_INVITE:Int = 6
Const RPO_START_SPECTATING:Int = 7
Const RPO_STOP_SPECTATING:Int = 8
Const RPO_GRANT_MODERATOR:Int = 9
Const RPO_CHANGE_SLOT_COUNTS:Int = 10
Const RPO_SET_CUSTOM_ROOM_PROPERTIES:Int = 11
Const RPO_CHANGE_ROOM_NAME:Int = 12
Const RPO_SET_HIDDEN_FROM_SEARCHES:Int = 13
Const RPO_SET_DESTROY_ON_MODERATOR_LEAVE:Int = 14
Const RPO_SET_READY_STATUS:Int = 15
Const RPO_GET_READY_STATUS:Int = 16
Const RPO_SET_ROOM_LOCK_STATE:Int = 17
Const RPO_GET_ROOM_LOCK_STATE:Int = 18
Const RPO_ARE_ALL_MEMBERS_READY:Int = 19
Const RPO_KICK_MEMBER:Int = 20
Const RPO_UNBAN_MEMBER:Int = 21
Const RPO_GET_BAN_REASON:Int = 22
Const RPO_ADD_USER_TO_QUICK_JOIN:Int = 23
Const RPO_REMOVE_USER_FROM_QUICK_JOIN:Int = 24
Const RPO_IS_IN_QUICK_JOIN:Int = 25
Const RPO_SEARCH_BY_FILTER:Int = 26
Const RPO_CHANGE_HANDLE:Int = 27
Const RPO_ROOM_CHAT:Int = 28
Const RPN_QUICK_JOIN_EXPIRED:Int = 29
Const RPN_QUICK_JOIN_ENTERED_ROOM:Int = 30
Const RPN_ROOM_MEMBER_STARTED_SPECTATING:Int = 31
Const RPN_ROOM_MEMBER_STOPPED_SPECTATING:Int = 32
Const RPN_MODERATOR_CHANGED:Int = 33
Const RPN_SLOT_COUNTS_SET:Int = 34
Const RPN_CUSTOM_ROOM_PROPERTIES_SET:Int = 35
Const RPN_ROOM_NAME_SET:Int = 36
Const RPN_HIDDEN_FROM_SEARCHES_SET:Int = 37
Const RPN_ROOM_MEMBER_READY_STATUS_SET:Int = 38
Const RPN_ROOM_LOCK_STATE_SET:Int = 39
Const RPN_ROOM_MEMBER_KICKED:Int = 40
Const RPN_ROOM_MEMBER_HANDLE_SET:Int = 41
Const RPN_ROOM_MEMBER_LEFT_ROOM:Int = 42
Const RPN_ROOM_MEMBER_JOINED_ROOM:Int = 43
Const RPN_ROOM_INVITATION_SENT:Int = 44
Const RPN_ROOM_INVITATION_WITHDRAWN:Int = 45
Const RPN_ROOM_DESTROYED_ON_MODERATOR_LEFT:Int = 46
Const RPN_ROOM_CHAT_NOTIFICATION:Int = 47

Const REC_SUCCESS:Int = 0
Const REC_NOT_LOGGED_IN:Int = 1 ' Generic error when the requester of an operation is not logged in
Const REC_ADD_TO_ROOM_NO_RESERVED_OR_PUBLIC:Int = 2
Const REC_ADD_TO_ROOM_NO_PUBLIC:Int = 3
Const REC_ADD_TO_ROOM_NO_SPECTATOR:Int = 4
Const REC_ADD_TO_ROOM_ALREADY_IN_THIS_ROOM:Int = 5
Const REC_ADD_TO_ROOM_ALREADY_IN_ANOTHER_ROOM:Int = 6
Const REC_ADD_TO_ROOM_KICKED_OUT_OF_ROOM:Int = 7
Const REC_CHANGE_MEMBER_TYPE_NO_SLOTS:Int = 8
Const REC_SEARCH_BY_FILTER_UNKNOWN_TITLE:Int = 9
Const REC_JOIN_BY_FILTER_UNKNOWN_TITLE:Int = 10
Const REC_JOIN_BY_FILTER_NO_ROOMS:Int = 11
Const REC_JOIN_BY_FILTER_CURRENTLY_IN_A_ROOM:Int = 12
Const REC_JOIN_BY_FILTER_CURRENTLY_IN_QUICK_JOIN:Int = 13
Const REC_JOIN_BY_FILTER_CANNOT_JOIN_AS_MODERATOR:Int = 14
Const REC_JOIN_BY_FILTER_ROOM_LOCKED:Int = 15
Const REC_JOIN_BY_FILTER_BANNED:Int = 16
Const REC_JOIN_BY_FILTER_NO_SLOTS:Int = 17
Const REC_JOIN_BY_QUICK_JOIN_CANNOT_JOIN_AS_MODERATOR:Int = 18
Const REC_JOIN_BY_QUICK_JOIN_ROOM_LOCKED:Int = 19
Const REC_JOIN_BY_QUICK_JOIN_BANNED:Int = 20
Const REC_JOIN_BY_QUICK_JOIN_NO_SLOTS:Int = 21
Const REC_ADD_TO_QUICK_JOIN_CURRENTLY_IN_A_ROOM:Int = 22
Const REC_ADD_TO_QUICK_JOIN_UNKNOWN_TITLE:Int = 23
Const REC_ADD_TO_QUICK_JOIN_ALREADY_THERE:Int = 24
Const REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_LOW:Int = 25
Const REC_ADD_TO_QUICK_JOIN_INVALID_TIMEOUT_TOO_HIGH:Int = 26
Const REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_LOW:Int = 27
Const REC_ADD_TO_QUICK_JOIN_MINIMUM_SLOTS_TOO_HIGH:Int = 28
Const REC_REMOVE_FROM_QUICK_UNKNOWN_TITLE:Int = 29
Const REC_REMOVE_FROM_QUICK_JOIN_NOT_THERE:Int = 30
Const REC_CREATE_ROOM_UNKNOWN_TITLE:Int = 31
Const REC_CREATE_ROOM_CURRENTLY_IN_QUICK_JOIN:Int = 32
Const REC_CREATE_ROOM_CURRENTLY_IN_A_ROOM:Int = 33
Const REC_ROOM_CREATION_PARAMETERS_EMPTY_ROOM_NAME:Int = 34
Const REC_ROOM_CREATION_PARAMETERS_RESERVED_QUICK_JOIN_ROOM_NAME:Int = 35
Const REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_HAS_PROFANITY:Int = 36
Const REC_ROOM_CREATION_PARAMETERS_ROOM_NAME_IN_USE:Int = 37
Const REC_ROOM_CREATION_PARAMETERS_NO_PLAYABLE_SLOTS:Int = 38
Const REC_SET_ROOM_PROPERTIES_UNKNOWN_ROOM:Int = 39
Const REC_LEAVE_ROOM_UNKNOWN_ROOM_ID:Int = 40
Const REC_LEAVE_ROOM_CURRENTLY_IN_QUICK_JOIN:Int = 41
Const REC_LEAVE_ROOM_NOT_IN_ROOM:Int = 42
Const REC_ENTER_ROOM_UNKNOWN_TITLE:Int = 43
Const REC_ENTER_ROOM_CURRENTLY_IN_QUICK_JOIN:Int = 44
Const REC_ENTER_ROOM_CURRENTLY_IN_A_ROOM:Int = 45
Const REC_PROCESS_QUICK_JOINS_UNKNOWN_TITLE:Int = 46
Const REC_ROOM_QUERY_TOO_MANY_QUERIES:Int = 47
Const REC_ROOM_QUERY_INVALID_QUERIES_POINTER:Int = 48
Const REC_SEND_INVITE_UNKNOWN_ROOM_ID:Int = 49
Const REC_SEND_INVITE_INVITEE_ALREADY_INVITED:Int = 50
Const REC_SEND_INVITE_CANNOT_PERFORM_ON_SELF:Int = 51
Const REC_SEND_INVITE_INVITOR_ONLY_MODERATOR_CAN_INVITE:Int = 52 ' INVITE_MODE_MODERATOR_ONLY
Const REC_SEND_INVITE_INVITOR_LACK_INVITE_PERMISSIONS:Int = 53 ' Any other INVITE_MODE
Const REC_SEND_INVITE_INVITOR_NOT_IN_ROOM:Int = 54
Const REC_SEND_INVITE_NO_SLOTS:Int = 55
Const REC_SEND_INVITE_INVITEE_ALREADY_IN_THIS_ROOM:Int = 56
Const REC_SEND_INVITE_INVITEE_BANNED:Int = 57
Const REC_SEND_INVITE_RECIPIENT_NOT_ONLINE:Int = 58
Const REC_SEND_INVITE_ROOM_LOCKED:Int = 59
Const REC_ACCEPT_INVITE_UNKNOWN_ROOM_ID:Int = 60
Const REC_ACCEPT_INVITE_CURRENTLY_IN_A_ROOM:Int = 61
Const REC_ACCEPT_INVITE_CURRENTLY_IN_QUICK_JOIN:Int = 62
Const REC_ACCEPT_INVITE_BANNED:Int = 63
Const REC_ACCEPT_INVITE_NO_SLOTS:Int = 64
Const REC_ACCEPT_INVITE_ROOM_LOCKED:Int = 65
Const REC_ACCEPT_INVITE_NO_SUCH_INVITE:Int = 66
Const REC_SLOTS_VALIDATION_NO_PLAYABLE_SLOTS:Int = 67
Const REC_SLOTS_VALIDATION_NEGATIVE_PUBLIC_SLOTS:Int = 68
Const REC_SLOTS_VALIDATION_NEGATIVE_RESERVED_SLOTS:Int = 69
Const REC_SLOTS_VALIDATION_NEGATIVE_SPECTATOR_SLOTS:Int = 70
Const REC_START_SPECTATING_UNKNOWN_ROOM_ID:Int = 71
Const REC_START_SPECTATING_ALREADY_SPECTATING:Int = 72
Const REC_START_SPECTATING_NO_SPECTATOR_SLOTS_AVAILABLE:Int = 73
Const REC_START_SPECTATING_NOT_IN_ROOM:Int = 74
Const REC_START_SPECTATING_REASSIGN_MODERATOR_BEFORE_SPECTATE:Int = 75
Const REC_START_SPECTATING_ROOM_LOCKED:Int = 76
Const REC_STOP_SPECTATING_UNKNOWN_ROOM_ID:Int = 77
Const REC_STOP_SPECTATING_NOT_IN_ROOM:Int = 78
Const REC_STOP_SPECTATING_NOT_CURRENTLY_SPECTATING:Int = 79
Const REC_STOP_SPECTATING_NO_SLOTS:Int = 80
Const REC_STOP_SPECTATING_ROOM_LOCKED:Int = 81
Const REC_GRANT_MODERATOR_UNKNOWN_ROOM_ID:Int = 82
Const REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_ONLINE:Int = 83
Const REC_GRANT_MODERATOR_NOT_IN_ROOM:Int = 84
Const REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_ROOM:Int = 85
Const REC_GRANT_MODERATOR_CANNOT_PERFORM_ON_SELF:Int = 86
Const REC_GRANT_MODERATOR_MUST_BE_MODERATOR_TO_GRANT_MODERATOR:Int = 87
Const REC_GRANT_MODERATOR_NEW_MODERATOR_NOT_IN_PLAYABLE_SLOT:Int = 88
Const REC_CHANGE_SLOT_COUNTS_UNKNOWN_ROOM_ID:Int = 89
Const REC_CHANGE_SLOT_COUNTS_NOT_IN_ROOM:Int = 90
Const REC_CHANGE_SLOT_COUNTS_MUST_BE_MODERATOR:Int = 91
Const REC_SET_CUSTOM_ROOM_PROPERTIES_UNKNOWN_ROOM_ID:Int = 92
Const REC_SET_CUSTOM_ROOM_PROPERTIES_CONTAINS_DEFAULT_COLUMNS:Int = 93
Const REC_SET_CUSTOM_ROOM_PROPERTIES_NOT_IN_ROOM:Int = 94
Const REC_SET_CUSTOM_ROOM_PROPERTIES_MUST_BE_MODERATOR:Int = 95
Const REC_GET_ROOM_PROPERTIES_EMPTY_ROOM_NAME:Int = 96
Const REC_GET_ROOM_PROPERTIES_UNKNOWN_ROOM_NAME:Int = 97
Const REC_CHANGE_ROOM_NAME_UNKNOWN_ROOM_ID:Int = 98
Const REC_CHANGE_ROOM_NAME_NOT_IN_ROOM:Int = 99
Const REC_CHANGE_ROOM_NAME_MUST_BE_MODERATOR:Int = 100
Const REC_CHANGE_ROOM_NAME_HAS_PROFANITY:Int = 101
Const REC_CHANGE_ROOM_NAME_EMPTY_ROOM_NAME:Int = 102
Const REC_CHANGE_ROOM_NAME_NAME_ALREADY_IN_USE:Int = 103
Const REC_SET_HIDDEN_FROM_SEARCHES_UNKNOWN_ROOM_ID:Int = 104
Const REC_SET_HIDDEN_FROM_SEARCHES_NOT_IN_ROOM:Int = 105
Const REC_SET_HIDDEN_FROM_SEARCHES_MUST_BE_MODERATOR:Int = 106
Const REC_SET_DESTROY_ON_MODERATOR_LEAVE_UNKNOWN_ROOM_ID:Int = 107
Const REC_SET_DESTROY_ON_MODERATOR_LEAVE_NOT_IN_ROOM:Int = 108
Const REC_SET_DESTROY_ON_MODERATOR_LEAVE_MUST_BE_MODERATOR:Int = 109
Const REC_SET_READY_STATUS_UNKNOWN_ROOM_ID:Int = 110
Const REC_SET_READY_STATUS_NOT_IN_ROOM:Int = 111
Const REC_SET_READY_STATUS_NOT_IN_PLAYABLE_SLOT:Int = 112
Const REC_SET_READY_STATUS_AUTO_LOCK_ALL_PLAYERS_READY:Int = 113
Const REC_GET_READY_STATUS_NOT_IN_ROOM:Int = 114
Const REC_GET_READY_STATUS_UNKNOWN_ROOM_ID:Int = 115
Const REC_SET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID:Int = 116
Const REC_SET_ROOM_LOCK_STATE_NOT_IN_ROOM:Int = 117
Const REC_SET_ROOM_LOCK_STATE_MUST_BE_MODERATOR:Int = 118
Const REC_SET_ROOM_LOCK_STATE_BAD_ENUMERATION_VALUE:Int = 119
Const REC_GET_ROOM_LOCK_STATE_UNKNOWN_ROOM_ID:Int = 120
Const REC_GET_ROOM_LOCK_STATE_NOT_IN_ROOM:Int = 121
Const REC_ARE_ALL_MEMBERS_READY_UNKNOWN_ROOM_ID:Int = 122
Const REC_ARE_ALL_MEMBERS_READY_NOT_IN_ROOM:Int = 123
Const REC_KICK_MEMBER_UNKNOWN_ROOM_ID:Int = 124
Const REC_KICK_MEMBER_NOT_IN_ROOM:Int = 125
Const REC_KICK_MEMBER_TARGET_NOT_ONLINE:Int = 126
Const REC_KICK_MEMBER_TARGET_NOT_IN_YOUR_ROOM:Int = 127
Const REC_KICK_MEMBER_MUST_BE_MODERATOR:Int = 128
Const REC_KICK_MEMBER_CANNOT_PERFORM_ON_SELF:Int = 129
Const REC_GET_KICK_REASON_UNKNOWN_ROOM_ID:Int = 130
Const REC_GET_KICK_REASON_NOT_KICKED:Int = 131
Const REC_REMOVE_USER_NOT_IN_ROOM:Int = 132
Const REC_ADD_TITLE_ALREADY_IN_USE:Int = 133
Const REC_UNBAN_MEMBER_UNKNOWN_ROOM_ID:Int = 134
Const REC_UNBAN_MEMBER_NOT_IN_ROOM:Int = 135
Const REC_UNBAN_MEMBER_MUST_BE_MODERATOR:Int = 136
Const REC_UNBAN_MEMBER_NOT_BANNED:Int = 137
Const REC_GET_BAN_REASON_UNKNOWN_ROOM_ID:Int = 138
Const REC_GET_BAN_REASON_NOT_BANNED:Int = 139
Const REC_CHANGE_HANDLE_NEW_HANDLE_IN_USE:Int = 140
Const REC_CHANGE_HANDLE_CONTAINS_PROFANITY:Int = 141
Const REC_CHAT_USER_NOT_IN_ROOM:Int = 142
Const REC_CHAT_RECIPIENT_NOT_ONLINE:Int = 143
Const REC_CHAT_RECIPIENT_NOT_IN_ANY_ROOM:Int = 144
Const REC_CHAT_RECIPIENT_NOT_IN_YOUR_ROOM:Int = 145
Const REC_BITSTREAM_USER_NOT_IN_ROOM:Int = 146
Const REC_BITSTREAM_RECIPIENT_NOT_ONLINE:Int = 147
Const REC_BITSTREAM_RECIPIENT_NOT_IN_ANY_ROOM:Int = 148
Const REC_BITSTREAM_RECIPIENT_NOT_IN_YOUR_ROOM:Int = 149

Const RMM_MODERATOR:Int = 0
Const RMM_PUBLIC:Int = 1
Const RMM_RESERVED:Int = 2
Const RMM_SPECTATOR_PUBLIC:Int = 3
Const RMM_SPECTATOR_RESERVED:Int = 4
Const RMM_ANY_PLAYABLE:Int = 5
Const RMM_ANY_SPECTATOR:Int = 6


Const QF_EQUAL:Int = 0
Const QF_NOT_EQUAL:Int = 1
Const QF_GREATER_THAN:Int = 2
Const QF_GREATER_THAN_EQ:Int = 3
Const QF_LESS_THAN:Int = 4
Const QF_LESS_THAN_EQ:Int = 5
Const QF_IS_EMPTY:Int = 6
Const QF_NOT_EMPTY:Int = 7
