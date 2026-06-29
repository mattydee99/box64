#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define _GNU_SOURCE
#include <dlfcn.h>

#include "wrappedlibs.h"
#include "debug.h"
#include "wrapper.h"
#include "bridge.h"
#include "librarian/library_private.h"
#include "x64emu.h"
#include "emu/x64emu_private.h"
#include "callback.h"
#include "box64context.h"
#include "librarian.h"

const char* eossdkName = "libEOSSDK-LinuxArm64-Shipping.so";
#define LIBNAME eossdk

#include "generated/wrappedeossdktypes.h"
#include "wrappercallback.h"

// ---------------------------------------------------------------------------
// Callback bridge pool
// All EOS async / notification callbacks share void (*)(const void*) signature.
// ---------------------------------------------------------------------------
#define SUPER() \
    GO(0) GO(1) GO(2) GO(3) GO(4) GO(5) GO(6) GO(7) \
    GO(8) GO(9) GO(10) GO(11) GO(12) GO(13) GO(14) GO(15) \
    GO(16) GO(17) GO(18) GO(19) GO(20) GO(21) GO(22) GO(23) \
    GO(24) GO(25) GO(26) GO(27) GO(28) GO(29) GO(30) GO(31) \
    GO(32) GO(33) GO(34) GO(35) GO(36) GO(37) GO(38) GO(39) \
    GO(40) GO(41) GO(42) GO(43) GO(44) GO(45) GO(46) GO(47) \
    GO(48) GO(49) GO(50) GO(51) GO(52) GO(53) GO(54) GO(55) \
    GO(56) GO(57) GO(58) GO(59) GO(60) GO(61) GO(62) GO(63)

