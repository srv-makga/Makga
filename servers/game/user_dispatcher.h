#pragma once

#include "../common/dispatcher.h"

class User;
class UserDispatcher : public DispatcherTemplate<User>
{
public:
UserDispatcher(std::shared_ptr<User> owner);
virtual ~UserDispatcher() = default;

bool Execute(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet) override;

private:
// ── 로그인 시퀀스 ─────────────────────────────────────────────────────────
bool Login(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool SelectCharacter(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 이동 ─────────────────────────────────────────────────────────────────
bool Move(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 전투 / 스킬 ──────────────────────────────────────────────────────────
bool Attack(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool SkillUse(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool SkillList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool Resurrect(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 인벤토리 / 장비 ───────────────────────────────────────────────────────
bool InventoryList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool ItemUse(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool ItemDrop(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool ItemMove(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool ItemSplit(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool ItemLock(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool EquipItem(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool UnequipItem(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool EquipList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 상점 ─────────────────────────────────────────────────────────────────
bool ShopOpen(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool ShopBuy(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool ShopSell(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 거래 ─────────────────────────────────────────────────────────────────
bool TradeRequest(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TradeAccept(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TradeAddItem(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TradeAddGold(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TradeReady(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TradeConfirm(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TradeCancel(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 경매장 ───────────────────────────────────────────────────────────────
bool AuctionSearch(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool AuctionRegister(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool AuctionBid(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool AuctionBuyNow(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool AuctionCancel(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool AuctionMyList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 제작 / 강화 ───────────────────────────────────────────────────────────
bool CraftList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool CraftRequest(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool EnchantRequest(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 파티 ─────────────────────────────────────────────────────────────────
bool PartyInvite(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool PartyInviteResp(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool PartyLeave(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool PartyKick(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool PartyDisband(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool PartyLeaderChange(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool PartyLootChange(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 길드 ─────────────────────────────────────────────────────────────────
bool GuildCreate(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildDisband(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildInvite(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildInviteResp(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildLeave(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildKick(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildInfo(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildMemberList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildRankChange(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildNotice(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildDeposit(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool GuildWithdraw(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 던전 ─────────────────────────────────────────────────────────────────
bool DungeonEnter(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool DungeonExit(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 퀘스트 ───────────────────────────────────────────────────────────────
bool QuestList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool QuestAccept(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool QuestAbandon(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool QuestComplete(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 친구 ─────────────────────────────────────────────────────────────────
bool FriendList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool FriendAdd(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool FriendAddResp(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool FriendRemove(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool FriendBlock(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool Whisper(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 채팅 ─────────────────────────────────────────────────────────────────
bool Chat(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 업적 / 칭호 ───────────────────────────────────────────────────────────
bool AchievementList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TitleList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TitleEquip(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool TitleUnequip(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 랭킹 ─────────────────────────────────────────────────────────────────
bool RankingList(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 시즌 ─────────────────────────────────────────────────────────────────
bool SeasonInfo(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool SeasonReward(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
bool SeasonMission(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);

// ── 히든 ─────────────────────────────────────────────────────────────────
bool LegendaryForge(std::shared_ptr<User> owner, std::shared_ptr<NetPacket> packet);
};