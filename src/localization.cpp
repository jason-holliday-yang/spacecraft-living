#include "localization.h"

#include <array>
#include <cstring>
#include <string>

namespace {

struct NamedTranslation {
    const char *english;
    const char *simplifiedChinese;
};

GameLanguage gCurrentLanguage = GAME_LANGUAGE_EN;

const std::array<LocalizedText, LOC_TEXT_COUNT> kTextTable = {{
    {"Health", "生命"},
    {"Oxygen", "氧气"},
    {"Stage", "阶段"},
    {"Objective", "目标"},
    {"Cycle", "周期"},
    {"Event", "事件"},
    {"Start Game", "开始游戏"},
    {"Load", "读取"},
    {"Settings", "设置"},
    {"Exit", "退出"},
    {"Paused", "已暂停"},
    {"Resume", "继续"},
    {"Save", "保存"},
    {"Load", "读取"},
    {"Settings", "设置"},
    {"Main Menu", "主菜单"},
    {"Portable Communicator", "便携终端"},
    {"Crafting", "制作"},
    {"Controls", "操作说明"},
    {"Press the matching key to close", "按对应按键关闭"},
    {"Settings", "设置"},
    {"Locked", "未解锁"},
    {"Progress saved", "进度已保存"},
    {"Save failed", "保存失败"},
    {"No save file found", "未找到存档"},
    {"Save deleted", "存档已删除"},
    {"Delete failed", "删除失败"},
    {"Press ESC to close", "按 ESC 关闭"},
    {"Close", "关闭"},
    {"Press ESC to close.", "按 ESC 关闭。"},
    {"Press ESC to return", "按 ESC 返回"},
    {"Language", "语言"}
}};

const std::array<NamedTranslation, 7> kAreaNameTranslations = {{
    {"Ship Base", "飞船基地"},
    {"Crash Forest", "坠毁森林"},
    {"Spore Swamp", "孢子沼泽"},
    {"Ruins", "遗迹"},
    {"Boss Arena", "遗迹"},
    {"Unknown Area", "未知区域"},
    {"Echo Wilds", "回响荒野"}
}};

const std::array<NamedTranslation, 27> kLocationNameTranslations = {{
    {"Central Corridor", "中央走廊"},
    {"Cargo Hold", "货舱"},
    {"Crew Quarters", "船员舱"},
    {"Diagnostics", "诊断舱"},
    {"Terminal Bay", "终端舱"},
    {"Life Support", "生命维持舱"},
    {"Workshop", "工坊"},
    {"Power Bay", "动力舱"},
    {"Airlock Link", "气闸通道"},
    {"Northwest Ruins", "西北遗迹"},
    {"Signal Tower Plateau", "信号塔高台"},
    {"Monolith Ring", "石碑环区"},
    {"Ruins Approach", "遗迹前沿"},
    {"Deep Gate", "深层入口"},
    {"Deep Basin", "深潭区"},
    {"Outer Swamp Rim", "外沼边缘"},
    {"Flooded Detour", "积水绕路"},
    {"West Frontier", "西部前线"},
    {"Survey Break", "勘测断点"},
    {"Canopy Hollow", "林冠洼地"},
    {"Echo Basin", "回声盆地"},
    {"Last Camp", "最后营地"},
    {"South Collapse", "南部塌陷区"},
    {"Vent Galleries", "通风廊道"},
    {"Service Shafts", "维护井道"},
    {"Purifier Ring", "净化环区"},
    {"Root Vault", "根脉核心"}
}};

static const NamedTranslation kLiteralTranslations[] = {
    {"Username must be at least 3 characters.", "用户名至少需要 3 个字符。"},
    {"Username is too long.", "用户名过长。"},
    {"Username can only use letters, numbers, '_' or '-'.", "用户名只能使用字母、数字、下划线或连字符。"},
    {"Password must be at least 4 characters.", "密码至少需要 4 个字符。"},
    {"Password is too long.", "密码过长。"},
    {"Password must use visible ASCII characters.", "密码只能使用可见的 ASCII 字符。"},
    {"Couldn't read local account data.", "无法读取本地账号数据。"},
    {"That account could not be found.", "找不到该账号。"},
    {"Couldn't remove the account from the local registry.", "无法从本地注册表中移除该账号。"},
    {"Couldn't delete this account's save data.", "无法删除该账号的存档数据。"},
    {"Account not found. Create it first.", "找不到该账号，请先创建。"},
    {"Incorrect password.", "密码错误。"},
    {"Couldn't open this account's save folder.", "无法打开该账号的存档目录。"},
    {"That username already exists. Try signing in.", "该用户名已存在，请尝试直接登录。"},
    {"Couldn't create the local account.", "无法创建本地账号。"},
    {"The account was created, but its save folder could not be opened.", "账号已创建，但无法打开它的存档目录。"},
    {"There is no signed-in account to delete.", "当前没有已登录的账号可供删除。"},
    {"Follow Loxi and keep exploring.", "跟着洛希继续探索。"},
    {"Recover vitals and move to the next objective.", "先恢复状态，再前往下一个目标。"},
    {"Recover vitals and follow current objective.", "先恢复状态，并继续当前目标。"},
    {"Return to Loxi and choose the final route.", "返回洛希处，选择最终路线。"},
    {"Return to Loxi and review the assembled archive before choosing the final route.", "返回洛希处，先复核整理好的档案，再选择最终路线。"},
    {"Recover the Canopy Handoff record, then return to Loxi to confirm heroic rescue.", "先补回林冠交接记录，再回洛希处确认强行救援。"},
    {"Return to Loxi and choose among the unlocked endings.", "返回洛希处，在已解锁的结局中作出选择。"},
    {"Sync fragments at Loxi terminal.", "在洛希终端同步碎片。"},
    {"Sync the fragments at Loxi and review how the archive now reframes rescue, stabilization, and settlement.", "带着碎片去洛希处同步，并查看档案如何重新解释救援、稳定与定居三种终局。"},
    {"Sync the fragments at Loxi and let it align the west crew trail with the south facility record.", "带着碎片去洛希处同步，让它把西线船员轨迹与南方设施记录对齐。"},
    {"Recover 3 Relic Fragments and bring them to Loxi so it can weigh force, stabilization, and settlement against the full system record.", "收集 3 枚遗迹碎片并带回洛希，让它对照完整系统记录，比较强行、稳定与定居三种路线。"},
    {"Recover 3 Relic Fragments and bring them to Loxi so it can align the west and south evidence into one timeline.", "收集 3 枚遗迹碎片并带回洛希，让它把西线与南线证据拼成同一条时间线。"},
    {"Heroic route chosen. Reach the Signal Tower and launch the rescue beacon.", "已选择强行救援路线。前往信号塔并发射求救信标。"},
    {"Heroic route chosen. Defeat the guardian in the northwest ruins, then return to the Signal Tower.", "已选择强行救援路线。先在西北遗迹击败守卫，再返回信号塔。"},
    {"Heroic route chosen. Push into the northwest ruins and commit to the guardian fight.", "已选择强行救援路线。推进西北遗迹，正式投入守卫战。"},
    {"Heroic route chosen. The ring is complete. Defeat the guardian, then reach the Signal Tower.", "已选择强行救援路线。石碑环已完成，先击败守卫，再前往信号塔。"},
    {"Heroic route chosen. Light remaining monoliths or defeat the guardian, then reach the Signal Tower.", "已选择强行救援路线。点亮剩余石碑或击败守卫后，再前往信号塔。"},
    {"Heroic route chosen. Light monoliths or defeat the guardian, then reach the Signal Tower.", "已选择强行救援路线。点亮石碑或击败守卫后，再前往信号塔。"},
    {"Peaceful route chosen. Carry the Signal Amplifier to the Signal Tower.", "已选择和平救援路线。把信号放大器带到信号塔。"},
    {"Peaceful route chosen. Craft the Signal Amplifier at the workshop, then carry it to the Signal Tower.", "已选择和平救援路线。先在工坊制作信号放大器，再把它带到信号塔。"},
    {"Peaceful route chosen. Recover 3 Relic Fragments, craft the Signal Amplifier at the workshop, then carry it to the Signal Tower.", "已选择和平救援路线。先找回 3 枚遗迹碎片，再在工坊制作信号放大器并把它带到信号塔。"},
    {"Peaceful route chosen. Craft the Signal Amplifier, then carry it to the Signal Tower.", "已选择和平救援路线。先制作信号放大器，再把它带到信号塔。"},
    {"Recover the remaining mainline logs and finish west/south archive tasks before choosing an ending with Loxi.", "在向洛希选择结局前，请先找回剩余主线日志，并完成西线 / 南线档案任务。"},
    {"Full archive context is assembled. Return to Loxi so it can review how the west, south, and mainline evidence reshape each ending before you choose.", "完整档案背景已经拼合完成。回到洛希处，让它先带你复核西线、南线与主线证据如何改变每一种结局的含义，然后再作选择。"},
    {"Full archive context is assembled, but no ending line is fully locked yet. Return to Loxi after recovering at least one route-ending archive record.", "完整档案脉络已经拼齐，但还没有任何一条结局线被彻底锁定。至少补回一份路线结局档案后，再回洛希处。"},
    {"Full archive context is assembled. Multiple endings are unlocked now, and Loxi can let you choose among them immediately at the terminal.", "完整档案脉络已经拼齐。现在已有多个结局被解锁，洛希可以立刻在终端让你作出选择。"},
    {"The archive is assembled, but Loxi still needs a final review at the ship before any ending route becomes a deliberate commitment.", "档案已经齐备，但在任何结局路线变成一次明确承诺之前，你仍需要回到飞船，让洛希完成最后一次复核。"},
    {"All mainline logs recovered. Return to Loxi and review the assembled truth before choosing how to answer it.", "所有主线日志都已回收。回到洛希处，先复核整理好的真相，再决定你要如何回应它。"},
    {"West Frontier is still sealed. Restore the comm relay first.", "西部前线仍被封锁。先修复通讯中继。"},
    {"Echo Basin is archived. Move on to the Last Camp investigation.", "回声盆地已归档。继续前往最后营地调查。"},
    {"Canopy Hollow is archived. Move on to the Echo Basin investigation.", "林冠洼地已归档。继续前往回声盆地调查。"},
    {"Survey Break is archived. Move on to the Canopy Hollow investigation.", "勘测断点已归档。继续前往林冠洼地调查。"},
    {"Finish the West Frontier investigation, then return to base.", "完成西部前线调查后返回基地。"},
    {"Finish the Survey Break investigation, then return to base.", "完成勘测断点调查后返回基地。"},
    {"Finish the Canopy Hollow investigation, then return to base.", "完成林冠洼地调查后返回基地。"},
    {"Finish the Echo Basin investigation, then return to base.", "完成回声盆地调查后返回基地。"},
    {"Finish the Last Camp investigation, then return to base.", "完成最后营地调查后返回基地。"},
    {"The west trail is open. Complete the first West Frontier investigation and report back to base.", "西线路线已开放。先完成西部前线的首个调查，再回基地汇报。"},
    {"Survey Break is still sealed. Restore the comm relay first.", "勘测断点仍被封锁。先修复通讯中继。"},
    {"Finish the Survey Break objectives, then return to base.", "完成勘测断点目标后返回基地。"},
    {"Canopy Hollow is still sealed. Restore the comm relay first.", "林冠洼地仍被封锁。先修复通讯中继。"},
    {"Finish the Canopy Hollow investigation here, then return to base.", "在这里完成林冠洼地调查后返回基地。"},
    {"Echo Basin is still sealed. Restore the comm relay first.", "回声盆地仍被封锁。先修复通讯中继。"},
    {"Last Camp is still sealed. Restore the comm relay first.", "最后营地仍被封锁。先修复通讯中继。"},
    {"The Canopy Hollow investigation is active. Finish it here, then return.", "林冠洼地调查已开启。在这里完成后返回。"},
    {"Complete the West Frontier investigation first.", "先完成西部前线调查。"},
    {"Complete the Survey Break investigation first.", "先完成勘测断点调查。"},
    {"Finish the Echo Basin investigation here, then return to base.", "在这里完成回声盆地调查后返回基地。"},
    {"The Echo Basin investigation is active here. Finish it, then return.", "回声盆地调查正在此处进行。完成后返回。"},
    {"Complete the Canopy Hollow investigation first.", "先完成林冠洼地调查。"},
    {"Finish the Last Camp investigation here, then return to base.", "在这里完成最后营地调查后返回基地。"},
    {"Complete the Echo Basin investigation first.", "先完成回声盆地调查。"},
    {"South Collapse is still sealed. Restore the Power Bay first.", "南部塌陷区仍被封锁。先修复动力舱。"},
    {"Finish the Root Vault investigation, then return to base.", "完成根脉核心调查后返回基地。"},
    {"Finish the Purifier Ring investigation, then return to base.", "完成净化环区调查后返回基地。"},
    {"Finish the Service Shaft investigation, then return to base.", "完成维护井道调查后返回基地。"},
    {"Finish the Vent Galleries investigation, then return to base.", "完成通风廊道调查后返回基地。"},
    {"Finish the South Collapse investigation, then return to base.", "完成南部塌陷区调查后返回基地。"},
    {"The south trail is open. Complete the first South Collapse investigation and report back to base.", "南线路线已开放。先完成南部塌陷区的首个调查，再回基地汇报。"},
    {"Vent Galleries are still sealed. Restore the Power Bay first.", "通风廊道仍被封锁。先修复动力舱。"},
    {"Service Shafts are still sealed. Restore the Power Bay first.", "维护井道仍被封锁。先修复动力舱。"},
    {"Finish the Service Shafts investigation here, then return to base.", "在这里完成维护井道调查后返回基地。"},
    {"Purifier Ring is still sealed. Restore the Power Bay first.", "净化环区仍被封锁。先修复动力舱。"},
    {"Root Vault is still sealed. Restore the Power Bay first.", "根脉核心仍被封锁。先修复动力舱。"},
    {"The Service Shafts investigation is active. Finish it here, then return.", "维护井道调查已开启。在这里完成后返回。"},
    {"Finish the Vent Galleries investigation here, then return to base.", "在这里完成通风廊道调查后返回基地。"},
    {"Complete the South Collapse investigation first.", "先完成南部塌陷区调查。"},
    {"The Root Vault investigation is active. Finish it, then return.", "根脉核心调查已开启。完成后返回。"},
    {"Complete the Vent Galleries investigation first.", "先完成通风廊道调查。"},
    {"Finish the Purifier Ring investigation here, then return to base.", "在这里完成净化环区调查后返回基地。"},
    {"The Purifier Ring investigation is active here. Finish it, then return.", "净化环区调查正在此处进行。完成后返回。"},
    {"Complete the Service Shaft investigation first.", "先完成维护井道调查。"},
    {"Finish the Root Vault investigation here, then return to base.", "在这里完成根脉核心调查后返回基地。"},
    {"Complete the Purifier Ring investigation first.", "先完成净化环区调查。"},
    {"The west archive trail is complete.", "西线档案线索已全部完成。"},
    {"The south archive trail is complete.", "南线档案线索已全部完成。"},
    {"The west trail is fully archived.", "西线路线已全部归档。"},
    {"The south trail is fully archived.", "南线路线已全部归档。"},
    {"The west trail is archived through Echo Basin.", "西线路线已归档至回声盆地。"},
    {"The south trail is archived through the Service Shafts.", "南线路线已归档至维护井道。"},
    {"Survey Break is archived. Continue to Canopy Hollow.", "勘测断点已归档。继续前往林冠洼地。"},
    {"The Survey Break investigation is active here. Finish it, then return.", "勘测断点调查正在此处进行。完成后返回。"},
    {"The Last Camp investigation is active. Finish it, then return.", "最后营地调查已开启。完成后返回。"},
    {"The Last Camp investigation is active. Finish it and return.", "最后营地调查已开启。完成后返回。"},
    {"Echo Basin is archived. Continue to Last Camp.", "回声盆地已归档。继续前往最后营地。"},
    {"The Echo Basin investigation is active. Finish it, then return.", "回声盆地调查已开启。完成后返回。"},
    {"The Echo Basin investigation is active. Finish it and return.", "回声盆地调查已开启。完成后返回。"},
    {"Canopy Hollow is archived. Continue to Echo Basin.", "林冠洼地已归档。继续前往回声盆地。"},
    {"The Canopy Hollow investigation is active here. Finish it, then return.", "林冠洼地调查正在此处进行。完成后返回。"},
    {"The Canopy Hollow investigation is active. Finish it and return.", "林冠洼地调查已开启。完成后返回。"},
    {"Survey Break investigation in progress.", "勘测断点调查进行中。"},
    {"Last Camp is archived.", "最后营地已归档。"},
    {"The Last Camp investigation is active here. Finish it, then return.", "最后营地调查正在此处进行。完成后返回。"},
    {"Purifier Ring is archived. Move on to the Root Vault investigation.", "净化环区已归档。继续前往根脉核心调查。"},
    {"Service Shafts are archived. Move on to the Purifier Ring investigation.", "维护井道已归档。继续前往净化环区调查。"},
    {"Vent Galleries are archived. Move on to the Service Shaft investigation.", "通风廊道已归档。继续前往维护井道调查。"},
    {"South Collapse is archived. Move on to Vent Galleries.", "南部塌陷区已归档。继续前往通风廊道。"},
    {"Vent Galleries are archived. Continue to the Service Shafts.", "通风廊道已归档。继续前往维护井道。"},
    {"The Vent Galleries investigation is active here. Finish it, then return.", "通风廊道调查正在此处进行。完成后返回。"},
    {"Purifier Ring is archived. Continue to Root Vault.", "净化环区已归档。继续前往根脉核心。"},
    {"The Purifier Ring investigation is active. Finish it, then return.", "净化环区调查已开启。完成后返回。"},
    {"The Purifier Ring investigation is active. Finish it and return.", "净化环区调查已开启。完成后返回。"},
    {"Service Shafts are archived. Continue to Purifier Ring.", "维护井道已归档。继续前往净化环区。"},
    {"The Service Shafts investigation is active here. Finish it, then return.", "维护井道调查正在此处进行。完成后返回。"},
    {"Root Vault is archived.", "根脉核心已归档。"},
    {"The Root Vault investigation is active here. Finish it, then return.", "根脉核心调查正在此处进行。完成后返回。"},
    {"The Root Vault investigation is active. Finish it and return.", "根脉核心调查已开启。完成后返回。"},
    {"Canopy Hollow is archived. Proceed to Echo Basin.", "林冠洼地已归档。继续前往回声盆地。"},
    {"Canopy Hollow investigation in progress.", "林冠洼地调查进行中。"},
    {"Echo Basin is archived. Proceed to Last Camp.", "回声盆地已归档。继续前往最后营地。"},
    {"Echo Basin investigation in progress.", "回声盆地调查进行中。"},
    {"Last Camp investigation in progress.", "最后营地调查进行中。"},
    {"Vent Galleries investigation in progress.", "通风廊道调查进行中。"},
    {"Service Shafts are archived. Proceed to Purifier Ring.", "维护井道已归档。继续前往净化环区。"},
    {"Purifier Ring is archived. Proceed to Root Vault.", "净化环区已归档。继续前往根脉核心。"},
    {"Purifier Ring investigation in progress.", "净化环区调查进行中。"},
    {"Root Vault investigation in progress.", "根脉核心调查进行中。"},
    {"Gather Wood and Scrap, then restore the oxygen console.", "收集木材和金属残片，然后修理氧气控制台。"},
    {"Gather Glow Moss and Ore, craft a Glow Stick, and finish the repair.", "收集发光苔和矿石，制作荧光棒，完成修理。"},
    {"West Frontier is archived. Move on to Survey Break.", "西部前线已归档。继续前往勘测断点。"},
    {"Use the east airlock and begin the comm relay repair.", "使用东侧气闸，开始修复通讯中继。"},
    {"Gather Vine, Shell Fruit, and Fungus, then head back.", "收集藤蔓、果实与菌株后返回。"},
    {"Stay on the relay route and avoid deeper detours for now.", "先沿着中继路线推进，暂时不要深入绕路。"},
    {"Craft the Laser Gun and Protection Suit at the workbench.", "在工作台制作激光枪和防护服。"},
    {"Inspect the crash clue once both upgrades are equipped.", "装备好两项升级后，再去调查坠毁线索。"},
    {"Install Energy Core in power bay.", "将能源核心安装到动力舱。"},
    {"Prepare the suit, camp, and oxygen support, then head east for the Energy Core.", "准备好防护服、营地与补氧手段，然后向东寻找能源核心。"},
    {"Secure the Energy Core in deep swamp and return safely.", "在深层沼泽取得能源核心并安全返回。"},
    {"Use camp support and oxygen supplies to manage the eastern runs.", "在东线路线中善用营地支援与补氧物品。"},
    {"The fragments are ready. Loxi can now rewrite the ending picture around one failing maintenance lattice.", "碎片已齐。洛希现在可以围绕一套失效中的维护格网重构终局图景。"},
    {"The fragments are ready. Loxi can now align the west crew trail with the south facility record.", "碎片已齐。洛希现在可以把西线船员轨迹与南方设施记录对齐。"},
    {"Late-route analysis is unlocked. Recover 3 fragments so Loxi can finish its rewritten ending picture.", "终局分析已经解锁。请收集 3 枚碎片，让洛希完成它重写后的终局图景。"},
    {"Shared evidence is unlocked. Recover 3 fragments so Loxi can join the west and south record.", "共通证据已解锁。请收集 3 枚碎片，让洛希把西线与南线记录拼在一起。"},
    {"Collect 3 Relic Fragments in north ruins.", "在北方遗迹中收集 3 枚遗迹碎片。"},
    {"The ruins now support Loxi's rewritten ending picture. Finish the fragment set and return.", "遗迹中的准备如今已经能支撑洛希重写后的终局图景。收齐碎片后返回。"},
    {"Recover the fragments in the ruins, then return to base.", "在遗迹中收集碎片后返回基地。"},
    {"Recover the fragments, then sync with Loxi.", "先收集碎片，再与洛希同步。"},
    {"Full archive context is assembled. Loxi can now frame heroic rescue, peaceful rescue, or settlement as deliberate answers to the same truth.", "完整档案背景已经拼合完成。洛希现在能把强行救援、和平救援与异星定居都解释为对同一真相的不同回应。"},
    {"After Loxi's final review, the full archive frames this as a deliberate high-cost rescue.", "在洛希完成最终复核后，完整档案将这一选择定格为一次代价高昂但明确的救援。"},
    {"After Loxi's final review, the full archive frames this as full-system stabilization through understanding.", "在洛希完成最终复核后，完整档案将这一选择定格为一次建立在理解之上的整套系统稳定化。"},
    {"After Loxi's final review, the full archive frames settlement as deliberate long-term stewardship.", "在洛希完成最终复核后，完整档案将定居解释为一次有意识的长期守护。"},
    {"Even after Loxi's final review, the final attempt still collapsed under sustained pressure.", "即使已经经过洛希的最终复核，最后一次尝试仍在持续压力下崩塌。"},
    {"By the time Loxi confirmed it, the late findings had shown the tower, purifier ring, and monoliths were one damaged lattice.", "等到洛希替你确认这条路线时，后期发现已经说明塔楼、净化环与石碑原本就是同一张受损格网。"},
    {"By the time Loxi confirmed it, the late findings had shown the tower and purifier controls were one failing system.", "等到洛希替你确认这条路线时，后期发现已经说明塔楼与净化控制原本就是同一套失效中的系统。"},
    {"By the time Loxi confirmed it, settlement meant inheriting the maintenance lattice instead of escaping it.", "等到洛希替你确认这条路线时，定居就已不再意味着逃离，而是继承这套维护格网。"},
    {"Loxi could already see the late findings, but the damaged lattice still outlasted the run.", "洛希其实已经看清后期发现指向的真相，但那张受损格网仍旧拖垮了这次尝试。"},
    {"By the time Loxi confirmed it, west crew handoffs and south facility records had aligned into one survival timeline.", "等到洛希替你确认这条路线时，西线船员交接与南方设施记录早已对齐成同一条生存时间线。"},
    {"By the time Loxi confirmed it, the archive had already shown that coordination mattered more than panic.", "等到洛希替你确认这条路线时，档案早已说明协同远比恐慌更重要。"},
    {"By the time Loxi confirmed it, the archive had already shown that staying was deliberate, not surrender.", "等到洛希替你确认这条路线时，档案早已说明留下是一种主动选择，而不是投降。"},
    {"By the time Loxi framed the choice, the shared trail was visible, but the run still collapsed before it held.", "等到洛希把抉择摆到你面前时，那条共通线索已经清晰可见，但这次行动仍在真正稳住之前崩塌了。"},
    {"Heroic route chosen. Head to the Signal Tower.", "已选择强行救援路线。前往信号塔。"},
    {"Heroic route chosen. Finish monolith prep if needed, then defeat the guardian.", "已选择强行救援路线。如有需要先完成石碑准备，再击败守卫。"},
    {"Heroic route chosen. The ring is fully lit. Push into the northwest ruins when you are ready for the guardian fight.", "已选择强行救援路线。石碑环已完全点亮。准备好后推进西北遗迹，开始守卫战。"},
    {"Heroic route chosen. Your monolith prep carries into the northwest ruins. Push in when you are ready.", "已选择强行救援路线。你的石碑准备会直接带进西北遗迹。准备好后就推进。"},
    {"Heroic route chosen. Push into the northwest ruins to begin the guardian fight.", "已选择强行救援路线。推进西北遗迹，开始守卫战。"},
    {"Peaceful route chosen. Take the Signal Amplifier to the tower.", "已选择和平救援路线。把信号放大器带到塔楼。"},
    {"Peaceful route chosen. Craft the Signal Amplifier at the workshop before the tower run.", "已选择和平救援路线。前往塔楼前先在工坊制作信号放大器。"},
    {"Peaceful route chosen. Recover 3 Relic Fragments, then craft the Signal Amplifier at the workshop before the tower run.", "已选择和平救援路线。先找回 3 枚遗迹碎片，再在工坊制作信号放大器后前往塔楼。"},
    {"Peaceful route chosen. Craft the Signal Amplifier before the tower run.", "已选择和平救援路线。前往塔楼前先制作信号放大器。"},
    {"West crew testimony and south system truth now agree on what the final choice means. Finish the main archive and return here.", "西线船员证词与南方系统真相如今已经一致说明最终抉择的意义。补完主线档案后再回来。"},
    {"Loxi has rewritten the late-game comparison around one failing maintenance lattice. Finish the main archive before the branch opens.", "洛希已经围绕一套失效中的维护格网重写了终局比较。在分支开启前，请先补完主线档案。"},
    {"Crew traces and facility handovers now align into one timeline. Keep recovering the main archive.", "船员痕迹与设施交接如今已对齐成同一条时间线。继续回收主线档案吧。"},
    {"Final stage is now an evidence sweep: recover every remaining mainline log, then choose your ending here.", "最终阶段现在是一场证据搜集：找回所有剩余主线日志，然后在这里选择你的结局。"},
    {"The tower is ready for the chosen heroic ending.", "信号塔已经为你选择的强行救援结局做好准备。"},
    {"Heroic route selected. The guardian is weakened.", "已选择强行救援路线。守卫已经被削弱。"},
    {"Heroic route selected. Finish monolith prep or hunt the guardian.", "已选择强行救援路线。继续完成石碑准备，或直接去猎杀守卫。"},
    {"Heroic route selected. The guardian is pinned in the northwest ruins, and the fully lit ring is weakening it.", "已选择强行救援路线。守卫已被压制在西北遗迹，而完整石碑环正在削弱它。"},
    {"Heroic route selected. The guardian is in the northwest ruins. Finish the fight, then return to the tower.", "已选择强行救援路线。守卫就在西北遗迹。结束这场战斗后，再回到塔楼。"},
    {"Heroic route selected. The guardian is in the northwest ruins, and the ring is already weakening it. Push in when ready.", "已选择强行救援路线。守卫就在西北遗迹，而石碑环也已经在削弱它。准备好后就推进。"},
    {"Heroic route selected. Push into the northwest ruins to enter the guardian fight.", "已选择强行救援路线。推进西北遗迹，进入守卫战。"},
    {"Peaceful route selected. Bring the Signal Amplifier to the tower.", "已选择和平救援路线。把信号放大器带到塔楼。"},
    {"Peaceful route selected. Craft the Signal Amplifier at the workshop first.", "已选择和平救援路线。先在工坊制作信号放大器。"},
    {"Peaceful route selected. Recover 3 Relic Fragments, then craft the Signal Amplifier at the workshop.", "已选择和平救援路线。先找回 3 枚遗迹碎片，再在工坊制作信号放大器。"},
    {"Peaceful route selected. Craft the Signal Amplifier first.", "已选择和平救援路线。先制作信号放大器。"},
    {"The main archive is complete. Return to Loxi to choose the ending route before pushing farther.", "主线档案已经完整。继续深入之前，请回到洛希处选择结局路线。"},
    {"Recover the remaining mainline logs and finish archive tasks before the final branch opens.", "在最终分支开启前，请先找回剩余主线日志并完成档案任务。"},
    {"Watch oxygen, health, and retreat route.", "留意氧气、生命值与撤离路线。"},
    {"Low oxygen. Retreat or recover now.", "氧气过低。立刻撤退或恢复。"},
    {"High poison. Use a Recovery Ration or retreat.", "毒素过高。使用复苏口粮，或立刻撤退。"},
    {"Oxygen leak active. Repair suit soon.", "漏氧状态正在生效。尽快修补防护服。"},
    {"Vitals unstable for advanced crafting.", "当前生命体征不稳定，无法进行高级制作。"},
    {"The west trail stays sealed until the comm relay is restored.", "在通讯中继修好之前，西线路线会一直封锁。"},
    {"Last Camp investigation in progress. Finish it and return.", "最后营地调查进行中。完成后返回。"},
    {"Echo Basin is archived. Move on to Last Camp.", "回声盆地已归档。继续前往最后营地。"},
    {"Echo Basin investigation in progress. Finish it and return.", "回声盆地调查进行中。完成后返回。"},
    {"Canopy Hollow is archived. Move on to Echo Basin.", "林冠洼地已归档。继续前往回声盆地。"},
    {"Canopy Hollow investigation in progress. Finish it and return.", "林冠洼地调查进行中。完成后返回。"},
    {"Survey Break is archived. Move on to Canopy Hollow.", "勘测断点已归档。继续前往林冠洼地。"},
    {"Survey Break investigation in progress. Finish it and return.", "勘测断点调查进行中。完成后返回。"},
    {"West Frontier is archived. Start Survey Break.", "西部前线已归档。开始勘测断点调查。"},
    {"West Frontier investigation in progress. Complete it and return.", "西部前线调查进行中。完成后返回。"},
    {"Survey Break stays sealed until the comm relay is restored.", "在通讯中继修好之前，勘测断点会一直封锁。"},
    {"Survey Break is archived. Proceed to Canopy Hollow.", "勘测断点已归档。继续前往林冠洼地。"},
    {"The Survey Break investigation is active here now that West Frontier is archived.", "西部前线归档后，勘测断点调查已在此处开启。"},
    {"Canopy Hollow stays sealed until the comm relay is restored.", "在通讯中继修好之前，林冠洼地会一直封锁。"},
    {"The Canopy Hollow investigation is active here now that Survey Break is archived.", "勘测断点归档后，林冠洼地调查已在此处开启。"},
    {"Echo Basin stays sealed until the comm relay is restored.", "在通讯中继修好之前，回声盆地会一直封锁。"},
    {"The Echo Basin investigation is active here now that Canopy Hollow is archived.", "林冠洼地归档后，回声盆地调查已在此处开启。"},
    {"Last Camp stays sealed until the comm relay is restored.", "在通讯中继修好之前，最后营地会一直封锁。"},
    {"The Last Camp investigation is active here now that Echo Basin is archived.", "回声盆地归档后，最后营地调查已在此处开启。"},
    {"The south trail stays sealed until the Power Bay is restored.", "在动力舱修好之前，南线路线会一直封锁。"},
    {"Root Vault investigation in progress. Finish it and return.", "根脉核心调查进行中。完成后返回。"},
    {"Purifier Ring is archived. Move on to Root Vault.", "净化环区已归档。继续前往根脉核心。"},
    {"Purifier Ring investigation in progress. Finish it and return.", "净化环区调查进行中。完成后返回。"},
    {"Service Shafts are archived. Move on to Purifier Ring.", "维护井道已归档。继续前往净化环区。"},
    {"Service Shafts investigation in progress. Finish it and return.", "维护井道调查进行中。完成后返回。"},
    {"Vent Galleries are archived. Move on to Service Shafts.", "通风廊道已归档。继续前往维护井道。"},
    {"Vent Galleries investigation in progress. Finish it and return.", "通风廊道调查进行中。完成后返回。"},
    {"South Collapse is archived. Start Vent Galleries.", "南部塌陷区已归档。开始通风廊道调查。"},
    {"South Collapse investigation in progress. Complete it and return.", "南部塌陷区调查进行中。完成后返回。"},
    {"Vent Galleries stay sealed until the Power Bay is restored.", "在动力舱修好之前，通风廊道会一直封锁。"},
    {"The Service Shafts investigation is active. Finish it and return.", "维护井道调查已开启。完成后返回。"},
    {"Vent Galleries are archived. Proceed to Service Shafts.", "通风廊道已归档。继续前往维护井道。"},
    {"The Vent Galleries investigation is active here now that South Collapse is archived.", "南部塌陷区归档后，通风廊道调查已在此处开启。"},
    {"Service Shafts stay sealed until the Power Bay is restored.", "在动力舱修好之前，维护井道会一直封锁。"},
    {"Service Shafts investigation in progress.", "维护井道调查进行中。"},
    {"The Service Shafts investigation is active here now that Vent Galleries are archived.", "通风廊道归档后，维护井道调查已在此处开启。"},
    {"Purifier Ring stays sealed until the Power Bay is restored.", "在动力舱修好之前，净化环区会一直封锁。"},
    {"The Purifier Ring investigation is active here now that Service Shafts are archived.", "维护井道归档后，净化环区调查已在此处开启。"},
    {"Root Vault stays sealed until the Power Bay is restored.", "在动力舱修好之前，根脉核心会一直封锁。"},
    {"The Root Vault investigation is active here now that Purifier Ring is archived.", "净化环区归档后，根脉核心调查已在此处开启。"},
    {"Base is your relay hub. Prepare here, then return safely.", "基地是你往返中继任务的据点。先在这里做好准备，再安全返回。"},
    {"Craft the Laser Gun and Protection Suit before heading out.", "出发前先制作激光枪和防护服。"},
    {"Your gear is ready. Inspect the crash clue.", "装备已经齐备，去调查坠毁线索。"},
    {"Prepare the suit and camp before pushing into the deep swamp.", "深入深层沼泽前，请先准备防护服与营地。"},
    {"The fragments can now let Loxi compare rescue, repair, and settlement with full system context.", "这些碎片如今能让洛希在完整系统背景下比较救援、修复与定居。"},
    {"The fragments can now let Loxi align west crew traces with south facility handovers.", "这些碎片如今能让洛希把西线船员痕迹与南方设施交接记录对齐。"},
    {"Bring the fragments back to Loxi's terminal.", "把碎片带回洛希终端。"},
    {"Finish the fragment runs, then keep advancing the last archive trails so Loxi can finish reading what this ending really demands.", "完成碎片搜集后，继续推进最后几段档案线索，好让洛希读清这个结局到底真正要求你承担什么。"},
    {"Finish the fragment runs so Loxi can lock the west and south timeline together.", "完成碎片搜集，让洛希把西线与南线时间线彻底锁定。"},
    {"All mainline logs recovered. Return to Loxi and choose how to answer the full archive truth.", "所有主线日志都已回收。回到洛希处，决定你要如何回应完整档案揭示的真相。"},
    {"Heroic route chosen. Finish at the Signal Tower.", "已选择强行救援路线。前往信号塔完成最后一步。"},
    {"Heroic route chosen. Push the guardian when ready.", "已选择强行救援路线。准备好后就去推进守卫战。"},
    {"Heroic route chosen. The ring is ready. Push into the northwest ruins when you are ready for the guardian fight.", "已选择强行救援路线。石碑环已经准备完毕。准备好后推进西北遗迹，开始守卫战。"},
    {"Heroic route chosen. Push into the northwest ruins to start the guardian fight.", "已选择强行救援路线。推进西北遗迹，开始守卫战。"},
    {"Peaceful route chosen. Carry the amplifier to the tower.", "已选择和平救援路线。把放大器带到塔楼。"},
    {"Peaceful route chosen. Craft the amplifier at the workshop before leaving base.", "已选择和平救援路线。离开基地前先在工坊制作放大器。"},
    {"Peaceful route chosen. Recover 3 fragments, then craft the amplifier at the workshop before leaving base.", "已选择和平救援路线。先找回 3 枚碎片，再在工坊制作放大器后离开基地。"},
    {"Archive almost complete. One final sweep should let Loxi frame the ending choice with full context.", "档案已接近完整。再完成最后一次搜集，洛希就能在完整背景下解释结局选择。"},
    {"Archive almost complete. Last Camp and Root Vault are filed; one final ship-log sweep should let Loxi frame the ending choice with full context.", "档案已接近完整。最后营地与根脉核心都已归档；再完成最后一次飞船日志清扫，洛希就能在完整背景下梳理结局选择。"},
    {"Main archive is now the last blocker before Loxi's rewritten ending picture becomes a real choice.", "主线档案如今是最后一道阻碍。补完它后，洛希重写的终局图景才会变成真正可选的分支。"},
    {"Aligned crew and facility evidence is ready. Keep bringing back mainline proof.", "船员与设施证据已经对齐。继续把主线证据带回来。"},
    {"Final stage evidence sweep: collect truth first, choose an ending second.", "最终阶段的重点是搜集证据：先找齐真相，再选择结局。"},
    {"Base is the safest place to recover, craft, and sync.", "基地是恢复状态、制作装备和同步信息最安全的地方。"},
    {"Forest cover can break up long sight lines, but movement and route choice matter more than hiding in place.", "森林掩护能打断长距离视线，但移动节奏和路线选择比原地躲藏更重要。"},
    {"Outer Swamp Rim is the safer approach. Practice short runs and quick returns.", "外沼边缘是更安全的路线，适合练习短途往返。"},
    {"Flooded Detour is longer. Keep an oxygen buffer and a camp fallback ready.", "积水绕路更长，请保留氧气余量，并准备好营地作为回撤点。"},
    {"Use rope shortcuts if they help, and keep the Field Camp as your fallback.", "善用绳索捷径，并把野外营地留作回撤点。"},
    {"The rope can shorten the route, but you can finish the run without it.", "绳索能缩短路线，但没有它也能完成这段行程。"},
    {"The Field Camp gives you a safe fallback point.", "野外营地能提供一个安全的回撤点。"},
    {"The outer swamp drains oxygen steadily. Plan your return before you commit.", "外层沼泽会持续消耗氧气。推进之前务必先规划好返程。"},
    {"Deep Gate is a reliable turn-back checkpoint.", "深层入口是一个可靠的折返点。"},
    {"Deep Basin is high risk. Keep the suit and oxygen support ready.", "深潭区风险极高，请确保防护服和补氧支援随时可用。"},
    {"The Protection Suit helps, but the deep swamp is still dangerous.", "防护服会有帮助，但深层沼泽依旧危险。"},
    {"The deep swamp demands a Protection Suit and poison control.", "深入深层沼泽必须准备防护服和控毒手段。"},
    {"Peaceful route chosen. Push for tower stabilization.", "已选择和平救援路线。继续推进塔楼稳定化。"},
    {"Heroic route chosen. This is the best timing for the tower push after the guardian falls.", "已选择强行救援路线。守卫倒下后，这会是推进塔楼的最佳时机。"},
    {"Do not commit here blind. Return to Loxi once the archive is complete.", "不要在这里盲目做决定。等档案完整后，先回洛希处确认。"},
    {"Do not commit here blind. Return to Loxi and review the assembled archive first.", "不要在这里盲目做决定。先回到洛希处，复核整理好的档案。"},
    {"Plateau is dangerous. Follow the chosen route's prep before committing.", "高台区域非常危险。正式推进前，请按所选路线完成全部准备。"},
    {"Monolith ring complete.", "石碑环已完整点亮。"},
    {"Light remaining monoliths.", "点亮剩余石碑。"},
    {"Start lighting monoliths to weaken guardian.", "开始点亮石碑，以削弱守卫。"},
    {"Ruins Approach: prepare and decide push depth.", "遗迹前沿：先做好准备，再决定推进深度。"},
    {"Tower area: keep oxygen margin and retreat plan.", "塔楼区域：请保持足够氧气余量，并准备好撤退计划。"},
    {"Ruins have high oxygen pressure. Use short planned runs.", "遗迹区域的氧气压力很高，适合采用短线、计划好的推进。"},
    {"Northwest ruins are clear. Return to the ship and finish at the Signal Tower.", "西北遗迹已经清空。返回飞船，然后去信号塔完成最后一步。"},
    {"Northwest Ruins: the monolith ring is already softening this fight. Keep oxygen margin for the last phase.", "西北遗迹守卫战：石碑环已经在削弱这场战斗。为最后阶段保留足够氧气余量。"},
    {"Northwest Ruins: no route tricks remain. Stabilize oxygen and win the fight cleanly.", "西北遗迹守卫战：已经没有路线技巧可取巧了。稳住氧气，把这场战斗干净利落地打完。"},
    {"Unknown Stage", "未知阶段"},
    {"Day", "白天"},
    {"Dusk", "黄昏"},
    {"Night", "夜晚"},
    {"Unknown", "未知"},
    {"Resource Surge", "资源丰涌"},
    {"Quiet Creatures", "生物沉寂"},
    {"Clear Skies", "晴空"},
    {"Spore Storm", "孢雾风暴"},
    {"Monster Frenzy", "怪物狂乱"},
    {"Device Fault", "设备故障"},
    {"None", "无"},
    {"Interaction unavailable.", "当前无法交互。"},
    {"Resource status unavailable.", "当前无法读取资源状态。"},
    {"Crafting data unavailable.", "当前无法读取制作数据。"},
    {"You are too injured to gather safely right now.", "你现在伤势过重，无法安全采集。"},
    {"Your oxygen margin is too thin to gather safely right now.", "你当前的氧气余量过低，无法安全采集。"},
    {"A hostile creature is blocking the path.", "有敌对生物挡住了去路。"},
    {"That tile cannot be crossed.", "该地块无法通过。"},
    {"Loxi link is offline. Sync with the terminal bay uplink first, then press N.", "洛希链路尚未上线。请先在终端舱完成同步，再按 N。"},
    {"No ship logs collected yet.", "还没有收集到任何飞船日志。"},
    {"Username must be at least 3 characters.", "用户名至少需要 3 个字符。"},
    {"Username is too long.", "用户名过长。"},
    {"Username can only use letters, numbers, '_' or '-'.", "用户名只能使用字母、数字、下划线或连字符。"},
    {"Password must be at least 4 characters.", "密码至少需要 4 个字符。"},
    {"Password is too long.", "密码过长。"},
    {"Password must use visible ASCII characters.", "密码只能使用可见 ASCII 字符。"},
    {"Couldn't read local account data.", "无法读取本地账号数据。"},
    {"That account could not be found.", "找不到该账号。"},
    {"Couldn't remove the account from the local registry.", "无法从本地账号列表中移除此账号。"},
    {"Couldn't delete this account's save data.", "无法删除该账号的存档数据。"},
    {"Account not found. Create it first.", "未找到该账号，请先注册。"},
    {"Incorrect password.", "密码错误。"},
    {"Couldn't open this account's save folder.", "无法打开该账号的存档目录。"},
    {"That username already exists. Try signing in.", "该用户名已存在，请直接登录。"},
    {"Couldn't create the local account.", "无法创建本地账号。"},
    {"The account was created, but its save folder could not be opened.", "账号已创建，但无法打开对应的存档目录。"},
    {"There is no signed-in account to delete.", "当前没有可删除的已登录账号。"},
    {"The comm relay is restored. Loxi can now read the east route properly, and the west frontier archive has become recoverable as a real log trail instead of background debris.", "通讯中继已经恢复。洛希现在可以正确读取东线路线，而西部前线档案也终于能作为真正的日志线索被回收，而不再只是背景残骸。"},
    {"Comm relay repair still needs 2 Vines, 2 Shell Fruit, and 1 Special Fungus.", "修复通讯中继仍需要 2 份异星藤蔓、2 份壳果和 1 份特殊菌株。"},
    {"The comm relay is stable.", "通讯中继已经稳定。"},
    {"You found the crash clue. The residue points deeper east, the deep swamp entrance is now unlocked for the real qualification run, and the wreck's black-box log can finally be recovered.", "你找到了坠毁线索。残留物指向更深的东侧区域，深层沼泽入口现已开放，而残骸中的黑匣子日志也终于可以回收。"},
    {"The wreck is leaking dangerous residue. Better prepare a weapon and suit first if you want this clue to turn into a real lead.", "残骸仍在泄露危险残留物。如果你想把这条线索变成真正的突破口，最好先准备好武器和防护服。"},
    {"The wreck has already been fully explored. The residue trail is logged, and the eastern lead is now part of your route data.", "这处残骸已经彻底探索完毕。残留物轨迹已被记录，东线线索也已经纳入路线数据。"},
    {"The monolith is still dormant. Loxi suggests finishing the main prep first.", "石碑仍处于沉寂状态。洛希建议先完成主要准备工作。"},
    {"This monolith is already active. The full set is easing the final tower climb and keeping the guardian route more manageable.", "这座石碑已经激活。完整石碑环会减轻最后攀塔的压力，也让守卫路线更可控。"},
    {"This monolith is already resonating at the front of the sequence. Follow the ring to the next silent stone.", "这座石碑已经站在当前共鸣序列的前端。沿着石碑环去寻找下一座沉默的石碑。"},
    {"This monolith is already active. Follow the resonance to the next silent stone in the ring.", "这座石碑已经激活。沿着共鸣去寻找石碑环中的下一座沉默石碑。"},
    {"The ring rejects that order. Another silent stone should answer before this one.", "石碑环拒绝了这个顺序。在点亮它之前，应该先有另一座沉默石碑回应。"},
    {"The Signal Tower is still offline. Loxi suggests finishing the endgame prep first.", "信号塔仍未上线。洛希建议先完成终局准备。"},
    {"The tower remains unreadable. Recover the remaining mainline logs, finish the route archive work, then return to Loxi before committing to an ending.", "塔楼仍无法被正确解读。请先找回剩余主线日志、完成路线归档工作，再回到洛希处确认结局。"},
    {"Do not commit at the tower blind. Return to Loxi, review the complete archive, and choose the ending route at the ship first.", "不要在塔楼前盲目做决定。先回到洛希处，查看完整档案，并在飞船内先选定结局路线。"},
    {"The settlement route has already been chosen. The tower is no longer part of this ending.", "你已经选择了异星定居路线。塔楼不再属于这个结局的一部分。"},
    {"The guardian is down, the full monolith ring is stable, and west/south archive evidence confirms what this beacon costs. You followed the route chosen with Loxi, committed to the heroic ending, and sent the rescue beacon.", "守卫已经倒下，完整石碑环也已稳定，西线 / 南线档案证据共同说明了这道信标的代价。你遵循与洛希共同确认的路线，选择了强行救援结局，并发出了求救信标。"},
    {"The guardian is down. West and south archive findings now frame this as a deliberate heroic commitment, and you sent the rescue beacon after confirming the route with Loxi.", "守卫已经倒下。西线与南线档案如今共同证明，这是一次有意为之的强行救援承诺，而你也在与洛希确认路线后发出了求救信标。"},
    {"The guardian is down and the fully lit ring holds the route steady. You followed the chosen heroic route, manually activated the Signal Tower, and sent the rescue beacon.", "守卫已经倒下，而完整点亮的石碑环也稳住了这条路线。你遵循已选定的强行救援路线，手动启动了信号塔并发出了求救信标。"},
    {"The guardian is down. You followed the chosen heroic route, manually activated the Signal Tower, and sent the rescue beacon.", "守卫已经倒下。你遵循已选定的强行救援路线，手动启动了信号塔并发出了求救信标。"},
    {"The Signal Amplifier stabilized the tower without another fight. With west and south records aligned, this peaceful rescue now lands as a full-system repair choice chosen with full context back at the ship.", "信号放大器在无需再次战斗的情况下稳定了塔楼。随着西线与南线记录完全对齐，这场和平救援如今也被证明是一种在完整背景下作出的系统级修复选择。"},
    {"The Signal Amplifier stabilized the tower without another fight. The peaceful route chosen with Loxi still carries the lowest-risk rescue profile.", "信号放大器在无需再次战斗的情况下稳定了塔楼。你与洛希共同选定的和平路线，依然是风险最低的救援方案。"},
    {"The Signal Amplifier stabilized the tower without another fight. You followed the peaceful route chosen with Loxi, and the final plateau stayed calmer than the forced heroic climb.", "信号放大器在无需再次战斗的情况下稳定了塔楼。你遵循了与洛希共同选定的和平路线，而最终高台也比强行突进时平静得多。"},
    {"The heroic route is locked in, but the tower still detects the guardian. The fully lit ring has weakened it and softened the climb. Finish the kill, then return here.", "强行救援路线已经锁定，但塔楼仍能探测到守卫。完整点亮的石碑环已经削弱了它，也减轻了攀登压力。先完成击杀，再回到这里。"},
    {"The heroic route is locked in. Lit monoliths are helping, but you still need the guardian down before this tower will accept the final launch.", "强行救援路线已经锁定。已点亮的石碑正在提供帮助，但你仍需先击倒守卫，塔楼才会接受最终发射。"},
    {"The heroic route is locked in. Light the monolith ring if you want a safer push, then defeat the guardian before using the tower.", "强行救援路线已经锁定。如果你想让推进更安全，就先点亮石碑环，再击败守卫后使用塔楼。"},
    {"The heroic route is locked in, but the tower is waiting on the guardian kill in the northwest ruins. Push through the ruins approach, clear that fight, then return here.", "强行救援路线已经锁定，但塔楼仍在等待你清掉西北遗迹里的守卫。沿遗迹前沿推进，打完那场战斗后，再回到这里。"},
    {"The heroic route is locked in. The northwest-ruins fight is ready, and your lit monoliths will still help once the guardian engages.", "强行救援路线已经锁定。西北遗迹的守卫战已经就绪，而你点亮的石碑会在守卫开战后继续提供帮助。"},
    {"The heroic route is locked in. The ship is only your staging ground now. Clear the guardian in the northwest ruins before trying to launch the tower.", "强行救援路线已经锁定。飞船现在只剩下整备作用。先清掉西北遗迹里的守卫，再尝试启动塔楼。"},
    {"Loxi online. Heroic route confirmed. The guardian is holding the northwest ruins, and the completed monolith ring will help once you push into that fight.", "洛希已上线。强行救援路线已确认。守卫正在把守西北遗迹，而完整石碑环会在你推进那场战斗时提供帮助。"},
    {"Loxi online. Heroic route confirmed. The guardian is waiting in the northwest ruins. Any monolith prep you already completed will still help there.", "洛希已上线。强行救援路线已确认。守卫正在西北遗迹等着你。你此前完成的任何石碑准备，都会在那场战斗中继续生效。"},
    {"Airlock cycle complete. The world route is open; head through the ruins approach, win the guardian fight in the northwest ruins, then come back for the tower.", "气闸循环完成，外部路线已经打开。沿遗迹前沿推进，在西北遗迹赢下守卫战后，再回来完成塔楼。"},
    {"The guardian collapses in the northwest ruins. The Signal Tower is now the final step.", "守卫已经在西北遗迹倒下。信号塔现在就是最后一步。"},
    {"Base summary: guardian fight complete. The Signal Tower is now exposed.", "基地简报：守卫战已经完成。信号塔现在已经完全暴露出来。"},
    {"The peaceful route is locked in, but the tower still needs the Signal Amplifier. The fully lit ring is helping keep the climb stable while you prepare it.", "和平救援路线已经锁定，但塔楼仍需要信号放大器。完整点亮的石碑环会在你准备期间帮助稳定攀登。"},
    {"The peaceful route is locked in, but the tower still needs the Signal Amplifier. Lit monoliths are helping while you finish preparations.", "和平救援路线已经锁定，但塔楼仍需要信号放大器。在你完成准备期间，已点亮的石碑会提供帮助。"},
    {"The peaceful route is locked in, but the tower still needs the Signal Amplifier before activation.", "和平救援路线已经锁定，但塔楼在启动前仍需要信号放大器。"},
    {"You rested at the field camp and recovered part of your health and oxygen. It is a safe outdoor fallback, not a full reset, and it leaves a short Camp Recovery boost.", "你在野外营地休息了一次，恢复了部分生命与氧气。这里是安全的户外回撤点，但并非完全重置，同时还会留下短暂的营地恢复增益。"},
    {"You used the rope to secure a shortcut across the hazardous terrain.", "你利用绳索在危险地形之间固定出一条捷径。"},
    {"Flooded Detour is longer. Keep oxygen margin and a camp fallback ready.", "被淹没的岔路更长。请提前留足氧气余量，并准备营地回撤点。"},
    {"Use rope shortcuts if helpful, and keep the Field Camp as fallback.", "如果顺路，绳索捷径会有帮助；同时把野外营地当作回撤点。"},
    {"Rope can shorten the route, but it is optional.", "绳索可以缩短路线，但并非必需。"},
    {"Workshop: prepare camp supplies, oxygen backup, and combat gear for the eastern push.", "工坊：为东线推进准备营地补给、补氧手段和战斗装备。"},
    {"Base summary: east relay run complete. Debrief at Loxi.", "基地简报：东侧中继行动已完成。回洛希处汇报。"},
    {"Base summary: Energy Core returned. Install it in Power Bay.", "基地简报：能源核心已带回。请将它安装到动力舱。"},
    {"Base summary: prepare gear before next east deep run.", "基地简报：在下一次深入东侧前，先补齐装备。"},
    {"Base summary: east complete. Recover and pivot north.", "基地简报：东线阶段已完成。先恢复状态，再转向北线。"},
    {"Base summary: east run complete. Recover and continue.", "基地简报：东线行动已完成。先恢复状态，再继续推进。"},
    {"Base summary: fragment set complete. Sync with Loxi.", "基地简报：碎片已收齐。去和洛希同步。"},
    {"Base summary: continue north fragment runs after recovery.", "基地简报：恢复后继续推进北线碎片搜集。"},
    {"Base summary: guardian fight complete. Finish the heroic route at the Signal Tower.", "基地简报：守卫战已经完成。前往信号塔完成强行救援路线。"},
    {"Base summary: heroic route locked. Push through the ruins approach and clear the guardian in the northwest ruins when you are ready.", "基地简报：强行救援路线已经锁定。准备好后就穿过遗迹前沿，在西北遗迹清掉守卫。"},
    {"Base summary: peaceful route locked. Carry the Signal Amplifier to the Signal Tower.", "基地简报：和平救援路线已经锁定。把信号放大器带到信号塔。"},
    {"Base summary: peaceful route locked. Craft the Signal Amplifier before returning to the tower.", "基地简报：和平救援路线已经锁定。先制作信号放大器，再返回塔楼。"},
    {"Base summary: archive review complete. Confirm the final route with Loxi.", "基地简报：档案复核已经完成。去洛希处确认最终路线。"},
    {"Base summary: the archive is assembled. Return to Loxi for the final review.", "基地简报：档案已经拼合完成。返回洛希处进行最终复核。"},
    {"Base summary: boss defeated. Return to Loxi and commit to the final route.", "基地简报：守卫已被击败。返回洛希处，正式确认最终路线。"},
    {"Base summary: Signal Amplifier ready. Return to Loxi and commit to the final route.", "基地简报：信号放大器已经准备好。返回洛希处，正式确认最终路线。"},
    {"Base summary: boss defeated. Choose rescue or settlement.", "基地简报：守卫已被击败。现在可以选择救援或定居。"},
    {"Base summary: peaceful route available. Choose rescue or settlement.", "基地简报：和平路线已经可用。现在可以选择救援或定居。"},
    {"Base summary: monolith ring complete. Prepare for final push.", "基地简报：石碑环已完整点亮。准备最后推进。"},
    {"Base summary: monolith progress made. Recover before next push.", "基地简报：石碑推进已有进展。下一次推进前先恢复状态。"},
    {"Base summary: north is the active endgame route.", "基地简报：北线是当前的终局主路线。"},
    {"Base summary: the final west archive has been filed.", "基地简报：西线最终档案已归档。"},
    {"Base summary: the final south archive has been filed.", "基地简报：南线最终档案已归档。"},
    {"Base summary: west and south evidence now supports a fully informed rescue-versus-settlement choice.", "基地简报：西线与南线证据现已足够支撑一次完整知情的救援 / 定居抉择。"},
    {"Base summary: Echo Basin findings are archived. The west route now points toward Last Camp.", "基地简报：回声盆地线索已归档。西线路线现已指向最后营地。"},
    {"Base summary: Purifier Ring controls are archived. The south route now points toward the Root Vault.", "基地简报：净化环控制记录已归档。南线路线现已指向根脉核心。"},
    {"Base summary: Loxi can now redraw the final choice using both investigations.", "基地简报：洛希现在可以结合两条调查线重构最终抉择。"},
    {"Base summary: Canopy Hollow evidence is archived. The west route now points toward Echo Basin.", "基地简报：林冠洼地证据已归档。西线路线现已指向回声盆地。"},
    {"Base summary: Service Shaft records are archived. The south route now points toward the Purifier Ring.", "基地简报：维护井道记录已归档。南线路线现已指向净化环区。"},
    {"Base summary: west traces and south facility records now align into one investigation.", "基地简报：西线痕迹与南方设施记录现已对齐成同一场调查。"},
    {"Base summary: Survey Break anchors are archived. The west trail now points deeper into Canopy Hollow.", "基地简报：勘测断点锚点已归档。西线踪迹现已继续深入林冠洼地。"},
    {"Base summary: Vent calibration records are archived. The south route now points deeper into the Service Shafts.", "基地简报：通风校准记录已归档。南线路线现已继续深入维护井道。"},
    {"Base summary: the opening west signal fragment is archived.", "基地简报：西线起始信号残片已归档。"},
    {"Base summary: the South Collapse outage memo is archived.", "基地简报：南部塌陷区停机备忘已归档。"},
    {"Ruins Approach: this is still the last low-commitment prep lane. Stabilize oxygen, suit, and fallback before you push farther north.", "遗迹前沿：这里仍是最后一段低投入准备路线。继续向北推进前，请先稳住氧气、防护服和回撤手段。"},
    {"Ruins Approach: gather fragments around the ring, then fall back to Loxi before the final route opens.", "遗迹前沿：先在石碑环周围收集碎片，再回到洛希处，等待最终路线开启。"},
    {"Monolith Ring: the full resonance is active. This is your cleanest transition point from prep into the final north execution.", "石碑环区：完整共鸣已经启动。这是你从准备阶段转入北线最终执行的最佳节点。"},
    {"Monolith Ring: only one silent stone remains. Finish it now if you want the cleanest heroic push before the plateau.", "石碑环区：只剩最后一座沉默石碑。如果你想在登上高台前拥有最顺畅的强攻窗口，现在就完成它。"},
    {"Monolith Ring: the first stone is helping, but the route still wants one more real prep loop before the plateau commit.", "石碑环区：第一座石碑已经开始发挥作用，但在正式登上高台前，这条路线仍建议你再完成一轮真正的准备。"},
    {"Monolith Ring: this is the prep-to-execution hinge. Light the stones here before treating the plateau like a real final climb.", "石碑环区：这里就是准备与执行之间的转折点。把这里的石碑点亮后，再把高台当作真正的最终攀登。"},
    {"Signal Tower Plateau: the guardian is down. Finish the rescue beacon now, or fall back and return to base for settlement.", "信号塔高台：守卫已经倒下。现在就完成求救信标，或者撤回基地转而选择定居。"},
    {"Signal Tower Plateau: peaceful route is live. The amplifier is calming the tower systems, but this is still the final oxygen commit.", "信号塔高台：和平路线已经开启。放大器正在安抚塔楼系统，但这里仍是最后一次高氧气压力投入。"},
    {"Signal Tower Plateau: the full ring is lit. This is your cleanest heroic window, but the final climb is still a hard commitment.", "信号塔高台：完整石碑环已被点亮。这是你最好的强攻窗口，但最后的攀登依旧需要下定决心。"},
    {"Signal Tower Plateau: partial ring prep is helping, but you are entering the final climb early. Retreat if oxygen is not stable.", "信号塔高台：部分石碑准备正在发挥作用，但你仍是过早进入了最终攀登。如果氧气不稳定，请立刻撤退。"},
    {"Signal Tower Plateau: this is the final climb. Expect heavy oxygen leaks and no cheap recovery beyond this point.", "信号塔高台：这里就是最后的攀登。请预期强烈漏氧，而且再往前几乎没有廉价恢复机会。"}
};

const NamedTranslation *FindTranslation(const NamedTranslation *items, size_t count, const char *english) {
    size_t index;

    if (english == NULL) {
        return NULL;
    }

    for (index = 0; index < count; ++index) {
        if (std::strcmp(items[index].english, english) == 0) {
            return &items[index];
        }
    }

    return NULL;
}

bool ExtractMiddle(const char *text, const char *prefix, const char *suffix, std::string *middle) {
    size_t prefixLength;
    size_t suffixLength;
    size_t textLength;

    if (text == NULL || prefix == NULL || suffix == NULL || middle == NULL) {
        return false;
    }

    prefixLength = std::strlen(prefix);
    suffixLength = std::strlen(suffix);
    textLength = std::strlen(text);
    if (textLength < prefixLength + suffixLength) {
        return false;
    }
    if (std::strncmp(text, prefix, prefixLength) != 0) {
        return false;
    }
    if (std::strcmp(text + textLength - suffixLength, suffix) != 0) {
        return false;
    }

    *middle = std::string(text + prefixLength, textLength - prefixLength - suffixLength);
    return true;
}

bool ExtractDoubleMiddle(const char *text,
                         const char *prefix,
                         const char *middleMarker,
                         const char *suffix,
                         std::string *first,
                         std::string *second) {
    std::string tail;

    if (!ExtractMiddle(text, prefix, suffix, &tail) || first == NULL || second == NULL) {
        return false;
    }

    size_t marker = tail.find(middleMarker);
    if (marker == std::string::npos) {
        return false;
    }

    *first = tail.substr(0, marker);
    *second = tail.substr(marker + std::strlen(middleMarker));
    return true;
}

const char *TranslateLocationName(const std::string &english) {
    const NamedTranslation *entry;

    entry = FindTranslation(kLocationNameTranslations.data(), kLocationNameTranslations.size(), english.c_str());
    return entry != NULL ? entry->simplifiedChinese : english.c_str();
}

bool TryTranslatePattern(const char *english, std::string *translated) {
    std::string one;
    std::string two;

    if (english == NULL || translated == NULL) {
        return false;
    }

    if (ExtractMiddle(english, "Finish the ", " investigation and return to base.", &one)
        || ExtractMiddle(english, "Finish the ", " investigation here and return to base.", &one)) {
        *translated = std::string("完成") + TranslateLocationName(one) + "调查后返回基地。";
        return true;
    }

    if (ExtractMiddle(english, "Finish the ", " objectives and return to base.", &one)) {
        *translated = std::string("完成") + TranslateLocationName(one) + "目标后返回基地。";
        return true;
    }

    if (ExtractMiddle(english, "The ", " investigation is active. Complete it and return.", &one)
        || ExtractMiddle(english, "The ", " investigation is active here. Complete it and return.", &one)) {
        *translated = std::string(TranslateLocationName(one)) + "调查已开启，完成后返回。";
        return true;
    }

    if (ExtractMiddle(english, "Complete the ", " investigation first.", &one)) {
        *translated = std::string("请先完成") + TranslateLocationName(one) + "调查。";
        return true;
    }

    if (ExtractDoubleMiddle(english, "", " archive complete. Start the ", " investigation.", &one, &two)) {
        *translated = std::string(TranslateLocationName(one)) + "档案已归档。开始" + TranslateLocationName(two) + "调查。";
        return true;
    }

    if (ExtractDoubleMiddle(english, "", " archive complete. Proceed to ", ".", &one, &two)
        || ExtractDoubleMiddle(english, "", " archive complete. Move to ", ".", &one, &two)) {
        *translated = std::string(TranslateLocationName(one)) + "档案已归档。前往" + TranslateLocationName(two) + "。";
        return true;
    }

    if (ExtractDoubleMiddle(english, "", " archive complete. Start ", ".", &one, &two)) {
        *translated = std::string(TranslateLocationName(one)) + "档案已归档。开始" + TranslateLocationName(two) + "。";
        return true;
    }

    if (ExtractMiddle(english, "", " investigation in progress. Finish and return.", &one)) {
        *translated = std::string(TranslateLocationName(one)) + "调查进行中。完成后返回。";
        return true;
    }

    if (ExtractMiddle(english, "", " investigation in progress.", &one)) {
        *translated = std::string(TranslateLocationName(one)) + "调查进行中。";
        return true;
    }

    if (ExtractMiddle(english, "", " archive complete.", &one)) {
        *translated = std::string(TranslateLocationName(one)) + "档案已归档。";
        return true;
    }

    if (ExtractDoubleMiddle(english, "The ", " investigation is active here after the ", " archive is filed.", &one, &two)) {
        *translated = std::string(TranslateLocationName(two)) + "档案归档后，" + TranslateLocationName(one) + "调查已在此开启。";
        return true;
    }

    if (ExtractMiddle(english, "", " locked. Restore comm relay first.", &one)) {
        *translated = std::string(TranslateLocationName(one)) + "尚未开放。先修复通讯中继。";
        return true;
    }

    if (ExtractMiddle(english, "", " locked. Restore power bay first.", &one)) {
        *translated = std::string(TranslateLocationName(one)) + "尚未开放。先恢复动力舱。";
        return true;
    }

    if (ExtractMiddle(english, "", " stays closed until the comm relay is restored.", &one)) {
        *translated = std::string("通讯中继修复前，") + TranslateLocationName(one) + "仍然关闭。";
        return true;
    }

    if (ExtractMiddle(english, "", " stays closed until the Power Bay is restored.", &one)) {
        *translated = std::string("动力舱恢复前，") + TranslateLocationName(one) + "仍然关闭。";
        return true;
    }

    if (ExtractMiddle(english,
                      "Base summary: the record is still open. Bring back the ",
                      " before Loxi can close it.",
                      &one)) {
        const std::string suffix = " record";
        std::string locationName = one;

        if (locationName.size() > suffix.size()
            && locationName.compare(locationName.size() - suffix.size(), suffix.size(), suffix) == 0) {
            locationName.erase(locationName.size() - suffix.size());
        }

        *translated = std::string("基地简报：该记录仍未完成。先把")
            + TranslateLocationName(locationName)
            + "记录带回，洛希才能归档。";
        return true;
    }

    return false;
}

}  // namespace

