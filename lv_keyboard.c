
/**
 * @file lv_kb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_keyboard.h"
#if LV_USE_KEYBOARD != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "lv_textarea.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_keyboard"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_keyboard_signal(lv_obj_t * kb, lv_signal_t sign, void * param);
static void lv_keyboard_update_map(lv_obj_t * kb);
static void lv_keyboard_show_match_ch(lv_obj_t * kb, uint8_t mode, char ch);
static void lv_keyboard_match_clear(lv_obj_t * kb);
static lv_event_cb_t match_word_click_cb (lv_obj_t * label, lv_event_t e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
/* clang-format off */
static const char * const default_kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                 "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 "أب",
#endif
                                                 LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_lc_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#endif
    2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const default_kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                                 "Ch", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 "أب",
#endif
                                                 LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_uc_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#endif
    2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
static const char * const default_kb_map_ar[] = {
    "1#", "ض", "ص", "ث", "ق", "ف", "غ", "ع", "ه", "خ", "ح", "ج", "\n",
    "ش", "س", "ي", "ب", "ل", "ا", "ت", "ن", "م", "ك", "ط", LV_SYMBOL_BACKSPACE, "\n",
    "ذ", "ء", "ؤ", "ر", "ى", "ة", "و", "ز", "ظ", "د", "ز", "ظ", "د", "\n",
    LV_SYMBOL_CLOSE, "abc", LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_NEW_LINE, LV_SYMBOL_OK, ""
};

static const lv_btnmatrix_ctrl_t default_kb_ctrl_ar_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 2, 6, 2, 3, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};
#endif

static const char * const default_kb_map_spec[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0", LV_SYMBOL_BACKSPACE, "\n",
                                                   "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                                   "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                                   LV_SYMBOL_CLOSE,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                   "أب",
#endif
                                                   LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                  };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_spec_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#endif
    2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * const default_kb_map_num[] = {"1", "2", "3", LV_SYMBOL_CLOSE, "\n",
                                                  "4", "5", "6", LV_SYMBOL_OK, "\n",
                                                  "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                                  "+/-", "0", ".", LV_SYMBOL_LEFT, LV_SYMBOL_RIGHT, ""
                                                 };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_num_map[] = {
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
    1, 1, 1, 2,
    1, 1, 1, 1, 1
};
/* clang-format on */

static const char * const default_kb_map_ch[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                                 "abc", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                                 "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                                 LV_SYMBOL_CLOSE,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
                                                 "أب",
#endif
                                                 LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""
                                                };

static const lv_btnmatrix_ctrl_t default_kb_ctrl_ch_map[] = {
    LV_KEYBOARD_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    LV_KEYBOARD_CTRL_BTN_FLAGS | 2,
#endif
    2, 6, 2, LV_KEYBOARD_CTRL_BTN_FLAGS | 2
};

static const char * * kb_map[] = {
    (const char * *)default_kb_map_lc,
    (const char * *)default_kb_map_uc,
    (const char * *)default_kb_map_spec,
    (const char * *)default_kb_map_num,
    (const char * *)default_kb_map_ch
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    , (const char * *)default_kb_map_ar
#endif
};
static const lv_btnmatrix_ctrl_t * kb_ctrl[] = {
    default_kb_ctrl_lc_map,
    default_kb_ctrl_uc_map,
    default_kb_ctrl_spec_map,
    default_kb_ctrl_num_map,
    default_kb_ctrl_ch_map
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    , default_kb_ctrl_ar_map
#endif
};

