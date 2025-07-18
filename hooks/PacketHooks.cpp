/*
 * Copyright (C) 2010 - 2024 Eluna Lua Engine <https://elunaluaengine.github.io/>
 * This program is free software licensed under GPL version 3
 * Please see the included DOCS/LICENSE.md for more information
 */

#include "Hooks.h"
#include "HookHelpers.h"
#include "LuaEngine.h"
#include "BindingMap.h"
#include "ElunaIncludes.h"
#include "ElunaTemplate.h"

using namespace Hooks;

#define START_HOOK_SERVER(EVENT) \
    auto binding = GetBinding<EventKey<ServerEvents>>(REGTYPE_SERVER);\
    auto key = EventKey<ServerEvents>(EVENT);\
    if (!binding->HasBindingsFor(key))\
        return;

#define START_HOOK_PACKET(EVENT, OPCODE) \
    auto binding = GetBinding<EntryKey<PacketEvents>>(REGTYPE_PACKET);\
    auto key = EntryKey<PacketEvents>(EVENT, OPCODE);\
    if (!binding->HasBindingsFor(key))\
        return;

bool Eluna::OnPacketSend(WorldSession* session, const WorldPacket& packet)
{
    bool result = true;
    Player* player = NULL;
    if (session)
        player = session->GetPlayer();
    OnPacketSendAny(player, packet, result);
    OnPacketSendOne(player, packet, result);
    return result;
}
void Eluna::OnPacketSendAny(Player* player, const WorldPacket& packet, bool& result)
{
    START_HOOK_SERVER(SERVER_EVENT_ON_PACKET_SEND);
    HookPush(&packet); // pushing pointer to local is fine, a copy of value will be stored, not pointer itself
    HookPush(player);
    int n = SetupStack(binding, key, 2);

    while (n > 0)
    {
        int r = CallOneFunction(n--, 2, 1);

        if (lua_isboolean(L, r + 0) && !lua_toboolean(L, r + 0))
            result = false;

        lua_pop(L, 1);
    }

    CleanUpStack(2);
}

void Eluna::OnPacketSendOne(Player* player, const WorldPacket& packet, bool& result)
{
    START_HOOK_PACKET(PACKET_EVENT_ON_PACKET_SEND, packet.GetOpcode());
    HookPush(&packet); // pushing pointer to local is fine, a copy of value will be stored, not pointer itself
    HookPush(player);
    int n = SetupStack(binding, key, 2);

    while (n > 0)
    {
        int r = CallOneFunction(n--, 2, 1);

        if (lua_isboolean(L, r + 0) && !lua_toboolean(L, r + 0))
            result = false;

        lua_pop(L, 1);
    }

    CleanUpStack(2);
}

bool Eluna::OnPacketReceive(WorldSession* session, WorldPacket& packet)
{
    bool result = true;
    Player* player = NULL;
    if (session)
        player = session->GetPlayer();
    OnPacketReceiveAny(player, packet, result);
    OnPacketReceiveOne(player, packet, result);
    return result;
}

void Eluna::OnPacketReceiveAny(Player* player, WorldPacket& packet, bool& result)
{
    START_HOOK_SERVER(SERVER_EVENT_ON_PACKET_RECEIVE);
    HookPush(&packet); // pushing pointer to local is fine, a copy of value will be stored, not pointer itself
    HookPush(player);
    int n = SetupStack(binding, key, 2);

    while (n > 0)
    {
        int r = CallOneFunction(n--, 2, 2);

        if (lua_isboolean(L, r + 0) && !lua_toboolean(L, r + 0))
            result = false;

        if (lua_isuserdata(L, r + 1))
            if (WorldPacket* data = CHECKOBJ<WorldPacket>(r + 1, false))
            {
#if defined ELUNA_TRINITY || defined ELUNA_VMANGOS
                packet = std::move(*data);
#else
                packet = *data;
#endif
            }

        lua_pop(L, 2);
    }

    CleanUpStack(2);
}

void Eluna::OnPacketReceiveOne(Player* player, WorldPacket& packet, bool& result)
{
    START_HOOK_PACKET(PACKET_EVENT_ON_PACKET_RECEIVE, packet.GetOpcode());
    HookPush(&packet); // pushing pointer to local is fine, a copy of value will be stored, not pointer itself
    HookPush(player);
    int n = SetupStack(binding, key, 2);

    while (n > 0)
    {
        int r = CallOneFunction(n--, 2, 2);

        if (lua_isboolean(L, r + 0) && !lua_toboolean(L, r + 0))
            result = false;

        if (lua_isuserdata(L, r + 1))
            if (WorldPacket* data = CHECKOBJ<WorldPacket>(r + 1, false))
            {
#if defined ELUNA_TRINITY || defined ELUNA_VMANGOS
                packet = std::move(*data);
#else
                packet = *data;
#endif
            }

        lua_pop(L, 2);
    }

    CleanUpStack(2);
}