GameLanguage Loc_GetLanguage(void) {
    return gCurrentLanguage;
}

void Loc_SetLanguage(GameLanguage language) {
    gCurrentLanguage = Loc_NormalizeLanguage((int)language);
}

GameLanguage Loc_NormalizeLanguage(int languageValue) {
    if (languageValue == (int)GAME_LANGUAGE_ZH_CN) {
        return GAME_LANGUAGE_ZH_CN;
    }

    return GAME_LANGUAGE_EN;
}

const char *Loc_PickLiteral(const char *english, const char *simplifiedChinese) {
    if (gCurrentLanguage == GAME_LANGUAGE_ZH_CN && simplifiedChinese != NULL && simplifiedChinese[0] != '\0') {
        return simplifiedChinese;
    }

    return english != NULL ? english : "";
}

const char *Loc_PickText(LocalizedText text) {
    return Loc_PickLiteral(text.english, text.simplifiedChinese);
}

const char *Loc_GetText(LocTextId id) {
    if (id < 0 || id >= LOC_TEXT_COUNT) {
        return "";
    }

    return Loc_PickText(kTextTable[(size_t)id]);
}

const char *Loc_Translate(const char *english) {
    const NamedTranslation *entry;
    static std::string translated;

    if (english == NULL || english[0] == '\0' || gCurrentLanguage != GAME_LANGUAGE_ZH_CN) {
        return english != NULL ? english : "";
    }

    entry = FindTranslation(kLiteralTranslations, sizeof(kLiteralTranslations) / sizeof(kLiteralTranslations[0]), english);
    if (entry != NULL) {
        return entry->simplifiedChinese;
    }

    if (TryTranslatePattern(english, &translated)) {
        return translated.c_str();
    }

    return english;
}