#define GO(A) \
static uintptr_t my_eossdk_cb_fct_##A = 0; \
static void my_eossdk_cb_##A(void* data) \
{ RunFunctionFmt(my_eossdk_cb_fct_##A, "p", data); }
SUPER()
#undef GO

static void* find_eossdk_cb_Fct(void* fct)
{
    if (!fct) return fct;
    if (GetNativeFnc((uintptr_t)fct)) return GetNativeFnc((uintptr_t)fct);
    #define GO(A) if (my_eossdk_cb_fct_##A == (uintptr_t)fct) return my_eossdk_cb_##A;
    SUPER()
    #undef GO
    #define GO(A) if (my_eossdk_cb_fct_##A == 0) { my_eossdk_cb_fct_##A = (uintptr_t)fct; return my_eossdk_cb_##A; }
    SUPER()
    #undef GO
    printf_log(LOG_NONE, "Warning, no more slot for EOS SDK callback\n");
    return NULL;
}

#undef SUPER

// Forward declarations
EXPORT int32_t my_EOS_Logging_SetCallback(x64emu_t* emu, void* a0);
EXPORT void my_EOS_Auth_Login(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Auth_Logout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Auth_LinkAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Auth_DeletePersistentAuth(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Auth_QueryIdToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Auth_VerifyIdToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Auth_VerifyUserAuth(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Auth_AddNotifyLoginStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_Login(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_Logout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_CreateUser(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_LinkAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_UnlinkAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_CreateDeviceId(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_DeleteDeviceId(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_TransferDeviceIdAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_QueryExternalAccountMappings(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_QueryProductUserIdMappings(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Connect_VerifyIdToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Connect_AddNotifyAuthExpiration(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Connect_AddNotifyLoginStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_UpdateSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_DestroySession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_JoinSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_StartSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_EndSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_RegisterPlayers(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_UnregisterPlayers(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_SendInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_RejectInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sessions_QueryInvites(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Sessions_AddNotifySessionInviteReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Sessions_AddNotifySessionInviteAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Sessions_AddNotifySessionInviteRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Sessions_AddNotifyJoinSessionAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Sessions_AddNotifyLeaveSessionRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Sessions_AddNotifySendSessionNativeInviteRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_SessionSearch_Find(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_CreateLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_DestroyLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_JoinLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_JoinLobbyById(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_LeaveLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_UpdateLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_PromoteMember(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_KickMember(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_SendInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_RejectInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_QueryInvites(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_HardMuteMember(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_JoinRTCRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Lobby_LeaveRTCRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyUpdateReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyMemberUpdateReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyMemberStatusReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyInviteReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyInviteAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyInviteRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyJoinLobbyAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyLeaveLobbyRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifyRTCRoomConnectionChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Lobby_AddNotifySendLobbyNativeInviteRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_LobbySearch_Find(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_P2P_QueryNATType(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionRequest(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionEstablished(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionInterrupted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionClosed(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_P2P_AddNotifyIncomingPacketQueueFull(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Achievements_QueryDefinitions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Achievements_QueryPlayerAchievements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Achievements_UnlockAchievements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Achievements_AddNotifyAchievementsUnlocked(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Achievements_AddNotifyAchievementsUnlockedV2(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Stats_IngestStat(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Stats_QueryStats(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Leaderboards_QueryLeaderboardDefinitions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Leaderboards_QueryLeaderboardRanks(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Leaderboards_QueryLeaderboardUserScores(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_UserInfo_QueryUserInfo(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_UserInfo_QueryUserInfoByDisplayName(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_UserInfo_QueryUserInfoByExternalAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Friends_QueryFriends(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Friends_SendInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Friends_AcceptInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Friends_RejectInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Friends_AddNotifyFriendsUpdate(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Friends_AddNotifyBlockedUsersUpdate(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Presence_QueryPresence(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Presence_SetPresence(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Presence_AddNotifyOnPresenceChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Presence_AddNotifyJoinGameAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Reports_SendPlayerBehaviorReport(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sanctions_QueryActivePlayerSanctions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Sanctions_CreatePlayerSanctionAppeal(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_AntiCheatServer_BeginSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_AntiCheatServer_EndSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatServer_AddNotifyClientActionRequired(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatServer_AddNotifyClientAuthStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatServer_AddNotifyMessageToClient(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_AntiCheatClient_BeginSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_AntiCheatClient_EndSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyMessageToServer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyMessageToPeer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyPeerActionRequired(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyPeerAuthStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyClientIntegrityViolated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT int32_t my_EOS_UI_ShowFriends(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT int32_t my_EOS_UI_HideFriends(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT int32_t my_EOS_UI_ShowBlockPlayer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT int32_t my_EOS_UI_ShowReportPlayer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT int32_t my_EOS_UI_ShowNativeProfile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_UI_AddNotifyDisplaySettingsUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_UI_AddNotifyMemoryMonitor(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_QueryOwnership(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_QueryOwnershipBySandboxIds(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_QueryOwnershipToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_QueryEntitlements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_QueryEntitlementToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_QueryOffers(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_Checkout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Ecom_RedeemEntitlements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_PlayerDataStorage_QueryFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_PlayerDataStorage_QueryFileList(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_PlayerDataStorage_ReadFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_PlayerDataStorage_WriteFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_PlayerDataStorage_DeleteFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_PlayerDataStorage_DeleteCache(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_PlayerDataStorage_DuplicateFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_TitleStorage_QueryFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_TitleStorage_QueryFileList(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_TitleStorage_ReadFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_TitleStorage_DeleteCache(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Mods_EnumerateMods(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Mods_InstallMod(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Mods_UninstallMod(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Mods_UpdateMod(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_KWS_QueryAgeGate(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_KWS_CreateUser(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_KWS_QueryPermissions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_KWS_RequestPermissions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_KWS_UpdateParentEmail(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_KWS_AddNotifyPermissionsUpdateReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_ProgressionSnapshot_BeginSnapshot(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_ProgressionSnapshot_SubmitSnapshot(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_ProgressionSnapshot_DeleteSnapshot(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_CustomInvites_SendCustomInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_CustomInvites_SendRequestToJoin(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_CustomInvites_AcceptRequestToJoin(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_CustomInvites_RejectRequestToJoin(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_CustomInvites_FinalizeInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyCustomInviteReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyCustomInviteAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyCustomInviteRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinResponseReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_CustomInvites_AddNotifySendCustomNativeInviteRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_IntegratedPlatform_FinalizeDeferredUserLogout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_IntegratedPlatform_AddNotifyUserLoginStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTC_JoinRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTC_LeaveRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTC_BlockParticipant(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTC_AddNotifyDisconnected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTC_AddNotifyParticipantStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTC_AddNotifyRoomStatisticsUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAudio_QueryInputDevicesInformation(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAudio_QueryOutputDevicesInformation(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAudio_UpdateSending(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAudio_UpdateReceiving(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAudio_UpdateSendingVolume(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAudio_UpdateReceivingVolume(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAudio_UpdateParticipantVolume(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioDevicesChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioInputState(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioOutputState(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioBeforeSend(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioBeforeRender(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyParticipantUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCData_UpdateSending(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCData_UpdateReceiving(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCData_AddNotifyDataReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_RTCData_AddNotifyParticipantUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAdmin_QueryJoinRoomToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAdmin_Kick(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_RTCAdmin_SetParticipantHardMute(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Audio_QueryInputDevices(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT void my_EOS_Audio_QueryOutputDevices(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);
EXPORT uint64_t my_EOS_Audio_SetNotifyDevicesChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3);

#include "wrappedlib_init.h"

// ---------------------------------------------------------------------------
// my_* wrapper implementations — bridge x86 callbacks to ARM64 native EOS SDK
// ---------------------------------------------------------------------------
EXPORT int32_t my_EOS_Logging_SetCallback(x64emu_t* emu, void* a0)
{
    (void)emu;
    return (int32_t)my->EOS_Logging_SetCallback(find_eossdk_cb_Fct(a0));
}
EXPORT void my_EOS_Auth_Login(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Auth_Login(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Auth_Logout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Auth_Logout(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Auth_LinkAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Auth_LinkAccount(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Auth_DeletePersistentAuth(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Auth_DeletePersistentAuth(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Auth_QueryIdToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Auth_QueryIdToken(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Auth_VerifyIdToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Auth_VerifyIdToken(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Auth_VerifyUserAuth(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Auth_VerifyUserAuth(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Auth_AddNotifyLoginStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Auth_AddNotifyLoginStatusChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_Login(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_Login(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_Logout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_Logout(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_CreateUser(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_CreateUser(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_LinkAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_LinkAccount(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_UnlinkAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_UnlinkAccount(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_CreateDeviceId(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_CreateDeviceId(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_DeleteDeviceId(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_DeleteDeviceId(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_TransferDeviceIdAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_TransferDeviceIdAccount(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_QueryExternalAccountMappings(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_QueryExternalAccountMappings(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_QueryProductUserIdMappings(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_QueryProductUserIdMappings(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Connect_VerifyIdToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Connect_VerifyIdToken(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Connect_AddNotifyAuthExpiration(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Connect_AddNotifyAuthExpiration(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Connect_AddNotifyLoginStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Connect_AddNotifyLoginStatusChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_UpdateSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_UpdateSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_DestroySession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_DestroySession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_JoinSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_JoinSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_StartSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_StartSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_EndSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_EndSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_RegisterPlayers(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_RegisterPlayers(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_UnregisterPlayers(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_UnregisterPlayers(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_SendInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_SendInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_RejectInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_RejectInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sessions_QueryInvites(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sessions_QueryInvites(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Sessions_AddNotifySessionInviteReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Sessions_AddNotifySessionInviteReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Sessions_AddNotifySessionInviteAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Sessions_AddNotifySessionInviteAccepted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Sessions_AddNotifySessionInviteRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Sessions_AddNotifySessionInviteRejected(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Sessions_AddNotifyJoinSessionAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Sessions_AddNotifyJoinSessionAccepted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Sessions_AddNotifyLeaveSessionRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Sessions_AddNotifyLeaveSessionRequested(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Sessions_AddNotifySendSessionNativeInviteRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Sessions_AddNotifySendSessionNativeInviteRequested(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_SessionSearch_Find(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_SessionSearch_Find(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_CreateLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_CreateLobby(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_DestroyLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_DestroyLobby(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_JoinLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_JoinLobby(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_JoinLobbyById(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_JoinLobbyById(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_LeaveLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_LeaveLobby(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_UpdateLobby(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_UpdateLobby(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_PromoteMember(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_PromoteMember(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_KickMember(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_KickMember(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_SendInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_SendInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_RejectInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_RejectInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_QueryInvites(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_QueryInvites(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_HardMuteMember(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_HardMuteMember(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_JoinRTCRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_JoinRTCRoom(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Lobby_LeaveRTCRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Lobby_LeaveRTCRoom(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyUpdateReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyLobbyUpdateReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyMemberUpdateReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyLobbyMemberUpdateReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyMemberStatusReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyLobbyMemberStatusReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyInviteReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyLobbyInviteReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyInviteAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyLobbyInviteAccepted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyLobbyInviteRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyLobbyInviteRejected(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyJoinLobbyAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyJoinLobbyAccepted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyLeaveLobbyRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyLeaveLobbyRequested(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifyRTCRoomConnectionChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifyRTCRoomConnectionChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Lobby_AddNotifySendLobbyNativeInviteRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Lobby_AddNotifySendLobbyNativeInviteRequested(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_LobbySearch_Find(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_LobbySearch_Find(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_P2P_QueryNATType(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_P2P_QueryNATType(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionRequest(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_P2P_AddNotifyPeerConnectionRequest(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionEstablished(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_P2P_AddNotifyPeerConnectionEstablished(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionInterrupted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_P2P_AddNotifyPeerConnectionInterrupted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_P2P_AddNotifyPeerConnectionClosed(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_P2P_AddNotifyPeerConnectionClosed(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_P2P_AddNotifyIncomingPacketQueueFull(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_P2P_AddNotifyIncomingPacketQueueFull(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Achievements_QueryDefinitions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Achievements_QueryDefinitions(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Achievements_QueryPlayerAchievements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Achievements_QueryPlayerAchievements(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Achievements_UnlockAchievements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Achievements_UnlockAchievements(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Achievements_AddNotifyAchievementsUnlocked(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Achievements_AddNotifyAchievementsUnlocked(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Achievements_AddNotifyAchievementsUnlockedV2(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Achievements_AddNotifyAchievementsUnlockedV2(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Stats_IngestStat(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Stats_IngestStat(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Stats_QueryStats(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Stats_QueryStats(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Leaderboards_QueryLeaderboardDefinitions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Leaderboards_QueryLeaderboardDefinitions(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Leaderboards_QueryLeaderboardRanks(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Leaderboards_QueryLeaderboardRanks(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Leaderboards_QueryLeaderboardUserScores(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Leaderboards_QueryLeaderboardUserScores(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_UserInfo_QueryUserInfo(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_UserInfo_QueryUserInfo(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_UserInfo_QueryUserInfoByDisplayName(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_UserInfo_QueryUserInfoByDisplayName(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_UserInfo_QueryUserInfoByExternalAccount(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_UserInfo_QueryUserInfoByExternalAccount(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Friends_QueryFriends(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Friends_QueryFriends(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Friends_SendInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Friends_SendInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Friends_AcceptInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Friends_AcceptInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Friends_RejectInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Friends_RejectInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Friends_AddNotifyFriendsUpdate(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Friends_AddNotifyFriendsUpdate(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Friends_AddNotifyBlockedUsersUpdate(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Friends_AddNotifyBlockedUsersUpdate(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Presence_QueryPresence(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Presence_QueryPresence(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Presence_SetPresence(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Presence_SetPresence(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Presence_AddNotifyOnPresenceChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Presence_AddNotifyOnPresenceChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Presence_AddNotifyJoinGameAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Presence_AddNotifyJoinGameAccepted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Reports_SendPlayerBehaviorReport(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Reports_SendPlayerBehaviorReport(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sanctions_QueryActivePlayerSanctions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sanctions_QueryActivePlayerSanctions(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Sanctions_CreatePlayerSanctionAppeal(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Sanctions_CreatePlayerSanctionAppeal(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_AntiCheatServer_BeginSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_AntiCheatServer_BeginSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_AntiCheatServer_EndSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_AntiCheatServer_EndSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatServer_AddNotifyClientActionRequired(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatServer_AddNotifyClientActionRequired(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatServer_AddNotifyClientAuthStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatServer_AddNotifyClientAuthStatusChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatServer_AddNotifyMessageToClient(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatServer_AddNotifyMessageToClient(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_AntiCheatClient_BeginSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_AntiCheatClient_BeginSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_AntiCheatClient_EndSession(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_AntiCheatClient_EndSession(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyMessageToServer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatClient_AddNotifyMessageToServer(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyMessageToPeer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatClient_AddNotifyMessageToPeer(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyPeerActionRequired(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatClient_AddNotifyPeerActionRequired(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyPeerAuthStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatClient_AddNotifyPeerAuthStatusChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_AntiCheatClient_AddNotifyClientIntegrityViolated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_AntiCheatClient_AddNotifyClientIntegrityViolated(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT int32_t my_EOS_UI_ShowFriends(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (int32_t)my->EOS_UI_ShowFriends(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT int32_t my_EOS_UI_HideFriends(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (int32_t)my->EOS_UI_HideFriends(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT int32_t my_EOS_UI_ShowBlockPlayer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (int32_t)my->EOS_UI_ShowBlockPlayer(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT int32_t my_EOS_UI_ShowReportPlayer(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (int32_t)my->EOS_UI_ShowReportPlayer(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT int32_t my_EOS_UI_ShowNativeProfile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (int32_t)my->EOS_UI_ShowNativeProfile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_UI_AddNotifyDisplaySettingsUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_UI_AddNotifyDisplaySettingsUpdated(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_UI_AddNotifyMemoryMonitor(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_UI_AddNotifyMemoryMonitor(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_QueryOwnership(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_QueryOwnership(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_QueryOwnershipBySandboxIds(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_QueryOwnershipBySandboxIds(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_QueryOwnershipToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_QueryOwnershipToken(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_QueryEntitlements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_QueryEntitlements(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_QueryEntitlementToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_QueryEntitlementToken(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_QueryOffers(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_QueryOffers(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_Checkout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_Checkout(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Ecom_RedeemEntitlements(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Ecom_RedeemEntitlements(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_PlayerDataStorage_QueryFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_PlayerDataStorage_QueryFile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_PlayerDataStorage_QueryFileList(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_PlayerDataStorage_QueryFileList(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_PlayerDataStorage_ReadFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_PlayerDataStorage_ReadFile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_PlayerDataStorage_WriteFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_PlayerDataStorage_WriteFile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_PlayerDataStorage_DeleteFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_PlayerDataStorage_DeleteFile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_PlayerDataStorage_DeleteCache(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_PlayerDataStorage_DeleteCache(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_PlayerDataStorage_DuplicateFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_PlayerDataStorage_DuplicateFile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_TitleStorage_QueryFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_TitleStorage_QueryFile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_TitleStorage_QueryFileList(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_TitleStorage_QueryFileList(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_TitleStorage_ReadFile(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_TitleStorage_ReadFile(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_TitleStorage_DeleteCache(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_TitleStorage_DeleteCache(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Mods_EnumerateMods(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Mods_EnumerateMods(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Mods_InstallMod(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Mods_InstallMod(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Mods_UninstallMod(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Mods_UninstallMod(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Mods_UpdateMod(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Mods_UpdateMod(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_KWS_QueryAgeGate(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_KWS_QueryAgeGate(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_KWS_CreateUser(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_KWS_CreateUser(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_KWS_QueryPermissions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_KWS_QueryPermissions(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_KWS_RequestPermissions(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_KWS_RequestPermissions(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_KWS_UpdateParentEmail(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_KWS_UpdateParentEmail(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_KWS_AddNotifyPermissionsUpdateReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_KWS_AddNotifyPermissionsUpdateReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_ProgressionSnapshot_BeginSnapshot(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_ProgressionSnapshot_BeginSnapshot(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_ProgressionSnapshot_SubmitSnapshot(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_ProgressionSnapshot_SubmitSnapshot(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_ProgressionSnapshot_DeleteSnapshot(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_ProgressionSnapshot_DeleteSnapshot(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_CustomInvites_SendCustomInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_CustomInvites_SendCustomInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_CustomInvites_SendRequestToJoin(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_CustomInvites_SendRequestToJoin(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_CustomInvites_AcceptRequestToJoin(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_CustomInvites_AcceptRequestToJoin(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_CustomInvites_RejectRequestToJoin(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_CustomInvites_RejectRequestToJoin(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_CustomInvites_FinalizeInvite(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_CustomInvites_FinalizeInvite(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyCustomInviteReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifyCustomInviteReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyCustomInviteAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifyCustomInviteAccepted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyCustomInviteRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifyCustomInviteRejected(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifyRequestToJoinReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinAccepted(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifyRequestToJoinAccepted(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinRejected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifyRequestToJoinRejected(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifyRequestToJoinResponseReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifyRequestToJoinResponseReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_CustomInvites_AddNotifySendCustomNativeInviteRequested(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_CustomInvites_AddNotifySendCustomNativeInviteRequested(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_IntegratedPlatform_FinalizeDeferredUserLogout(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_IntegratedPlatform_FinalizeDeferredUserLogout(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_IntegratedPlatform_AddNotifyUserLoginStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_IntegratedPlatform_AddNotifyUserLoginStatusChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTC_JoinRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTC_JoinRoom(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTC_LeaveRoom(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTC_LeaveRoom(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTC_BlockParticipant(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTC_BlockParticipant(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTC_AddNotifyDisconnected(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTC_AddNotifyDisconnected(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTC_AddNotifyParticipantStatusChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTC_AddNotifyParticipantStatusChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTC_AddNotifyRoomStatisticsUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTC_AddNotifyRoomStatisticsUpdated(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAudio_QueryInputDevicesInformation(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAudio_QueryInputDevicesInformation(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAudio_QueryOutputDevicesInformation(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAudio_QueryOutputDevicesInformation(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAudio_UpdateSending(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAudio_UpdateSending(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAudio_UpdateReceiving(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAudio_UpdateReceiving(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAudio_UpdateSendingVolume(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAudio_UpdateSendingVolume(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAudio_UpdateReceivingVolume(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAudio_UpdateReceivingVolume(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAudio_UpdateParticipantVolume(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAudio_UpdateParticipantVolume(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioDevicesChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCAudio_AddNotifyAudioDevicesChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioInputState(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCAudio_AddNotifyAudioInputState(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioOutputState(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCAudio_AddNotifyAudioOutputState(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioBeforeSend(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCAudio_AddNotifyAudioBeforeSend(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyAudioBeforeRender(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCAudio_AddNotifyAudioBeforeRender(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCAudio_AddNotifyParticipantUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCAudio_AddNotifyParticipantUpdated(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCData_UpdateSending(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCData_UpdateSending(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCData_UpdateReceiving(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCData_UpdateReceiving(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCData_AddNotifyDataReceived(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCData_AddNotifyDataReceived(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_RTCData_AddNotifyParticipantUpdated(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_RTCData_AddNotifyParticipantUpdated(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAdmin_QueryJoinRoomToken(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAdmin_QueryJoinRoomToken(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAdmin_Kick(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAdmin_Kick(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_RTCAdmin_SetParticipantHardMute(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_RTCAdmin_SetParticipantHardMute(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Audio_QueryInputDevices(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Audio_QueryInputDevices(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT void my_EOS_Audio_QueryOutputDevices(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    my->EOS_Audio_QueryOutputDevices(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
EXPORT uint64_t my_EOS_Audio_SetNotifyDevicesChanged(x64emu_t* emu, void* a0, void* a1, void* a2, void* a3)
{
    (void)emu;
    return (uint64_t)my->EOS_Audio_SetNotifyDevicesChanged(a0, a1, a2, find_eossdk_cb_Fct(a3));
}