//汉字排列表
const char py_code_table_a     []={"啊阿腌吖锕厑嗄錒呵腌"};
const char py_code_table_ai    []={"爱埃挨哎唉哀皑癌蔼矮艾碍隘捱嗳嗌嫒瑷暧砹锿霭"};
const char py_code_table_an    []={"安俺按暗岸案鞍氨谙胺埯揞犴庵桉铵鹌黯"};
const char py_code_table_ang   []={"昂盎肮骯"};
const char py_code_table_ao    []={"凹敖熬翱袄傲奥懊澳坳拗嗷螯鏊鳌鏖岙廒遨媪骜獒聱"};
const char py_code_table_ba    []={"八巴爸拔芭捌扒叭吧笆疤跋靶把耙坝霸罢茇菝岜灞钯粑鲅魃"};
const char py_code_table_bai   []={"白柏百摆佰败拜稗伯捭掰"};
const char py_code_table_ban   []={"半办斑班搬扳般颁板版扮拌伴瓣绊阪坂贲钣瘢癍舨"};
const char py_code_table_bang  []={"邦帮梆榜膀绑棒磅蚌镑傍谤蒡浜"};
const char py_code_table_bao   []={"包宝保抱报暴苞胞褒剥薄雹堡饱豹鲍爆刨炮勹葆孢煲鸨褓趵龅"};
const char py_code_table_bei   []={"背贝北倍杯碑悲卑辈钡狈备惫焙被臂孛陂邶埤萆蓓呗悖碚鹎褙鐾鞴"};
const char py_code_table_ben   []={"本笨奔苯畚坌贲锛"};
const char py_code_table_beng  []={"蚌崩绷甭泵蹦迸堋嘣甏"};
const char py_code_table_bi    []={"逼鼻比鄙笔彼碧蓖蔽毕毙毖币庇痹闭匕敝弊必辟壁臂避陛秘泌俾埤芘荜荸萆薜吡哔狴庳愎滗濞弼妣婢嬖璧贲畀铋秕裨筚箅篦舭襞跸髀"};
const char py_code_table_bian  []={"边编贬便变卞鞭扁辨辩辫遍匾弁苄忭汴缏煸砭碥窆褊蝙笾鳊"};
const char py_code_table_biao  []={"表标彪膘婊骠杓飑飙飚镖镳瘭裱鳔髟"};
const char py_code_table_bie   []={"别鳖憋瘪蹩"};
const char py_code_table_bin   []={"彬斌濒滨宾摈傧豳缤玢槟殡膑镔髌鬓"};
const char py_code_table_bing  []={"兵冰柄丙秉饼炳病并屏禀冫邴摒槟"};
const char py_code_table_bo    []={"柏百剥薄博玻菠卜播拨钵波勃搏铂箔伯帛舶脖膊渤泊驳孛亳蕃啵饽檗擘礴钹鹁簸趵跛踣"};
const char py_code_table_bu    []={"不布步堡捕卜哺补埠簿部怖埔卟逋瓿晡钚钸醭"};
const char py_code_table_ca    []={"擦嚓礤"};
const char py_code_table_cai   []={"才财睬猜裁材踩采彩菜蔡"};
const char py_code_table_can   []={"餐参蚕残惭惨灿孱骖璨粲黪"};
const char py_code_table_cang  []={"苍舱仓沧藏伧"};
const char py_code_table_cao   []={"草操糙槽曹艹嘈漕螬艚"};
const char py_code_table_ce    []={"侧册测厕策恻"};
const char py_code_table_cen   []={"参岑涔"};
const char py_code_table_ceng  []={"层蹭曾噌"};
const char py_code_table_cha   []={"插叉茬茶查碴搽察岔差诧刹喳衩嚓猹馇汊姹杈楂槎檫锸镲"};
const char py_code_table_chai  []={"差拆柴豺侪钗瘥虿龇"};
const char py_code_table_chan  []={"产阐颤搀掺蝉馋谗缠铲孱单冁谄蒇廛忏潺澶羼婵觇禅镡蟾躔"};
const char py_code_table_chang []={"长厂昌猖场尝常偿肠敞畅唱倡伥鬯苌菖徜怅惝阊娼嫦昶氅鲳"};
const char py_code_table_chao  []={"超抄钞朝嘲潮巢吵炒绰剿怊晁耖"};
const char py_code_table_che   []={"车扯撤掣彻澈坼屮砗"};
const char py_code_table_chen  []={"郴臣辰尘晨忱沉陈趁衬称谌谶抻嗔宸琛榇碜龀"};
const char py_code_table_cheng []={"撑称城橙成呈乘程惩澄诚承逞骋秤盛丞埕噌徵枨柽塍瞠铖铛裎蛏酲"};
const char py_code_table_chi   []={"吃痴持匙池迟弛驰耻齿侈尺赤翅斥炽笞叱哧傺坻墀茌啻嗤彳饬媸敕眵鸱瘛褫蚩螭篪豉踟魑"};
const char py_code_table_chong []={"充冲虫崇宠种重茺忡憧铳舂艟雠"};
const char py_code_table_chou  []={"抽酬丑畴踌稠愁筹仇绸瞅臭俦帱惆瘳雠"};
const char py_code_table_chu   []={"出初橱厨躇锄雏滁除楚础储矗搐触处畜亍刍怵憷绌杵楮樗褚蜍蹰黜"};
const char py_code_table_chuai []={"揣搋嘬膪踹"};
const char py_code_table_chuan []={"川穿椽传船喘串舛遄巛氚钏舡"};
const char py_code_table_chuang[]={"疮窗幢床闯创怆疒"};
const char py_code_table_chui  []={"吹炊捶锤垂陲棰槌"};
const char py_code_table_chun  []={"春椿醇唇淳纯蠢莼鹑蝽"};
const char py_code_table_chuo  []={"戳绰啜辶辍踔龊"};
const char py_code_table_ci    []={"此刺赐次伺差疵茨磁雌辞慈瓷词兹茈呲祠鹚粢糍"};
const char py_code_table_cong  []={"从丛聪葱囱匆苁淙骢琮璁枞"};
const char py_code_table_cou   []={"凑楱辏腠"};
const char py_code_table_cu    []={"粗醋簇促蔟徂猝殂酢蹙蹴"};
const char py_code_table_cuan  []={"蹿篡窜攒汆撺爨镩"};
const char py_code_table_cui   []={"摧崔催脆瘁粹淬翠萃啐悴璀榱毳隹"};
const char py_code_table_cun   []={"村存寸忖皴"};
const char py_code_table_cuo   []={"磋撮搓措挫错厝嵯脞锉矬痤瘥鹾蹉躜"};
const char py_code_table_da    []={"大搭达答瘩打耷哒嗒怛妲沓疸褡笪靼鞑"};
const char py_code_table_dai   []={"大呆歹傣戴带殆代贷袋待逮怠黛埭甙呔岱迨骀绐玳"};
const char py_code_table_dan   []={"耽担丹单郸掸胆旦氮但惮淡诞弹蛋赡石儋萏啖澹殚赕眈疸瘅聃箪"};
const char py_code_table_dang  []={"当挡党荡档谠凼菪宕砀铛裆"};
const char py_code_table_dao   []={"刀捣蹈倒岛祷导到稻悼道盗刂叨帱忉氘焘纛"};
const char py_code_table_de    []={"德得的地锝"};
const char py_code_table_dei   []={"得"};
const char py_code_table_deng  []={"澄蹬灯登等瞪凳邓噔嶝戥磴镫簦"};
const char py_code_table_di    []={"的抵底地蒂第帝弟递堤低滴迪敌笛狄涤翟嫡缔提氐籴诋谛邸坻荻嘀娣柢棣觌砥碲睇镝羝骶"};
const char py_code_table_dia   []={"嗲"};
const char py_code_table_dian  []={"电店惦点典靛奠淀殿颠掂滇碘垫佃甸阽坫巅玷钿癜癫簟踮"};
const char py_code_table_diao  []={"吊刁掉钓调碉叼雕凋铞铫貂鲷"};
const char py_code_table_die   []={"跌爹碟蝶迭谍叠垤堞揲喋牒瓞耋蹀鲽"};
const char py_code_table_ding  []={"丁盯叮钉顶鼎锭定订仃啶玎腚碇町铤疔耵酊"};
const char py_code_table_diu   []={"丢铥"};
const char py_code_table_dong  []={"东冬董懂动栋侗恫冻洞咚岽峒氡胨胴硐鸫"};
const char py_code_table_dou   []={"豆逗痘都兜抖斗陡蔸窦蚪篼"};
const char py_code_table_du    []={"肚度渡都督毒犊独读堵睹赌杜镀妒芏嘟渎椟牍蠹笃髑黩"};
const char py_code_table_duan  []={"端短锻段断缎椴煅簖"};
const char py_code_table_dui   []={"堆兑队对怼憝碓"};
const char py_code_table_dun   []={"盾墩吨蹲敦顿囤钝遁沌炖砘礅盹镦趸"};
const char py_code_table_duo   []={"多朵度掇哆夺垛躲跺舵剁惰堕驮咄哚沲缍铎裰踱"};
const char py_code_table_e     []={"阿蛾峨鹅俄额讹娥恶厄扼遏鄂饿哦噩谔垩苊莪萼呃愕屙婀轭腭锇锷鹗颚鳄"};
const char py_code_table_ei    []={"诶"};
const char py_code_table_en    []={"恩蒽摁"};
const char py_code_table_er    []={"二而儿耳尔饵洱贰迩珥铒鸸鲕"};
const char py_code_table_fa    []={"发罚筏伐乏阀法珐垡砝"};
const char py_code_table_fan   []={"凡烦反返范贩犯饭泛藩帆番翻樊矾钒繁蕃蘩幡梵燔畈蹯"};
const char py_code_table_fang  []={"坊芳方肪房防妨仿访纺放匚邡枋钫舫鲂"};
const char py_code_table_fei   []={"菲非啡飞肥匪诽吠肺废沸费芾狒斐悱妃绯淝榧贲腓扉砩镄痱蜚篚翡霏鲱"};
const char py_code_table_fen   []={"分芬奋份忿愤粪酚吩氛纷坟焚汾粉偾瀵玢棼贲鲼鼢"};
const char py_code_table_feng  []={"丰封枫蜂峰锋风凤疯烽逢冯缝讽奉俸酆葑唪沣砜"};
const char py_code_table_fo    []={"佛"};
const char py_code_table_fou   []={"否缶"};
const char py_code_table_fu    []={"复佛傅付阜父夫敷肤孵妇扶拂辐幅氟符伏俘服浮涪福袱弗甫抚辅俯釜斧脯腑府腐赴副覆赋腹负富讣附缚咐匐凫阝郛芙芾苻茯莩菔拊呋幞怫滏艴孚驸绂绋桴赙祓砩黻黼罘稃馥蚨蜉蝠蝮麸趺跗鲋鳆"};
const char py_code_table_ga    []={"噶嘎夹咖伽尬尕尜旮钆"};
const char py_code_table_gai   []={"该改概钙盖溉芥丐陔垓戤赅胲"};
const char py_code_table_gan   []={"干甘杆柑竿肝赶感秆敢赣坩苷尴擀泔淦澉绀橄旰矸疳酐"};
const char py_code_table_gang  []={"冈刚钢缸肛纲岗港杠扛戆罡筻"};
const char py_code_table_gao   []={"告皋高膏篙羔糕搞镐稿睾诰郜藁缟槔槁杲锆"};
const char py_code_table_ge    []={"个各盖哥骼膈歌搁戈鸽胳疙割革葛格蛤阁隔铬合咯鬲仡哿圪塥嗝纥搿铪镉袼虼舸"};
const char py_code_table_gei   []={"给"};
const char py_code_table_gen   []={"根跟亘茛哏艮"};
const char py_code_table_geng  []={"耕更庚羹埂耿梗颈哽赓绠鲠"};
const char py_code_table_gong  []={"工共攻功恭公宫弓龚供躬巩汞拱贡蚣廾珙肱觥"};
const char py_code_table_gou   []={"钩勾沟苟够垢构购佝诟狗篝岣遘媾缑枸觏彀笱鞲"};
const char py_code_table_gu    []={"姑估沽孤菇咕辜箍鼓古蛊骨谷股故顾固雇贾嘏诂菰崮汩梏轱牯牿臌毂瞽罟钴锢鸪鹄痼蛄酤觚鲴鹘"};
const char py_code_table_gua   []={"刮瓜剐寡挂褂卦诖呱栝胍鸹"};
const char py_code_table_guai  []={"乖拐怪掴"};
const char py_code_table_guan  []={"关官冠观管馆罐惯灌贯纶倌莞棺掼涫盥鹳鳏"};
const char py_code_table_guang []={"光广逛咣犷桄胱"};
const char py_code_table_gui   []={"瑰规圭硅归龟闺轨鬼诡桂柜跪贵刽癸炔匦刿庋宄妫桧炅晷皈簋鲑鳜"};
const char py_code_table_gun   []={"辊滚棍衮绲磙鲧"};
const char py_code_table_guo   []={"锅郭国果裹过涡馘埚掴呙帼崞猓椁虢锞聒蜾蝈"};
const char py_code_table_ha    []={"哈蛤铪"};
const char py_code_table_hai   []={"还咳孩海害氦亥骇骸嗨胲醢"};
const char py_code_table_han   []={"汗汉喊寒含涵函罕酣旱憾悍焊憨邯韩翰撼鼾捍邗菡撖瀚晗焓顸颔蚶"};
const char py_code_table_hang  []={"行夯杭航吭巷沆绗颃"};
const char py_code_table_hao   []={"郝好耗号昊皓毫浩灏镐壕嚎豪貉蒿薅嗥嚆濠颢蚝"};
const char py_code_table_he    []={"呵喝荷菏核禾和何合河盒貉阂涸赫贺褐鹤吓诃劾壑嗬阖纥曷盍颌蚵翮"};
const char py_code_table_hei   []={"黑嘿"};
const char py_code_table_hen   []={"痕很狠恨"};
const char py_code_table_heng  []={"哼亨横衡恒蘅珩桁"};
const char py_code_table_hong  []={"红轰哄烘虹鸿洪宏弘黉訇讧荭蕻薨闳泓"};
const char py_code_table_hou   []={"后喉侯猴吼厚候堠後逅瘊篌糇鲎骺"};
const char py_code_table_hu    []={"护互沪户核呼乎忽瑚壶葫胡蝴虎狐糊湖弧唬冱唿囫岵猢怙惚浒滹琥槲轷觳烀煳戽扈祜瓠鹄鹕鹱虍笏醐斛鹘"};
const char py_code_table_hua   []={"花华猾滑哗画划化话骅桦砉铧"};
const char py_code_table_huai  []={"坏槐徊怀淮踝"};
const char py_code_table_huan  []={"欢环桓还缓换患唤痪焕涣宦幻鬟浣豢郇奂垸萑擐圜獾洹漶寰逭缳锾鲩"};
const char py_code_table_huang []={"晃荒慌黄磺蝗簧皇凰惶煌幌恍谎隍徨湟潢遑璜肓癀蟥篁鳇"};
const char py_code_table_hui   []={"会回毁悔灰挥辉徽恢蛔慧卉惠晦贿秽烩汇讳诲绘彗溃诙茴荟蕙咴哕喙隳洄浍缋桧晖恚虺蟪麾"};
const char py_code_table_hun   []={"荤昏婚魂浑混诨馄阍溷珲"};
const char py_code_table_huo   []={"和豁活伙火获或惑霍货祸劐藿攉嚯夥钬锪镬耠蠖"};
const char py_code_table_ji    []={"计记给技击基几脊己圾籍集及急疾汲即嫉级挤蓟既忌际冀季伎祭剂机畸稽积肌饥迹激讥鸡姬绩缉吉极棘箕辑悸济寄寂妓继纪藉奇系丌亟乩剞佶偈墼芨芰荠萁蒺蕺掎叽咭哜唧岌嵴洎彐屐骥畿玑楫殛戟戢赍觊犄齑矶羁嵇稷瘠虮笈笄暨跻跽霁鲚鲫髻麂"};
const char py_code_table_jia   []={"家加嘉夹佳荚颊贾甲钾假稼价架驾嫁枷茄嘏伽郏葭岬浃迦珈戛胛恝铗铪镓痂瘕袷蛱笳袈跏"};
const char py_code_table_jian  []={"建见件简健减歼荐监捡俭剪坚尖间煎兼肩艰奸缄茧检柬碱硷拣笺槛鉴践贱键箭舰剑饯渐溅涧僭谏谫谮菅蒹搛囝湔蹇謇缣枧楗戋戬牮犍毽腱睑锏鹣裥笕翦趼踺鲣鞯"};
const char py_code_table_jiang []={"讲虹僵姜将浆江疆蒋桨奖匠酱降强茳洚绛缰犟礓耩糨豇"};
const char py_code_table_jiao  []={"叫较觉角校蕉椒礁焦胶交郊浇骄娇嚼搅铰矫侥脚狡饺缴绞剿教酵轿窖佼僬艽茭挢噍峤徼姣敫皎鹪蛟醮跤鲛"};
const char py_code_table_jie   []={"洁结解姐戒揭杰接皆界借介捷睫秸街阶截劫节桔竭藉芥疥诫届偈讦诘卩拮喈嗟婕孑桀碣锴疖颉蚧羯鲒骱"};
const char py_code_table_jin   []={"巾尽筋斤金今津紧锦仅谨进靳晋禁近烬浸劲襟卺荩堇噤馑廑妗缙瑾槿赆觐钅衿矜"};
const char py_code_table_jing  []={"京惊精经井睛晶景净颈静境敬警镜径竟竞劲荆兢茎鲸粳痉靖刭儆阱陉菁獍憬泾迳弪婧肼胫腈旌靓"};
const char py_code_table_jiong []={"炯窘冂迥扃"};
const char py_code_table_jiu   []={"九就救揪究纠玖韭久灸酒旧臼舅咎疚厩僦啾阄柩桕鸠鹫赳鬏"};
const char py_code_table_ju    []={"巨具距俱句惧车柜剧居驹菊局咀矩举沮聚拒据踞锯炬鞠拘狙疽倨讵苣苴莒掬遽屦琚枸椐榘榉橘犋飓钜锔窭裾趄醵踽龃雎瞿鞫"};
const char py_code_table_juan  []={"眷卷捐鹃娟倦绢圈鄄狷涓桊蠲锩镌隽"};
const char py_code_table_jue   []={"决诀绝觉角掘倔嚼脚撅攫抉爵厥劂谲矍堀蕨噘崛獗孓珏桷橛爝镢蹶觖"};
const char py_code_table_jun   []={"军君均菌钧峻俊竣浚郡骏龟捃皲筠麇"};
const char py_code_table_ka    []={"卡喀咖咯佧咔胩"};
const char py_code_table_kai   []={"开揩楷凯慨剀垲蒈忾恺铠锎锴"};
const char py_code_table_kan   []={"看刊堪勘坎砍侃龛瞰凵莰阚槛戡"};
const char py_code_table_kang  []={"康慷糠扛抗亢炕伉闶钪"};
const char py_code_table_kao   []={"考靠拷烤栲犒铐尻"};
const char py_code_table_ke    []={"可渴克科刻客课嗑坷苛柯棵磕颗壳咳恪蝌岢溘稞骒缂珂轲氪瞌钶铪呵锞疴窠颏蚵髁"};
const char py_code_table_ken   []={"肯啃垦恳裉"};
const char py_code_table_keng  []={"坑吭铿胫铒"};
const char py_code_table_kong  []={"空恐孔控倥崆箜"};
const char py_code_table_kou   []={"抠口扣寇芤蔻叩眍筘"};
const char py_code_table_ku    []={"枯哭窟苦酷库裤绔骷刳堀喾"};
const char py_code_table_kua   []={"夸垮挎跨胯侉锞"};
const char py_code_table_kuai  []={"会块筷侩快蒯郐哙狯脍"};
const char py_code_table_kuan  []={"宽款髋"};
const char py_code_table_kuang []={"况匡筐狂框矿眶旷诓诳邝圹夼哐纩贶"};
const char py_code_table_kui   []={"亏盔岿窥葵奎魁傀馈愧溃馗匮逵睽跬夔隗蒉揆喹喟悝愦暌聩蝰篑"};
const char py_code_table_kun   []={"坤昆捆困悃琨锟醌鲲阃髡"};
const char py_code_table_kuo   []={"括扩廓阔蛞"};
const char py_code_table_la    []={"垃拉喇蜡腊辣啦落剌邋旯砬瘌"};
const char py_code_table_lai   []={"莱来赖崃徕涞濑赉睐癞籁铼"};
const char py_code_table_lan   []={"蓝婪栏拦篮阑兰澜谰揽览懒缆烂滥岚漤榄斓镧褴罱"};
const char py_code_table_lang  []={"郎朗浪琅榔狼廊啷阆螂锒蒗稂莨"};
const char py_code_table_lao   []={"捞劳牢老佬姥酪烙涝落络唠崂醪栳铑铹痨耢"};
const char py_code_table_le    []={"勒乐了仂叻泐鳓"};
const char py_code_table_lei   []={"累勒雷镭蕾磊儡垒擂肋类泪羸诔嘞嫘缧檑耒酹"};
const char py_code_table_leng  []={"棱楞冷塄愣"};
const char py_code_table_li    []={"利力历厉立粒礼沥吏理李里厘励丽梨犁隶黎篱离漓鲤栗砾莉荔狸傈例俐痢璃哩鬲俪俚郦坜苈莅蓠藜呖唳喱猁溧澧逦娌嫠骊缡枥栎轹戾砺砬詈罹锂鹂疠疬蛎蜊蠡笠篥粝醴跞雳鲡鳢黧"};
const char py_code_table_lia   []={"俩"};
const char py_code_table_lian  []={"联莲连廉怜涟帘敛脸链恋炼练潋镰濂琏楝奁殓蔹臁裢裣蠊鲢"};
const char py_code_table_liang []={"两亮俩粮凉梁粱良辆量晾谅墚莨椋踉魉"};
const char py_code_table_liao  []={"了撩料聊僚疗燎寥辽潦撂镣廖蓼尥嘹獠寮缭钌鹩"};
const char py_code_table_lie   []={"列裂烈劣洌猎冽埒捩咧趔躐鬣"};
const char py_code_table_lin   []={"临邻赁拎琳林磷霖鳞淋凛吝蔺啉嶙廪懔遴檩辚膦瞵粼躏麟"};
const char py_code_table_ling  []={"令灵另棱玲菱零龄铃伶羚凌陵岭聆领酃苓呤囹泠绫柃棂瓴蛉翎鲮"};
const char py_code_table_liu   []={"刘六溜琉榴硫馏留瘤流柳碌陆浏遛骝绺旒熘锍镏鹨鎏"};
const char py_code_table_lo    []={"咯"};
const char py_code_table_long  []={"龙弄聋咙笼窿隆垄拢陇垅茏泷珑栊胧砻癃"};
const char py_code_table_lou   []={"楼娄搂篓漏陋露偻蒌喽嵝镂瘘耧蝼髅"};
const char py_code_table_lu    []={"六芦卢颅庐炉掳卤虏鲁麓碌露路赂鹿潞禄录陆戮绿垆撸噜泸渌漉逯璐栌橹轳辂辘贲氇胪镥鸬鹭簏舻鲈"};
const char py_code_table_lv    []={"滤率绿吕铝侣旅履屡缕虑氯律驴褛偻捋闾榈膂稆"};
const char py_code_table_luan  []={"乱孪峦挛滦卵"};
const char py_code_table_lue   []={"掠略锊"};
const char py_code_table_lun   []={"仑抡轮伦沦纶论囵"};
const char py_code_table_luo   []={"落罗铬咯烙萝螺逻锣箩骡裸洛骆络倮蠃荦捋摞猡泺漯珞椤脶硌镙瘰雒"};
const char py_code_table_m     []={"呒"};
const char py_code_table_ma    []={"妈麻玛码蚂马骂嘛吗摩抹唛犸嬷杩蟆"};
const char py_code_table_mai   []={"埋瞒馒蛮满蔓曼慢漫谩墁幔缦熳镘颟螨鳗鞔"};
const char py_code_table_man   []={"蛮馒瞒满曼谩慢漫蔓"};
const char py_code_table_mang  []={"芒茫盲氓忙莽邙漭硭蟒"};
const char py_code_table_mao   []={"猫茅锚毛矛髦卯茂冒帽貌贸袤铆茆峁泖瑁昴牦耄旄懋瞀蝥蟊"};
const char py_code_table_me    []={"么"};
const char py_code_table_mei   []={"每妹美玫枚昧寐袂魅梅酶霉煤没眉媒镁媚莓嵋猸浼湄楣镅鹛"};
const char py_code_table_men   []={"门闷们扪焖懑钔"};
const char py_code_table_meng  []={"萌梦蒙檬盟锰猛孟勐甍瞢懵朦礞虻蜢蠓艋艨"};
const char py_code_table_mi    []={"眯醚靡糜迷谜弥米秘觅泌蜜密麋谧幂芈冖蘼咪嘧猕汨宓弭纟脒祢敉糸縻"};
const char py_code_table_mian  []={"面棉眠绵冕免勉娩缅沔渑湎宀腼眄"};
const char py_code_table_miao  []={"苗描瞄藐秒渺庙妙喵邈缈缪杪淼眇鹋"};
const char py_code_table_mie   []={"蔑灭乜咩篾蠛"};
const char py_code_table_min   []={"民抿皿敏悯闽苠岷闵泯缗玟珉愍黾鳘"};
const char py_code_table_ming  []={"明螟鸣铭名命冥茗溟暝瞑酩"};
const char py_code_table_miu   []={"谬缪"};
const char py_code_table_mo    []={"脉没摸摹蘑模膜磨摩魔抹末莫墨默沫漠寞陌麽万谟茉蓦馍嫫殁镆秣瘼耱貊貘"};
const char py_code_table_mou   []={"谋牟某侔哞缪眸蛑鍪"};
const char py_code_table_mu    []={"木目睦模牟拇牡亩姆母墓暮幕募慕牧穆苜沐仫坶毪钼"};
const char py_code_table_na    []={"拿哪呐钠那娜纳捺肭镎衲"};
const char py_code_table_nai   []={"氖乃奶耐奈鼐佴艿萘柰"};
const char py_code_table_nan   []={"南男难喃囡楠腩蝻赧"};
const char py_code_table_nang  []={"囊囔馕攮曩"};
const char py_code_table_nao   []={"挠脑恼闹淖孬垴呶猱瑙硇铙蛲"};
const char py_code_table_ne    []={"哪呢讷"};
const char py_code_table_nei   []={"内馁"};
const char py_code_table_nen   []={"嫩恁"};
const char py_code_table_neng  []={"能"};
const char py_code_table_ng    []={"嗯"};
const char py_code_table_ni    []={"你呢拟尼妮霓倪泥匿腻逆溺伲坭猊怩昵旎祢慝睨铌鲵"};
const char py_code_table_nian  []={"蔫拈年碾撵捻念廿埝辇黏鲇鲶"};
const char py_code_table_niang []={"娘酿"};
const char py_code_table_niao  []={"鸟尿脲袅茑嬲"};
const char py_code_table_nie   []={"捏聂孽啮镊镍涅蹑蘖嗫乜陧颞臬"};
const char py_code_table_nin   []={"您"};
const char py_code_table_ning  []={"柠狞凝宁拧泞佞咛甯聍"};
const char py_code_table_niu   []={"牛扭钮纽拗妞狃忸"};
const char py_code_table_nong  []={"脓浓农弄侬哝"};
const char py_code_table_nou   []={"耨"};
const char py_code_table_nu    []={"奴努怒弩胬孥驽"};
const char py_code_table_nuan  []={"暖"};
const char py_code_table_nue   []={"疟虐"};
const char py_code_table_nuo   []={"娜挪懦糯诺傩搦喏锘"};
const char py_code_table_nv    []={"女恧钕衄"};
const char py_code_table_o     []={"哦喔噢"};
const char py_code_table_ou    []={"欧鸥殴藕呕偶沤区讴怄瓯耦"};
const char py_code_table_pa    []={"扒耙啪趴爬帕怕琶葩杷筢"};
const char py_code_table_pai   []={"拍排牌徘湃派迫俳蒎哌"};
const char py_code_table_pan   []={"番攀潘盘磐盼畔判叛蟠蹒胖拚丬爿泮贲袢襻"};
const char py_code_table_pang  []={"膀磅镑乓庞旁耪胖彷滂螃逄"};
const char py_code_table_pao   []={"抛咆刨炮袍跑泡狍庖疱脬匏"};
const char py_code_table_pei   []={"呸胚培裴赔陪配佩沛辔霈帔旆锫醅"};
const char py_code_table_pen   []={"喷盆湓"};
const char py_code_table_peng  []={"捧碰砰抨烹澎彭蓬棚硼篷膨朋鹏怦堋嘭蟛"};
const char py_code_table_pi    []={"辟否坯砒霹批披劈琵毗啤脾疲皮匹痞僻丕屁譬癖貔仳陂陴邳郫圮埤鼙芘擗噼庀淠媲纰枇甓睥罴铍裨疋蚍蜱"};
const char py_code_table_pian  []={"扁便篇偏片骗谝骈缏犏胼翩蹁"};
const char py_code_table_piao  []={"飘漂瓢票朴剽嫖缥瞟嘌骠殍螵"};
const char py_code_table_pie   []={"撇瞥丿苤氕"};
const char py_code_table_pin   []={"拼频贫品聘姘嫔榀牝颦"};
const char py_code_table_ping  []={"冯乒坪苹萍平凭瓶评屏俜娉枰鲆"};
const char py_code_table_po    []={"泊繁坡泼颇婆破魄迫粕朴叵陂鄱珀攴攵钋钷皤笸"};
const char py_code_table_pou   []={"剖裒掊"};
const char py_code_table_pu    []={"普堡暴脯扑铺仆莆葡菩蒲埔朴圃浦谱曝瀑匍噗溥蹼濮璞氆镤镨"};
const char py_code_table_qi    []={"七起妻乞企启稽缉期欺栖戚凄漆柒沏其棋奇歧畦崎脐齐旗祈岐祁骑岂琪琦杞契砌器气迄弃汽泣讫亟祺憩蹊鳍麒亓俟圻芑芪荠萋葺淇骐绮耆蕲桤槭欹嘁屺汔碛颀蛴蜞綦綮"};
const char py_code_table_qia   []={"卡掐恰洽葜袷髂"};
const char py_code_table_qian  []={"牵扦钎铅千迁签仟谦乾黔钱钳前潜遣浅谴堑嵌欠歉纤倩佥阡芊芡茜荨掮岍悭慊骞搴褰缱椠犍肷愆钤虔箝羟"};
const char py_code_table_qiang []={"枪呛腔羌墙蔷强抢跄戕戗炝锵镪丬襁嫱樯蜣羟"};
const char py_code_table_qiao  []={"壳橇锹敲悄桥瞧乔侨巧鞘撬翘峭俏窍雀诮谯荞峤憔樵跷鞒缲劁愀硗铫"};
const char py_code_table_qie   []={"切茄且怯窃伽惬慊妾趄郄挈锲箧"};
const char py_code_table_qin   []={"钦侵亲秦琴勤芹擒禽寝沁芩揿衾吣嗪噙廑檎锓矜覃螓"};
const char py_code_table_qing  []={"亲青轻氢倾卿清擎晴氰情顷请庆磬苘圊檠锖蜻罄箐綮謦鲭黥"};
const char py_code_table_qiong []={"琼穷穹蛩邛茕筇跫銎"};
const char py_code_table_qiu   []={"仇龟秋丘邱球求囚酋泅俅逑裘糗遒巯犰湫楸赇虬蚯蝤鳅"};
const char py_code_table_qu    []={"趋区蛆曲躯屈驱渠取娶龋趣去蛐黢戌诎劬凵苣蕖蘧岖衢阒璩觑氍朐祛磲鸲癯蠼麴瞿"};
const char py_code_table_quan  []={"圈颧权醛泉全痊拳犬券劝诠荃蜷鬈犭悛绻辁畎铨筌"};
const char py_code_table_que   []={"缺炔瘸却鹊榷确雀阕阙悫"};
const char py_code_table_qui   []={"鼽"};
const char py_code_table_qun   []={"裙群逡麇"};
const char py_code_table_ran   []={"然燃冉染苒蚺髯"};
const char py_code_table_rang  []={"瓤壤攘嚷让禳穰"};
const char py_code_table_rao   []={"饶扰绕荛娆桡"};
const char py_code_table_re    []={"惹热喏"};
const char py_code_table_ren   []={"壬仁人忍韧任认刃妊纫稔亻仞荏葚饪轫衽"};
const char py_code_table_reng  []={"扔仍"};
const char py_code_table_ri    []={"日"};
const char py_code_table_rong  []={"戎茸蓉荣融熔溶容绒冗嵘狨榕肜蝾"};
const char py_code_table_rou   []={"揉柔肉糅蹂鞣"};
const char py_code_table_ru    []={"茹蠕儒孺如辱乳汝入濡褥溽蓐嚅薷洳缛铷襦颥"};
const char py_code_table_ruan  []={"软阮朊"};
const char py_code_table_rui   []={"蕊瑞锐芮睿枘蕤蚋"};
const char py_code_table_run   []={"闰润"};
const char py_code_table_ruo   []={"若弱偌箬"};
const char py_code_table_sa    []={"撒洒萨卅仨飒脎"};
const char py_code_table_sai   []={"腮鳃塞赛噻"};
const char py_code_table_san   []={"三叁伞散馓毵糁"};
const char py_code_table_sang  []={"桑嗓丧搡磉颡"};
const char py_code_table_sao   []={"搔骚扫嫂臊瘙埽缫鳋"};
const char py_code_table_se    []={"塞瑟色涩啬铯穑"};
const char py_code_table_sen   []={"森"};
const char py_code_table_seng  []={"僧"};
const char py_code_table_sha   []={"莎砂杀刹沙纱傻啥煞杉厦痧裟霎鲨唼歃铩"};
const char py_code_table_shai  []={"色筛晒"};
const char py_code_table_shan  []={"掺单珊苫杉山删煽衫闪陕擅赡膳善汕扇缮栅讪疝舢蟮跚鳝剡鄯埏芟彡潸姗嬗骟膻禅钐髟"};
const char py_code_table_shang []={"墒伤商赏晌上尚裳绱殇觞垧熵"};
const char py_code_table_shao  []={"鞘梢捎稍烧芍勺韶少哨邵绍劭苕艄潲杓蛸筲"};
const char py_code_table_she   []={"奢赊蛇舌舍赦摄射慑涉社设麝折厍佘揲猞滠歙畲铊"};
const char py_code_table_shei  []={"谁"};
const char py_code_table_shen  []={"什参砷申呻伸身深娠绅神沈审婶甚肾慎渗诜谂莘葚哂渖椹胂矧蜃"};
const char py_code_table_sheng []={"乘声生甥牲升绳省盛剩胜圣笙嵊渑晟眚"};
const char py_code_table_shi   []={"是事匙师十石失狮施湿诗尸虱拾时什食蚀实识史矢使屎驶始式示士世柿拭誓逝势嗜噬适仕侍释饰氏市恃室视试似弑舐殖峙谥埘莳蓍饣轼贳炻礻铈铊螫筮酾豕鲥鲺"};
const char py_code_table_shou  []={"收手首守寿授售受瘦兽扌狩绶艏"};
const char py_code_table_shu   []={"蔬枢梳殊抒输叔舒淑疏书赎孰熟薯暑曙署蜀黍鼠属术述树束戍竖墅庶数漱恕倏塾沭澍姝丨菽摅纾毹腧殳镯秫疋"};
const char py_code_table_shua  []={"刷耍唰"};
const char py_code_table_shuai []={"率摔衰甩帅蟀"};
const char py_code_table_shuan []={"栓拴闩涮"};
const char py_code_table_shuang[]={"霜双爽泷孀"};
const char py_code_table_shui  []={"水睡税说氵"};
const char py_code_table_shun  []={"吮瞬顺舜"};
const char py_code_table_shuo  []={"数说硕朔烁蒴搠妁槊铄"};
const char py_code_table_si    []={"斯撕嘶思私司丝死肆寺嗣四伺似饲巳厮咝俟兕厶饣汜泗澌姒驷缌祀锶鸶耜蛳笥"};
const char py_code_table_song  []={"松耸怂颂送宋讼诵嵩忪悚淞竦崧菘凇"};
const char py_code_table_sou   []={"搜艘擞嗽叟嗖馊溲飕瞍锼螋嗾薮"};
const char py_code_table_su    []={"苏酥俗素速粟僳塑溯宿诉肃缩夙愫涑簌稣谡蔌嗉觫"};
const char py_code_table_suan  []={"酸蒜算狻"};
const char py_code_table_sui   []={"虽岁绥隋随髓祟遂碎隧穗"};
const char py_code_table_sun   []={"孙损笋"};
const char py_code_table_suo   []={"蓑梭唆缩琐索锁所唢嗦嗍娑莎桫挲睃羧"};
const char py_code_table_ta    []={"塌他它她塔獭挞沓蹋踏拓闼遢榻铊趿溻鳎漯"};
const char py_code_table_tai   []={"胎苔抬台泰酞太态汰跆邰薹骀肽炱钛鲐"};
const char py_code_table_tan   []={"弹坍摊贪瘫滩坛檀痰潭谭谈坦毯袒碳探叹炭覃郯澹昙赕忐钽锬镡"};
const char py_code_table_tang  []={"汤塘搪堂棠膛唐糖倘躺淌趟烫傥饧溏瑭樘铛螳铴镗耥螗羰醣帑"};
const char py_code_table_tao   []={"掏涛滔绦萄桃逃淘陶讨套叨啕饕洮韬焘鼗"};
const char py_code_table_te    []={"特忒忑铽"};
const char py_code_table_teng  []={"藤腾疼滕誊"};
const char py_code_table_ti    []={"梯剔踢锑提题蹄啼体替嚏惕涕剃屉倜悌缇荑逖绨鹈裼醍"};
const char py_code_table_tian  []={"天添填田甜恬舔腆殄掭忝阗畋钿锘"};
const char py_code_table_tiao  []={"调挑条迢眺跳佻髫苕祧铫窕蜩笤粜龆鲦"};
const char py_code_table_tie   []={"贴铁帖餮锇萜"};
const char py_code_table_ting  []={"厅听烃汀廷停亭庭挺艇莛葶婷铤蜓霆梃"};
const char py_code_table_tong  []={"桐瞳同铜彤童桶捅筒统痛佟僮茼恸潼酮仝垌嗵峒砼"};
const char py_code_table_tou   []={"偷投头透骰钭亠"};
const char py_code_table_tu    []={"凸秃突图徒途涂屠土吐兔堍荼菟钍酴"};
const char py_code_table_tuan  []={"湍团抟彖疃"};
const char py_code_table_tui   []={"推颓腿蜕褪退煺"};
const char py_code_table_tun   []={"囤褪吞屯臀饨豚氽暾"};
const char py_code_table_tuo   []={"说拖拓托脱鸵陀驮驼椭妥唾跎乇佗坨沱柝柁橐砣铊鼍箨酡庹"};
const char py_code_table_wa    []={"挖哇蛙洼娃瓦袜佤娲腽"};
const char py_code_table_wai   []={"歪外崴"};
const char py_code_table_wan   []={"蔓豌弯湾玩顽丸烷完碗挽晚皖惋宛婉万腕剜芄莞菀纨蜿绾琬脘畹鞔"};
const char py_code_table_wang  []={"汪王亡枉网往旺望忘妄罔惘魍尢辋"};
const char py_code_table_wei   []={"为位谓威巍微危韦维违桅围唯惟潍苇萎委伟伪尾纬未蔚味畏胃喂魏渭尉慰卫偎诿隗圩葳薇帏帷崴嵬猬逶娓痿炜玮闱猥隈沩洧涠韪軎煨艉鲔囗"};
const char py_code_table_wen   []={"问文闻纹瘟温蚊吻稳紊刎雯玟阌汶璺"};
const char py_code_table_weng  []={"嗡翁瓮蓊蕹"};
const char py_code_table_wo    []={"我握喔蜗涡窝斡卧沃挝倭莴幄龌渥肟硪"};
const char py_code_table_wu    []={"五物勿务午舞伍武侮恶悟误兀巫雾呜钨乌污诬屋无芜梧吾吴毋捂坞戊晤仵邬鹉圬芴唔庑怃忤鹜痦蜈浯寤迕妩婺骛阢杌牾於焐鋈鼯"};
const char py_code_table_xi    []={"希习夕戏细昔熙析西硒矽晰嘻吸锡牺稀蹊息悉膝惜熄溪汐犀袭席媳喜洗系隙羲皙屣嬉玺樨奚茜檄烯铣匚僖兮隰郗菥葸蓰唏徙饩阋浠淅曦觋欷歙熹禊禧穸裼蜥螅蟋舄舾粞翕醯鼷"};
const char py_code_table_xia   []={"瞎虾匣霞辖暇峡侠狭下厦夏吓呷狎遐瑕柙硖罅黠"};
const char py_code_table_xian  []={"现先仙嫌显险献县贤铣洗掀锨鲜纤咸衔舷闲涎弦腺馅羡宪陷限线冼苋莶藓岘猃暹娴氙燹祆鹇痃痫蚬筅籼酰跣跹霰"};
const char py_code_table_xiang []={"像向象降相厢镶香箱湘乡翔祥详想响享项巷橡襄飨芗葙饷庠骧缃蟓鲞"};
const char py_code_table_xiao  []={"小消肖萧硝霄削哮嚣销宵淆晓孝校啸笑效哓潇逍筱箫骁绡枭枵蛸魈"};
const char py_code_table_xie   []={"些歇蝎鞋协挟携邪斜胁谐写械卸蟹懈泄泻谢屑血解楔叶偕亵勰燮薤撷獬廨渫瀣邂绁缬榭榍颉躞鲑骱"};
const char py_code_table_xin   []={"薪芯锌欣辛新忻心信衅馨莘歆鑫昕囟忄镡"};
const char py_code_table_xing  []={"幸省星腥猩惺兴刑型形邢行醒杏性姓饧陉荇荥擤悻硎"};
const char py_code_table_xiong []={"兄凶胸匈汹雄熊芎"};
const char py_code_table_xiu   []={"臭宿休修羞朽嗅锈秀袖绣咻溴貅馐岫庥鸺髹"};
const char py_code_table_xu    []={"续绪墟戌需虚嘘须徐许蓄酗叙旭序畜恤絮婿诩煦勖圩蓿洫浒溆顼栩吁胥盱糈醑"};
const char py_code_table_xuan  []={"玄选轩喧宣渲悬旋暄绚炫眩癣儇谖萱泫漩璇楦煊揎碹铉镟券"};
const char py_code_table_xue   []={"削靴薛学穴雪血鳕谑噱泶踅"};
const char py_code_table_xun   []={"寻旬迅讯询训勋熏循驯巡殉汛逊郇埙荀薰峋徇醺鲟浚荨蕈獯巽恂洵浔曛窨"};
const char py_code_table_ya    []={"牙芽亚哑压雅押鸦鸭呀丫崖衙涯讶轧伢蚜垭揠岈迓娅琊桠氩砑睚痖疋"};
const char py_code_table_yan   []={"言演验厌严沿炎燕咽阉奄掩眼烟淹盐研岩延颜阎衍艳堰砚雁唁焉彦焰宴谚蜒闫妍嫣胭腌焱恹俨偃铅殷厣赝剡兖讠谳阽郾鄢芫菸崦魇阏湮滟琰檐晏罨筵酽餍鼹鼽"};
const char py_code_table_yang  []={"央仰羊洋阳养样杨扬恙殃鸯秧佯氧疡痒漾徉怏泱炀烊蛘鞅"};
const char py_code_table_yao   []={"要邀咬药腰妖摇尧肴窈遥窑谣姚舀耀钥夭爻幺吆瑶啮疟侥崤崾徭珧杳轺曜铫鹞繇鳐"};
const char py_code_table_ye    []={"也页业叶夜液椰噎耶爷野冶掖曳腋邪咽谒盅靥邺揶琊晔烨铘"};
const char py_code_table_yi    []={"一乙已以忆义议谊译异益壹医揖铱依伊衣颐夷遗移仪胰疑沂宜姨彝椅蚁倚矣艺抑易邑屹亿役臆逸肄疫亦裔意毅溢诣翼翌艾尾绎刈劓佚佾诒阝圯埸懿苡荑薏弈奕挹弋呓咦咿噫峄嶷猗饴怿怡悒漪迤驿缢殪轶贻欹旖熠眙钇铊镒镱痍瘗癔翊衤蜴舣羿翳酏黟"};
const char py_code_table_yin   []={"尹引印茵荫因殷音阴姻吟银淫寅饮隐龈胤蚓鄞廴垠堙茚吲喑狺夤洇湮氤铟瘾窨霪"};
const char py_code_table_ying  []={"映影英莹萤营荧樱婴鹰应缨蝇迎赢盈颖硬嬴莺萦鹦潆瀛滢瑛郢茔荥蓥撄嘤膺璎楹媵瘿颍罂"};
const char py_code_table_yo    []={"哟育唷"};
const char py_code_table_yong  []={"用拥佣勇涌臃痈庸雍踊蛹咏泳永恿俑壅墉喁慵邕镛甬鳙饔"};
const char py_code_table_you   []={"尤有又诱幼友右佑幽优悠忧由邮铀犹油游酉釉呦疣鱿黝鼬蚰攸卣侑莠莜莸尢囿宥柚猷牖铕蚴蝣蝤繇"};
const char py_code_table_yu    []={"余与于予宇玉雨育誉浴鱼迂淤盂虞愚舆俞逾愉渝渔隅娱屿禹语羽域芋郁吁遇喻峪御愈榆欲狱寓裕预豫驭竽毓谀於谕蔚尉禺伛俣萸菀蓣揄圄圉嵛狳饫馀庾阈鬻妪妤纡瑜昱觎腴欤煜熨燠肀聿钰鹆鹬瘐瘀窬窳蜮蝓臾舁雩龉"};
const char py_code_table_yuan  []={"原元园员圆源缘远愿怨院鸳渊冤垣袁援辕猿苑媛塬芫掾圜沅瑗橼爰眢鸢螈箢鼋"};
const char py_code_table_yue   []={"月悦阅乐说曰约越跃钥岳粤龠哕瀹栎樾刖钺"};
const char py_code_table_yun   []={"云员允运晕耘匀陨蕴酝熨韵孕氲殒芸郧郓狁恽愠纭韫昀筠"};
const char py_code_table_za    []={"匝砸杂扎咋咂拶"};
const char py_code_table_zai   []={"再在栽哉灾宰载崽甾"};
const char py_code_table_zan   []={"咱攒暂赞昝簪拶瓒糌趱錾"};
const char py_code_table_zang  []={"藏赃脏葬驵臧"};
const char py_code_table_zao   []={"枣遭早造皂灶糟凿澡躁噪燥藻蚤唣"};
const char py_code_table_ze    []={"责择则泽咋仄赜啧帻迮昃笮箦舴"};
const char py_code_table_zei   []={"贼"};
const char py_code_table_zen   []={"怎谮"};
const char py_code_table_zeng  []={"增憎曾赠缯甑罾锃"};
const char py_code_table_zha   []={"扎闸查喳渣札轧铡眨栅榨咋乍炸诈吒咤柞揸蚱哳喋楂砟痄龃齄"};
const char py_code_table_zhai  []={"宅窄债翟择摘斋寨砦瘵"};
const char py_code_table_zhan  []={"占战站展沾绽瞻毡詹粘盏斩辗崭蘸颤栈湛谵搌骣旃"};
const char py_code_table_zhang []={"张丈仗胀杖樟章彰漳掌涨帐账长瘴障幛嶂獐蟑仉鄣嫜璋"};
const char py_code_table_zhao  []={"找召爪着兆朝招昭诏沼赵照罩肇钊笊啁棹"};
const char py_code_table_zhe   []={"遮折哲蛰辙者锗蔗这浙着谪摺柘辄磔鹧褶蜇赭"};
const char py_code_table_zhen  []={"真贞针震振诊阵镇侦枕珍疹斟甄箴臻帧圳砧蓁浈溱缜桢椹榛轸赈胗朕祯畛稹鸩"};
const char py_code_table_zheng []={"正证蒸挣睁征狰争怔整拯政症郑诤峥徵钲铮筝鲭"};
const char py_code_table_zhi   []={"只职识执值直植殖治支纸志址指质置制止至侄芝之汁旨枝知肢脂织趾挚掷致帜峙智秩稚吱蜘炙痔滞窒枳芷卮陟郅埴摭帙徵夂忮彘咫骘栉栀桎轵轾贽胝膣祉祗黹雉鸷痣蛭絷酯跖踬踯豸觯"};
const char py_code_table_zhong []={"中众忠钟衷终种肿重仲盅冢忪锺螽舯踵"};
const char py_code_table_zhou  []={"舟周州洲诌粥轴肘帚咒皱宙昼骤荮啁妯纣绉胄碡籀繇酎"};
const char py_code_table_zhu   []={"主住注祝竹珠株蛛朱猪诸诛逐烛煮拄瞩嘱著柱助蛀贮铸筑驻伫竺箸侏属术邾苎茱洙渚潴杼槠橥炷铢疰瘃褚舳翥躅麈丶"};
const char py_code_table_zhua  []={"抓爪挝"};
const char py_code_table_zhuai []={"拽转"};
const char py_code_table_zhuan []={"传专砖转撰赚篆啭馔沌颛"};
const char py_code_table_zhuang[]={"幢桩庄装妆撞壮状奘戆"};
const char py_code_table_zhui  []={"椎锥追赘坠缀惴骓缒隹"};
const char py_code_table_zhun  []={"谆准饨肫窀"};
const char py_code_table_zhuo  []={"着捉拙卓桌灼茁浊酌琢啄倬诼擢浞涿濯焯禚斫镯"};
const char py_code_table_zi    []={"自子字仔兹咨资姿滋淄孜紫籽滓渍龇姊吱嵫谘茈孳缁梓辎赀恣眦锱秭耔笫粢趑觜訾鲻髭"};
const char py_code_table_zong  []={"宗综总鬃棕踪纵粽偬枞腙"};
const char py_code_table_zou   []={"走奏揍邹诹陬鄹驺鲰"};
const char py_code_table_zu    []={"足组卒族租祖诅阻俎菹镞"};
const char py_code_table_zuan  []={"钻攥纂缵躜"};
const char py_code_table_zui   []={"最罪嘴醉蕞觜"};
const char py_code_table_zun   []={"尊遵樽鳟撙"};
const char py_code_table_zuo   []={"左佐做作坐座昨撮唑柞阼琢嘬怍胙祚砟酢"};

