#include "DS_Table.h"

#ifndef __ROOM_TYPES_H
#define __ROOM_TYPES_H

enum RoomMemberMode
{
	RMM_MODERATOR,
	RMM_PUBLIC,
	RMM_RESERVED,
	RMM_SPECTATOR_PUBLIC,
	RMM_SPECTATOR_RESERVED,
	RMM_ANY_PLAYABLE,
	RMM_ANY_SPECTATOR,
};

struct DefaultRoomColumns
{
	enum
	{
		TC_TITLE_NAME,
		TC_TITLE_ID,
		TC_ROOM_NAME,
		TC_ROOM_ID,
		TC_TOTAL_SLOTS,
		TC_TOTAL_PUBLIC_PLUS_RESERVED_SLOTS,
		TC_USED_SLOTS,
		TC_USED_PUBLIC_PLUS_RESERVED_SLOTS,
		TC_REMAINING_SLOTS,
		TC_REMAINING_PUBLIC_PLUS_RESERVED_SLOTS,
		TC_TOTAL_PUBLIC_SLOTS,
		TC_TOTAL_RESERVED_SLOTS,
		TC_TOTAL_SPECTATOR_SLOTS,
		TC_USED_PUBLIC_SLOTS,
		TC_USED_RESERVED_SLOTS,
		TC_USED_SPECTATOR_SLOTS,
		TC_REMAINING_PUBLIC_SLOTS,
		TC_REMAINING_RESERVED_SLOTS,
		TC_REMAINING_SPECTATOR_SLOTS,
		TC_CREATION_TIME,
		TC_DESTROY_ON_MODERATOR_LEAVE,
		TC_LOBBY_ROOM_PTR,
		TC_TABLE_COLUMNS_COUNT
	} columnId;

	const char *columnName;
	DataStructures::Table::ColumnType columnType;

	static const char *GetColumnName(int columnId);
	static DataStructures::Table::ColumnType GetColumnType(int columnId);
	static bool HasColumnName(const char *columnName);
	static void AddDefaultColumnsToTable(DataStructures::Table *table);
	static bool HasDefaultColumns(DataStructures::Table *table);
};

#endif