const char *Loc_GetLanguageNativeName(GameLanguage language) {
    switch (Loc_NormalizeLanguage((int)language)) {
        case GAME_LANGUAGE_ZH_CN:
            return "简体中文";
        case GAME_LANGUAGE_EN:
        default:
            return "English";
    }
}

const char *Loc_GetAreaNameText(const char *canonicalEnglish) {
    const NamedTranslation *entry;

    if (Loc_GetLanguage() != GAME_LANGUAGE_ZH_CN) {
        return canonicalEnglish != NULL ? canonicalEnglish : "";
    }

    entry = FindTranslation(kAreaNameTranslations.data(), kAreaNameTranslations.size(), canonicalEnglish);
    return entry != NULL ? entry->simplifiedChinese : (canonicalEnglish != NULL ? canonicalEnglish : "");
}

const char *Loc_GetLocationNameText(const char *canonicalEnglish) {
    const NamedTranslation *entry;

    if (Loc_GetLanguage() != GAME_LANGUAGE_ZH_CN) {
        return canonicalEnglish != NULL ? canonicalEnglish : "";
    }

    entry = FindTranslation(kLocationNameTranslations.data(), kLocationNameTranslations.size(), canonicalEnglish);
    return entry != NULL ? entry->simplifiedChinese : (canonicalEnglish != NULL ? canonicalEnglish : "");
}