#define PY_CODE_TABLE_NUM 398

typedef struct {
    char *py;
    char *py_code;
} py_index;

const py_index py_code_table[]=
{
//    "a",
//    "ai",
//    "an",
//    "ang",
//    "ao",
//    "e",
//    "ei",
//    "en",
//    "eng",
//    "i",
//    "ian",
//    "iao",
//    "ie",
//    "in",
//    "ing",
//    "o",
//    "ong",
//    "ou",
//    "u",
//    "uai",
//    "uan",
//    "uang",
//    "ui",
//    "un",
//    "uo",

//    "b",
//    "c",
//    "d",
//    "f",
//    "g",
//    "h",
//    "j",
//    "k",
//    "l",
//    "m",
//    "n",
//    "p",
//    "q",
//    "r",
//    "s",
//    "t",
//    "w",
//    "x",
//    "y",
//    "z",

{"a",   &py_code_table_a},
{"e",   &py_code_table_e},
{"o",   &py_code_table_o},
{"ai",  &py_code_table_ai},
{"an",  &py_code_table_an},
{"ao",  &py_code_table_ao},
{"ba",  &py_code_table_ba},
{"bi",  &py_code_table_bi},
{"bo",  &py_code_table_bo},
{"bu",  &py_code_table_bu},
{"ca",  &py_code_table_ca},
{"ce",  &py_code_table_ce},
{"ci",  &py_code_table_ci},
{"cu",  &py_code_table_cu},
{"da",  &py_code_table_da},
{"de",  &py_code_table_de},
{"di",  &py_code_table_di},
{"du",  &py_code_table_du},
{"en",  &py_code_table_en},
{"er",  &py_code_table_er},
{"fa",  &py_code_table_fa},
{"fo",  &py_code_table_fo},
{"fu",  &py_code_table_fu},
{"ha",  &py_code_table_ha},
{"ga",  &py_code_table_ga},
{"ge",  &py_code_table_ge},
{"he",  &py_code_table_he},
{"gu",  &py_code_table_gu},
{"hu",  &py_code_table_hu},
{"ji",  &py_code_table_ji},
{"ju",  &py_code_table_ju},
{"ka",  &py_code_table_ka},
{"ke",  &py_code_table_ke},
{"ku",  &py_code_table_ku},
{"la",  &py_code_table_la},
{"le",  &py_code_table_le},
{"li",  &py_code_table_li},
{"lu",  &py_code_table_lu},
{"lv",  &py_code_table_lv},
{"ma",  &py_code_table_ma},
{"me",  &py_code_table_me},
{"mi",  &py_code_table_mi},
{"mo",  &py_code_table_mo},
{"mu",  &py_code_table_mu},
{"na",  &py_code_table_na},
{"ne",  &py_code_table_ne},
{"ni",  &py_code_table_ni},
{"nu",  &py_code_table_nu},
{"nv",  &py_code_table_nv},
{"ou",  &py_code_table_ou},
{"pa",  &py_code_table_pa},
{"pi",  &py_code_table_pi},
{"po",  &py_code_table_po},
{"pu",  &py_code_table_pu},
{"qi",  &py_code_table_qi},
{"qu",  &py_code_table_qu},
{"re",  &py_code_table_re},
{"ri",  &py_code_table_ri},
{"ru",  &py_code_table_ru},
{"sa",  &py_code_table_sa},
{"se",  &py_code_table_se},
{"si",  &py_code_table_si},
{"su",  &py_code_table_su},
{"ta",  &py_code_table_ta},
{"te",  &py_code_table_te},
{"ti",  &py_code_table_ti},
{"tu",  &py_code_table_tu},
{"wa",  &py_code_table_wa},
{"wo",  &py_code_table_wo},
{"wu",  &py_code_table_wu},
{"xi",  &py_code_table_xi},
{"xu",  &py_code_table_xu},
{"ya",  &py_code_table_ya},
{"ye",  &py_code_table_ye},
{"yi",  &py_code_table_yi},
{"yo",  &py_code_table_yo},
{"yu",  &py_code_table_yu},
{"za",  &py_code_table_za},
{"ze",  &py_code_table_ze},
{"zi",  &py_code_table_zi},
{"zu",  &py_code_table_zu},
{"ang", &py_code_table_ang},
{"bai", &py_code_table_bai},
{"ban", &py_code_table_ban},
{"bao", &py_code_table_bao},
{"bei", &py_code_table_bei},
{"ben", &py_code_table_ben},
{"bie", &py_code_table_bie},
{"bin", &py_code_table_bin},
{"cai", &py_code_table_cai},
{"can", &py_code_table_can},
{"cao", &py_code_table_cao},
{"cha", &py_code_table_cha},
{"che", &py_code_table_che},
{"chi", &py_code_table_chi},
{"chu", &py_code_table_chu},
{"cou", &py_code_table_cou},
{"cui", &py_code_table_cui},
{"cun", &py_code_table_cun},
{"cuo", &py_code_table_cuo},
{"dai", &py_code_table_dai},
{"dan", &py_code_table_dan},
{"dao", &py_code_table_dao},
{"dei", &py_code_table_dei},
{"die", &py_code_table_die},
{"diu", &py_code_table_diu},
{"dou", &py_code_table_dou},
{"dui", &py_code_table_dui},
{"dun", &py_code_table_dun},
{"duo", &py_code_table_duo},
{"fan", &py_code_table_fan},
{"fei", &py_code_table_fei},
{"fen", &py_code_table_fen},
{"fou", &py_code_table_fou},
{"gai", &py_code_table_gai},
{"gan", &py_code_table_gan},
{"gao", &py_code_table_gao},
{"gei", &py_code_table_gei},
{"gen", &py_code_table_gan},
{"gou", &py_code_table_gou},
{"gua", &py_code_table_gua},
{"gui", &py_code_table_gui},
{"gun", &py_code_table_gun},
{"guo", &py_code_table_guo},
{"hai", &py_code_table_hai},
{"han", &py_code_table_han},
{"hao", &py_code_table_hao},
{"hei", &py_code_table_hei},
{"hen", &py_code_table_hen},
{"hou", &py_code_table_hou},
{"hua", &py_code_table_hua},
{"hui", &py_code_table_hui},
{"hun", &py_code_table_hun},
{"huo", &py_code_table_huo},
{"jia", &py_code_table_jia},
{"jie", &py_code_table_jie},
{"jin", &py_code_table_jin},
{"jiu", &py_code_table_jiu},
{"jue", &py_code_table_jue},
{"jun", &py_code_table_jun},
{"kai", &py_code_table_kai},
{"kan", &py_code_table_kan},
{"kao", &py_code_table_kao},
{"ken", &py_code_table_ken},
{"kou", &py_code_table_kou},
{"kua", &py_code_table_kua},
{"kui", &py_code_table_kui},
{"kun", &py_code_table_kun},
{"kuo", &py_code_table_kuo},
{"lai", &py_code_table_lai},
{"lan", &py_code_table_lan},
{"lao", &py_code_table_lao},
{"lei", &py_code_table_lei},
{"lie", &py_code_table_lie},
{"lin", &py_code_table_lin},
{"liu", &py_code_table_liu},
{"lou", &py_code_table_lou},
{"lue", &py_code_table_lue},
{"lun", &py_code_table_lun},
{"luo", &py_code_table_luo},
{"mai", &py_code_table_mai},
{"man", &py_code_table_man},
{"mao", &py_code_table_mao},
{"mei", &py_code_table_mei},
{"men", &py_code_table_men},
{"mie", &py_code_table_mie},
{"min", &py_code_table_min},
{"miu", &py_code_table_miu},
{"mou", &py_code_table_mou},
{"nai", &py_code_table_nai},
{"nan", &py_code_table_nan},
{"nao", &py_code_table_nao},
{"nei", &py_code_table_nei},
{"nen", &py_code_table_nen},
{"nie", &py_code_table_nie},
{"nin", &py_code_table_nin},
{"niu", &py_code_table_niu},
{"nue", &py_code_table_nue},
{"nuo", &py_code_table_nuo},
{"pai", &py_code_table_pai},
{"pan", &py_code_table_pan},
{"pao", &py_code_table_pao},
{"pei", &py_code_table_pei},
{"pen", &py_code_table_pen},
{"pie", &py_code_table_pie},
{"pin", &py_code_table_pin},
{"pou", &py_code_table_pou},
{"qia", &py_code_table_qia},
{"qie", &py_code_table_qie},
{"qin", &py_code_table_qin},
{"qiu", &py_code_table_qiu},
{"que", &py_code_table_que},
{"qun", &py_code_table_qun},
{"ran", &py_code_table_ran},
{"rao", &py_code_table_rao},
{"ren", &py_code_table_ren},
{"rou", &py_code_table_rou},
{"rui", &py_code_table_rui},
{"run", &py_code_table_run},
{"ruo", &py_code_table_ruo},
{"sai", &py_code_table_sai},
{"sao", &py_code_table_sao},
{"san", &py_code_table_san},
{"sen", &py_code_table_sen},
{"sha", &py_code_table_sha},
{"she", &py_code_table_she},
{"shi", &py_code_table_shi},
{"shu", &py_code_table_shu},
{"sou", &py_code_table_sou},
{"sui", &py_code_table_sui},
{"sun", &py_code_table_sun},
{"suo", &py_code_table_suo},
{"tai", &py_code_table_tai},
{"tan", &py_code_table_tan},
{"tao", &py_code_table_tao},
{"tie", &py_code_table_tie},
{"tou", &py_code_table_tou},
{"tui", &py_code_table_tui},
{"tun", &py_code_table_tun},
{"tuo", &py_code_table_tuo},
{"wai", &py_code_table_wai},
{"wan", &py_code_table_wan},
{"wei", &py_code_table_wei},
{"wen", &py_code_table_wen},
{"xia", &py_code_table_xia},
{"xie", &py_code_table_xie},
{"xin", &py_code_table_xin},
{"xiu", &py_code_table_xiu},
{"xue", &py_code_table_xue},
{"xun", &py_code_table_xun},
{"yan", &py_code_table_yan},
{"yao", &py_code_table_yao},
{"yin", &py_code_table_yin},
{"you", &py_code_table_you},
{"yue", &py_code_table_yue},
{"yun", &py_code_table_yun},
{"zai", &py_code_table_zai},
{"zan", &py_code_table_zan},
{"zao", &py_code_table_zao},
{"zei", &py_code_table_zei},
{"zen", &py_code_table_zen},
{"zha", &py_code_table_zha},
{"zhe", &py_code_table_zhe},
{"zhi", &py_code_table_zhi},
{"zhu", &py_code_table_zhu},
{"zou", &py_code_table_zou},
{"zui", &py_code_table_zui},
{"zun", &py_code_table_zun},
{"zuo", &py_code_table_zuo},
{"bang", &py_code_table_bang},
{"beng", &py_code_table_beng},
{"bian", &py_code_table_bian},
{"biao", &py_code_table_biao},
{"bing", &py_code_table_bing},
{"cang", &py_code_table_cang},
{"ceng", &py_code_table_ceng},
{"chai", &py_code_table_chai},
{"chan", &py_code_table_chan},
{"chao", &py_code_table_chao},
{"chen", &py_code_table_chen},
{"chou", &py_code_table_chou},
{"chuai",&py_code_table_chuai},
{"chui", &py_code_table_chui},
{"chun", &py_code_table_chun},
{"chuo", &py_code_table_chuo},
{"cong", &py_code_table_cong},
{"cuan", &py_code_table_cuan},
{"dang", &py_code_table_dang},
{"deng", &py_code_table_deng},
{"dian", &py_code_table_dian},
{"diao", &py_code_table_diao},
{"ding", &py_code_table_ding},
{"dong", &py_code_table_dong},
{"duan", &py_code_table_duan},
{"fang", &py_code_table_fang},
{"feng", &py_code_table_feng},
{"gang", &py_code_table_gang},
{"geng", &py_code_table_geng},
{"gong", &py_code_table_gong},
{"guai", &py_code_table_guai},
{"guan", &py_code_table_guan},
{"hang", &py_code_table_hang},
{"heng", &py_code_table_heng},
{"hong", &py_code_table_hong},
{"huai", &py_code_table_huai},
{"huan", &py_code_table_huan},
{"jian", &py_code_table_jian},
{"jiao", &py_code_table_jiao},
{"jing", &py_code_table_jing},
{"juan", &py_code_table_juan},
{"kang", &py_code_table_kang},
{"keng", &py_code_table_keng},
{"kong", &py_code_table_kong},
{"kuai", &py_code_table_kuai},
{"kuan", &py_code_table_kuan},
{"lang", &py_code_table_lang},
{"leng", &py_code_table_leng},
{"lian", &py_code_table_lian},
{"liao", &py_code_table_liao},
{"ling", &py_code_table_ling},
{"long", &py_code_table_long},
{"luan", &py_code_table_luan},
{"mang", &py_code_table_mang},
{"meng", &py_code_table_meng},
{"mian", &py_code_table_mian},
{"miao", &py_code_table_miao},
{"ming", &py_code_table_ming},
{"nang", &py_code_table_nang},
{"neng", &py_code_table_neng},
{"nian", &py_code_table_nian},
{"niao", &py_code_table_niao},
{"ning", &py_code_table_ning},
{"nong", &py_code_table_nong},
{"nuan", &py_code_table_nuan},
{"pang", &py_code_table_pang},
{"peng", &py_code_table_peng},
{"pian", &py_code_table_pian},
{"piao", &py_code_table_piao},
{"ping", &py_code_table_ping},
{"qian", &py_code_table_qian},
{"qiao", &py_code_table_qiao},
{"qing", &py_code_table_qing},
{"quan", &py_code_table_quan},
{"rang", &py_code_table_rang},
{"reng", &py_code_table_reng},
{"rong", &py_code_table_rong},
{"ruan", &py_code_table_ruan},
{"sang", &py_code_table_sang},
{"seng", &py_code_table_seng},
{"shai", &py_code_table_shai},
{"shan", &py_code_table_shan},
{"shao", &py_code_table_shao},
{"shei", &py_code_table_shei},
{"shen", &py_code_table_shen},
{"shou", &py_code_table_shou},
{"shua", &py_code_table_shua},
{"shui", &py_code_table_shui},
{"shun", &py_code_table_shun},
{"shuo", &py_code_table_shuo},
{"song", &py_code_table_song},
{"suan", &py_code_table_suan},
{"tang", &py_code_table_tang},
{"teng", &py_code_table_teng},
{"tian", &py_code_table_tian},
{"tiao", &py_code_table_tiao},
{"ting", &py_code_table_ting},
{"tong", &py_code_table_tong},
{"tuan", &py_code_table_tuan},
{"wang", &py_code_table_wang},
{"weng", &py_code_table_weng},
{"xian", &py_code_table_xian},
{"xiao", &py_code_table_xiao},
{"xing", &py_code_table_xing},
{"xuan", &py_code_table_xuan},
{"yang", &py_code_table_yang},
{"ying", &py_code_table_ying},
{"yong", &py_code_table_yong},
{"yuan", &py_code_table_yuan},
{"zang", &py_code_table_zang},
{"zeng", &py_code_table_zeng},
{"zhai", &py_code_table_zhai},
{"zhan", &py_code_table_zhan},
{"zhao", &py_code_table_zhao},
{"zhen", &py_code_table_zhen},
{"zhou", &py_code_table_zhou},
{"zhua", &py_code_table_zhua},
{"zhui", &py_code_table_zhui},
{"zhun", &py_code_table_zhun},
{"zhuo", &py_code_table_zhuo},
{"zong", &py_code_table_zong},
{"zuan", &py_code_table_zuan},
{"chang",&py_code_table_chang},
{"cheng",&py_code_table_cheng},
{"chong",&py_code_table_chong},
{"chuan",&py_code_table_chuan},
{"guang",&py_code_table_guang},
{"huang",&py_code_table_huang},
{"jiang",&py_code_table_jiang},
{"jiong",&py_code_table_jiong},
{"kuang",&py_code_table_kuang},
{"liang",&py_code_table_liang},
{"niang",&py_code_table_niang},
{"qiang",&py_code_table_qiang},
{"qiong",&py_code_table_qiong},
{"shang",&py_code_table_shang},
{"sheng",&py_code_table_sheng},
{"shuai",&py_code_table_shuai},
{"shuan",&py_code_table_shuan},
{"xiang",&py_code_table_xiang},
{"xiong",&py_code_table_xiong},
{"zhang",&py_code_table_zhang},
{"zheng",&py_code_table_zheng},
{"zhong",&py_code_table_zhong},
{"zhuai",&py_code_table_zhuai},
{"zhuan",&py_code_table_zhuan},
{"chuang",&py_code_table_chuang},
{"shuang",&py_code_table_shuang},
{"zhuang",&py_code_table_zhuang},
};

py_index *py_code_res[10];

uint8_t str_match(char * str1,char * str2)
{
    uint8_t i=0;
    while(1)
    {
        if(*str1 != *str2)
            break;
        if(*str1 == '\0')
        {
            i=0XFF;
            break;
        }
        i++;
        str1++;
        str2++;
    }
    return i;
}

uint8_t get_matched_py_code(char *strin,py_index **matchlist)
{
    py_index *bestmatch=0;
    uint16_t pyindex_len=0;
    uint16_t i=0;
    uint8_t temp,mcnt=0,bmcnt=0;
    bestmatch=(py_index*)&py_code_table[0];
    pyindex_len=sizeof(py_code_table)/sizeof(py_code_table[0]);
    for(i=0;i<pyindex_len;i++)
    {
        temp=str_match(strin,(char *)py_code_table[i].py);
        if(temp)
        {
            if(temp==0XFF)matchlist[mcnt++]=(py_index*)&py_code_table[i];
            else if(temp>bmcnt)
            {
                bmcnt=temp;
                bestmatch=(py_index*)&py_code_table[i];
            }
        }
    }
    if(mcnt==0&&bmcnt)
    {
        matchlist[0]=bestmatch;
        mcnt=bmcnt|0X80;
    }
    return mcnt;
}

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_keyboard_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("keyboard create started");

    /*Create the ancestor of keyboard*/
    lv_obj_t * kb = lv_obj_create(par, copy);
    LV_ASSERT_MEM(kb);
    if(kb == NULL) return NULL;

    lv_obj_set_style_local_border_width(kb, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_radius(kb, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(kb);

    /*Allocate the keyboard type specific extended data*/
    lv_keyboard_ext_t * ext = lv_obj_allocate_ext_attr(kb, sizeof(lv_keyboard_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(kb);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->ta         = NULL;
    ext->mode       = LV_KEYBOARD_MODE_TEXT_LOWER;
    ext->cursor_mng = 0;
    ext->py_code_p = 0;
    ext->py_code[0] = '\0';

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(kb, lv_keyboard_signal);

    /*Init the new keyboard keyboard*/
    if(copy == NULL) {
        /* Set a size which fits into the parent.
         * Don't use `par` directly because if the window is created on a page it is moved to the
         * scrollable so the parent has changed */
        if(LV_VER_RES_MAX > LV_HOR_RES_MAX)
        {
            lv_obj_set_size(kb, lv_obj_get_width_fit(lv_obj_get_parent(kb)),
                        lv_obj_get_width_fit(lv_obj_get_parent(kb)) * 2 / 3);
        }
        else
        {
            lv_obj_set_size(kb, lv_obj_get_width_fit(lv_obj_get_parent(kb)) * 2 / 3,
                        lv_obj_get_height_fit(lv_obj_get_parent(kb)) / 2);
        }
        lv_obj_align(kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

        ext->btnm = lv_btnmatrix_create(kb, NULL);
        lv_btnmatrix_set_map(ext->btnm, kb_map[ext->mode]);
        lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[ext->mode]);
        lv_obj_set_size(ext->btnm, lv_obj_get_width(kb),
                        lv_obj_get_height(kb) * 5 / 6);
        lv_obj_align(ext->btnm, kb, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
        lv_obj_set_event_cb(ext->btnm, lv_keyboard_def_event_cb);
        lv_obj_set_base_dir(ext->btnm, LV_BIDI_DIR_LTR);
        lv_obj_add_protect(ext->btnm, LV_PROTECT_CLICK_FOCUS);

        ext->match_word = lv_label_create(kb, NULL);
        lv_label_set_text(ext->match_word,"");
        lv_obj_align(ext->match_word, kb, LV_ALIGN_IN_TOP_LEFT, 5, 2);

        ext->match_word_btn = lv_cont_create(kb, NULL);
        //lv_obj_set_size(ext->match_word_btn,lv_obj_get_width(kb) - 20,38);

        lv_obj_set_style_local_border_width(ext->match_word_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_radius(ext->match_word_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_style_local_bg_opa(ext->match_word_btn, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_obj_set_auto_realign(ext->match_word_btn, false);
        lv_cont_set_fit(ext->match_word_btn, LV_FIT_TIGHT);
        lv_cont_set_layout(ext->match_word_btn, LV_LAYOUT_ROW_MID);
        lv_obj_set_drag(ext->match_word_btn,true);
        lv_obj_set_drag_dir(ext->match_word_btn,LV_DRAG_DIR_HOR);
        lv_obj_align(ext->match_word_btn, ext->match_word, LV_ALIGN_OUT_BOTTOM_LEFT, 0, -10);

        lv_theme_apply(ext->btnm, LV_THEME_KEYBOARD);
        lv_obj_set_style_local_text_font(ext->btnm, LV_KEYBOARD_PART_BTN,LV_STATE_DEFAULT,&lv_font_montserrat_16);
    }
    /*Copy an existing keyboard*/
    else {
        lv_keyboard_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->ta                = copy_ext->ta;
        ext->mode              = copy_ext->mode;
        ext->cursor_mng        = copy_ext->cursor_mng;

        lv_btnmatrix_set_map(ext->btnm, kb_map[ext->mode]);
        lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[ext->mode]);

        /*Refresh the style with new signal function*/
        //        lv_obj_refresh_style(new_kb);
    }

    LV_LOG_INFO("keyboard created");

    return kb;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_keyboard_set_textarea(lv_obj_t * kb, lv_obj_t * ta)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);
    if(ta) {
        LV_ASSERT_OBJ(ta, "lv_textarea");
    }

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);

    /*Hide the cursor of the old Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        lv_textarea_set_cursor_hidden(ext->ta, true);
    }

    ext->ta = ta;

    /*Show the cursor of the new Text area if cursor management is enabled*/
    if(ext->ta && ext->cursor_mng) {
        lv_textarea_set_cursor_hidden(ext->ta, false);
    }
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_keyboard_mode_t'
 */
void lv_keyboard_set_mode(lv_obj_t * kb, lv_keyboard_mode_t mode)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->mode == mode) return;

    ext->mode = mode;
    lv_btnmatrix_set_map(ext->btnm, kb_map[mode]);
    lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[mode]);
}

/**
 * Automatically hide or show the cursor of Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_keyboard_set_cursor_manage(lv_obj_t * kb, bool en)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->cursor_mng == en) return;

    ext->cursor_mng = en == false ? 0 : 1;

    if(ext->ta) {

        if(ext->cursor_mng) {
            lv_textarea_set_cursor_hidden(ext->ta, false);
        }
        else {
            lv_textarea_set_cursor_hidden(ext->ta, true);
        }
    }
}

/**
 * Set a new map for the keyboard
 * @param kb pointer to a Keyboard object
 * @param mode keyboard map to alter 'lv_keyboard_mode_t'
 * @param map pointer to a string array to describe the map.
 *            See 'lv_btnmatrix_set_map()' for more info.
 */
void lv_keyboard_set_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const char * map[])
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    kb_map[mode] = map;
    lv_keyboard_update_map(ext->btnm);
}

/**
 * Set the button control map (hidden, disabled etc.) for the keyboard. The
 * control map array will be copied and so may be deallocated after this
 * function returns.
 * @param kb pointer to a keyboard object
 * @param mode keyboard ctrl map to alter 'lv_keyboard_mode_t'
 * @param ctrl_map pointer to an array of `lv_btn_ctrl_t` control bytes.
 *                 See: `lv_btnmatrix_set_ctrl_map` for more details.
 */
void lv_keyboard_set_ctrl_map(lv_obj_t * kb, lv_keyboard_mode_t mode, const lv_btnmatrix_ctrl_t ctrl_map[])
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    kb_ctrl[mode] = ctrl_map;
    lv_keyboard_update_map(ext->btnm);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_keyboard_get_textarea(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_keyboard_mode_t'
 */
lv_keyboard_mode_t lv_keyboard_get_mode(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->mode;
}

/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_keyboard_get_cursor_manage(const lv_obj_t * kb)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->cursor_mng == 0 ? false : true;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Default keyboard event to add characters to the Text area and change the map.
 * If a custom `event_cb` is added to the keyboard this function be called from it to handle the
 * button clicks
 * @param kb pointer to a keyboard
 * @param event the triggering event
 */
void lv_keyboard_def_event_cb(lv_obj_t * kb, lv_event_t event)
{
    LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    if(event != LV_EVENT_VALUE_CHANGED) return;

    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    uint16_t btn_id   = lv_btnmatrix_get_active_btn(ext->btnm);
    if(btn_id == LV_BTNMATRIX_BTN_NONE) return;
    if(lv_btnmatrix_get_btn_ctrl(ext->btnm, btn_id, LV_BTNMATRIX_CTRL_HIDDEN | LV_BTNMATRIX_CTRL_DISABLED)) return;
    if(lv_btnmatrix_get_btn_ctrl(ext->btnm, btn_id, LV_BTNMATRIX_CTRL_NO_REPEAT) && event == LV_EVENT_LONG_PRESSED_REPEAT) return;

    const char * txt = lv_btnmatrix_get_active_btn_text(ext->btnm);
    if(txt == NULL) return;

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        ext->mode = LV_KEYBOARD_MODE_TEXT_LOWER;
        lv_btnmatrix_set_map(ext->btnm, kb_map[LV_KEYBOARD_MODE_TEXT_LOWER]);
        lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[LV_KEYBOARD_MODE_TEXT_LOWER]);
        lv_label_set_text(ext->match_word,"");
        return;
    }
#if LV_USE_ARABIC_PERSIAN_CHARS == 1
    else if(strcmp(txt, "أب") == 0) {
        ext->mode = LV_KEYBOARD_MODE_TEXT_ARABIC;
        lv_btnmatrix_set_map(ext->btnm, kb_map[LV_KEYBOARD_MODE_TEXT_ARABIC]);
        lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[LV_KEYBOARD_MODE_TEXT_ARABIC]);
        return;
    }
#endif
    else if(strcmp(txt, "Ch") == 0) {
        ext->mode = LV_KEYBOARD_MODE_CH;
        lv_btnmatrix_set_map(ext->btnm, kb_map[LV_KEYBOARD_MODE_CH]);
        lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[LV_KEYBOARD_MODE_CH]);
        ext->py_code_p = 0; //刚切换中文时输入长度清零
        ext->py_code[0] = '\0';
        lv_label_set_text(ext->match_word,"拼音:");
        return;
    }
    else if(strcmp(txt, "ABC") == 0) {
        ext->mode = LV_KEYBOARD_MODE_TEXT_UPPER;
        lv_btnmatrix_set_map(ext->btnm, kb_map[LV_KEYBOARD_MODE_TEXT_UPPER]);
        lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[LV_KEYBOARD_MODE_TEXT_UPPER]);
        lv_label_set_text(ext->match_word,"");
        return;
    }
    else if(strcmp(txt, "1#") == 0) {
        ext->mode = LV_KEYBOARD_MODE_SPECIAL;
        lv_btnmatrix_set_map(ext->btnm, kb_map[LV_KEYBOARD_MODE_SPECIAL]);
        lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[LV_KEYBOARD_MODE_SPECIAL]);
        lv_label_set_text(ext->match_word,"");
        return;
    }
    else if(strcmp(txt, LV_SYMBOL_CLOSE) == 0) {
        if(kb->event_cb != lv_keyboard_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_CANCEL, NULL);
            if(res != LV_RES_OK) return;
        }
        else {
            lv_keyboard_set_textarea(kb, NULL); /*De-assign the text area to hide it cursor if needed*/
            lv_obj_del(kb);
            return;
        }
        return;
    }
    else if(strcmp(txt, LV_SYMBOL_OK) == 0) {
        if(kb->event_cb != lv_keyboard_def_event_cb) {
            lv_res_t res = lv_event_send(kb, LV_EVENT_APPLY, NULL);
            if(res != LV_RES_OK) return;
        }
        else {
            lv_keyboard_set_textarea(kb, NULL); /*De-assign the text area to hide it cursor if needed*/
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(ext->ta == NULL) return;

    if(strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0)
    {
        if(ext->mode == LV_KEYBOARD_MODE_CH)
        {
            if(ext->py_code_p > 0)//有中文的时候换行输入英文
            {
                lv_textarea_add_text(ext->ta, (const char *)ext->py_code);
                lv_keyboard_match_clear(kb);
            }
            else
            {
                lv_textarea_add_char(ext->ta, '\n');
            }
        }
        else
        {
            lv_textarea_add_char(ext->ta, '\n');
        }
    }
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0)
        lv_textarea_cursor_left(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0)
        lv_textarea_cursor_right(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
    {
        if(ext->mode == LV_KEYBOARD_MODE_CH)
        {
            if(ext->py_code_p>0)
            {
                lv_keyboard_show_match_ch(kb, 2, '0');
            }
            else
            {
                lv_textarea_del_char(ext->ta);
            }
        }
        else
        {
            lv_textarea_del_char(ext->ta);
        }
    }
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur        = lv_textarea_get_cursor_pos(ext->ta);
        const char * ta_txt = lv_textarea_get_text(ext->ta);
        if(ta_txt[0] == '-') {
            lv_textarea_set_cursor_pos(ext->ta, 1);
            lv_textarea_del_char(ext->ta);
            lv_textarea_add_char(ext->ta, '+');
            lv_textarea_set_cursor_pos(ext->ta, cur);
        }
        else if(ta_txt[0] == '+') {
            lv_textarea_set_cursor_pos(ext->ta, 1);
            lv_textarea_del_char(ext->ta);
            lv_textarea_add_char(ext->ta, '-');
            lv_textarea_set_cursor_pos(ext->ta, cur);
        }
        else {
            lv_textarea_set_cursor_pos(ext->ta, 0);
            lv_textarea_add_char(ext->ta, '-');
            lv_textarea_set_cursor_pos(ext->ta, cur + 1);
        }
    }
    else {
        if(ext->mode == LV_KEYBOARD_MODE_CH)
        {
            if((txt[0] >= 'a')&&(txt[0] <= 'z'))
            {
                lv_keyboard_show_match_ch(kb, 1, (char)txt[0]);
            }
            else if(txt[0] == ' ')//空格输入第一个文字
            {
                if(get_matched_py_code((char *)ext->py_code, py_code_res) > 0)
                {
                    char s[4];
                    s[0] = py_code_res[0]->py_code[0];
                    s[1] = py_code_res[0]->py_code[1];
                    s[2] = py_code_res[0]->py_code[2];
                    s[3] = '\0';
                    lv_textarea_add_text(ext->ta, (const char *)s);
                    lv_keyboard_match_clear(kb);
                }
            }
            else//符号直接输入
            {
                lv_textarea_add_text(ext->ta, txt);
            }
        }
        else
        {
            lv_textarea_add_text(ext->ta, txt);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the keyboard
 * @param kb pointer to a keyboard object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_keyboard_signal(lv_obj_t * kb, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(kb, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }
    else if(sign == LV_SIGNAL_FOCUS) {
        lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_textarea_set_cursor_hidden(ext->ta, false);
        }
    }
    else if(sign == LV_SIGNAL_DEFOCUS) {
        lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
        /*Show the cursor of the Text area if cursor management is enabled*/
        if(ext->ta && ext->cursor_mng) {
            lv_textarea_set_cursor_hidden(ext->ta, true);
        }
    }

    return res;
}

/**
 * Update the key map for the current mode
 * @param kb pointer to a keyboard object
 */
static void lv_keyboard_update_map(lv_obj_t * kb)
{
    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    lv_btnmatrix_set_map(ext->btnm, kb_map[ext->mode]);
    lv_btnmatrix_set_ctrl_map(ext->btnm, kb_ctrl[ext->mode]);
}

static void lv_keyboard_match_clear(lv_obj_t * kb)
{
    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
    ext->py_code_p = 0;
    ext->py_code[ext->py_code_p] = '\0';
    lv_label_set_text_fmt(ext->match_word,"拼音:");
    if(1)lv_obj_clean(ext->match_word_btn);
    lv_obj_align(ext->match_word_btn, ext->match_word, LV_ALIGN_OUT_BOTTOM_LEFT, 0, -10);//重置匹配字位置
}

static lv_event_cb_t match_word_click_cb (lv_obj_t * label, lv_event_t e)
{
    if(e == LV_EVENT_SHORT_CLICKED)
    {
        lv_obj_t * kb = lv_obj_get_parent(lv_obj_get_parent(label));
        lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);
        lv_textarea_add_text(ext->ta, lv_label_get_text(label));
        lv_keyboard_match_clear(kb);
    }
}

static void lv_keyboard_show_match_ch(lv_obj_t * kb, uint8_t mode, char ch)
{
    int i;
    int len;
    char str[4];
    lv_keyboard_ext_t * ext = lv_obj_get_ext_attr(kb);

    lv_obj_clean(ext->match_word_btn);
    if(mode == 1)
    {
        if(ext->py_code_p >= PY_CODE_MAX_LEN - 1)ext->py_code_p = PY_CODE_MAX_LEN - 1;
        ext->py_code[ext->py_code_p] = ch;
        ext->py_code[ext->py_code_p+1] = '\0';
        ext->py_code_p++;
    }
    else if(mode == 2)
    {
        if(ext->py_code_p > 0)
        {
            ext->py_code_p--;
            ext->py_code[ext->py_code_p] = '\0';
        }
        else if(ext->py_code_p == 0)
        {
            ext->py_code[ext->py_code_p] = '\0';
        }
    }
    if(get_matched_py_code((char *)ext->py_code, py_code_res) > 0)
    {
        if(0)lv_label_set_text_fmt(ext->match_word,"拼音:%s,匹配字:%s",(const char *)ext->py_code, py_code_res[0]->py_code);
        lv_label_set_text_fmt(ext->match_word,"拼音:%s",(const char *)ext->py_code);
        len = strlen(py_code_res[0]->py_code);
        lv_obj_set_width(ext->match_word_btn,32 * len / 3);
        for(i = 0;i < len / 3;i++)
        {
            lv_obj_t * btn = lv_label_create(ext->match_word_btn,NULL);
            lv_obj_set_size(btn,32,32);
            //lv_obj_align(btn, ext->match_word_btn, LV_ALIGN_IN_LEFT_MID, i * 34, 0);
            str[0] = py_code_res[0]->py_code[i * 3 + 0];
            str[1] = py_code_res[0]->py_code[i * 3 + 1];
            str[2] = py_code_res[0]->py_code[i * 3 + 2];
            str[3] = '\0';
            lv_label_set_text(btn,(const char *)str);
            lv_obj_set_click(btn,true);
            lv_obj_set_event_cb(btn,match_word_click_cb);
        }
    }
    else
    {
        lv_label_set_text_fmt(ext->match_word,"拼音:%s",(const char *)ext->py_code);
    }
}

#endif
