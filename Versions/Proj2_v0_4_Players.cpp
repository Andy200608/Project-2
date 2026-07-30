/*
 * File:    Proj2_v0_4_Players.cpp
 * Author:  Siyi Zhang
 * Purpose: 加入 parallel player arrays 和 Linear Search。
 * Version: 0.4
 */
// System Libraries
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <vector>
using namespace std;
// User Libraries - none
// Global Constants - none allowed in Project 2
// Function Prototypes
// INPUT / MENU functions
int getint(int low, int high);
float getbet(float bank);
char getact();
int menu();
void rules();
bool option(int pick, string nam[], float bank[], int wins[],
            int loss[], int push[], int cnt, int cur);
// PLAYER FILE 用 parallel arrays 保存资料
// 每个 index 代表同一个 player 的完整资料。
// nam[2]、bank[2] 和 wins[2] 必须属于同一人。
// load、save 和 search 都接收这些 arrays。
// 这一版最多放十人，避免 data 超出 fixed size。
// 后面的 sorting 版也要保持这些 indexes 对齐。
int load(string path, string nam[], float bank[], int wins[],
         int loss[], int push[], int max);
bool save(string path, const string nam[], const float bank[],
          const int wins[], const int loss[], const int push[], int cnt);
int findpl(const string nam[], int cnt, string key);
int addpl(string nam[], float bank[], int wins[], int loss[],
          int push[], int &cnt, int max, string who);
void stats(string nam, float bank, int wins, int loss, int push);
void findui(const string nam[], const float bank[], const int wins[],
            const int loss[], const int push[], int cnt);
// DECK / VECTOR functions
void mkdeck(int grid[][13], int deck[]);
void shufle(int deck[], int size = 52);
vector<int> deal(int deck[], int &pos);
void hit(vector<int> &hand, int deck[], int &pos);
int score(int card);
int score(const vector<int> &hand);
bool nat21(const vector<int> &hand);
int cards(int add = 0);
// 一局会用 reference 更新钱和战绩
float payout(float bet, bool black);
void play(float &bank, int &wins, int &loss, int &push);
// MAIN pseudocode:
// 1. Create 五组 local arrays，再从 file load players。
// 2. Use Linear Search 找名字，not found 就 add。
// 3. Repeat menu，处理 play、stats、search 和 rules。
// 4. Save 五组 matching arrays before exit。
// 5. display functions 只读取当前玩家的位置。
// 6. Exit status 由 save 的 bool result 决定。
// main 外面只有 prototypes，不放 global variables。
// Execution Begins Here!
int main(int argc, char **argv) {
    static_cast<void>(argc);
    static_cast<void>(argv);
    srand(static_cast<unsigned int>(time(0)));
    // 所有 data 和 constants 都在 local scope
    const int MAX = 10;
    const string PATH = "players.txt";
    string nam[MAX];
    float bank[MAX];
    int wins[MAX];
    int loss[MAX];
    int push[MAX];
    // sizeof 算出的容量会传进 load
    int cap = static_cast<int>(sizeof(nam) / sizeof(nam[0]));
    int cnt = load(PATH, nam, bank, wins, loss, push, cap);
    cout << fixed << showpoint << setprecision(2);
    cout << "SIMPLIFIED BLACKJACK - PROJECT 2\n";
    cout << "Enter one-word player name: ";
    string who;
    cin >> who;
    // SEARCH before add，避免重复玩家
    int cur = findpl(nam, cnt, who);
    if (cur >= 0) {
        cout << "Welcome back, " << nam[cur] << ".\n";
        cout << "Saved bankroll: $" << bank[cur] << '\n';
    } else {
        cur = addpl(nam, bank, wins, loss, push, cnt, cap, who);
        if (cur < 0) {
            cout << "The player list is full.";
            exit(EXIT_FAILURE);
        }
        cout << "New account: $100.00\n";
    }
    // do-while 让菜单至少执行一次
    // switch 放在 option 里面，所以 loop body 没有 break。
    bool run = true;
    do {
        int pick = menu();
        run = option(pick, nam, bank, wins, loss, push, cnt, cur);
    } while (run);
    // bool return 告诉 main 文件有没有写成功
    bool good = save(PATH, nam, bank, wins, loss, push, cnt);
    if (!good) {
        cout << "Could not save the player file.\n";
    }
    cout << "Goodbye, " << nam[cur] << ".";
    exit(good ? EXIT_SUCCESS : EXIT_FAILURE);
}
// INTEGER INPUT:
// 1. 先读一个数。
// 2. cin fail 或越界时清掉这一行。
// 3. 不在 loop 里 break，直到合法才 return。
// low 和 high 用 pass by value 送进来。
int getint(int low, int high) {
    int num;
    cin >> num;
    while (cin.fail() || num < low || num > high) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Enter " << low << " through " << high << ": ";
        cin >> num;
    }
    return num;
}
// BET INPUT:
// 1. 下注至少 $1，也不能超过 bankroll。
// 2. bad text 会清除 fail state。
// 3. 合法的 primitive float 由函数 return。
// 余额只读取，不在这个 function 里面修改。
float getbet(float bank) {
    float bet;
    cout << "Bankroll: $" << bank << "\nBet: $";
    cin >> bet;
    while (cin.fail() || bet < 1.0f || bet > bank) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Enter $1.00 through $" << bank << ": $";
        cin >> bet;
    }
    return bet;
}
// ACTION INPUT 用 do-while 检查 H/S
// 小写 char 会 type cast 成大写
// 其他 character 会继续留在 input loop。
char getact() {
    char act = ' ';
    do {
        cout << "(H)it or (S)tand? ";
        cin >> act;
        if (act >= 'a' && act <= 'z') {
            act = static_cast<char>(act - 'a' + 'A');
        }
    } while (act != 'H' && act != 'S');
    return act;
}
// 菜单统一调用 getint 做 validation
// menu 自己 return choice，不处理游戏 data。
int menu() {
    cout << "\n1. Play\n"
         << "2. My statistics\n"
         << "3. Find a player\n"
         << "4. Rules\n"
         << "5. Save and exit\n"
         << "Choice: ";
    return getint(1, 5);
}
// MENU OPTION:
// switch 单独放在这个 function，不属于任何 loop。
// 每个 case 直接 return，因此完全不需要 break。
// true 表示继续菜单，false 表示 save and exit。
bool option(int pick, string nam[], float bank[], int wins[],
            int loss[], int push[], int cnt, int cur) {
    switch (pick) {
        case 1:
            play(bank[cur], wins[cur], loss[cur], push[cur]);
            return true;
        case 2:
            stats(nam[cur], bank[cur], wins[cur], loss[cur], push[cur]);
            return true;
        case 3:
            findui(nam, bank, wins, loss, push, cnt);
            return true;
        case 4:
            rules();
            return true;
        default:
            return false;
    }
}
// 保留 Project 1 的 simplified rules
// 这不是新编规则，还是一般人熟悉的 Blackjack。
void rules() {
    cout << "\nGet close to 21 without going over.\n"
         << "Face cards are 10. An Ace is 11 or 1.\n"
         << "The dealer must hit below 17.\n"
         << "A two-card Blackjack pays 3 to 2.\n";
}
// FILE INPUT:
// 一行读 name, bank, wins, losses, pushes。
// 读到 EOF、bad row 或 max 人就停止。
// arrays 会把资料传回 main。
// cnt 每增加一次，就代表完整读到一位玩家。
// 简单课程项目只接受程序自己写出的五个 fields。
int load(string path, string nam[], float bank[], int wins[],
         int loss[], int push[], int max) {
    ifstream in(path.c_str());
    int cnt = 0;
    while (cnt < max &&
           in >> nam[cnt] >> bank[cnt] >> wins[cnt]
              >> loss[cnt] >> push[cnt]) {
        cnt++;
    }
    in.close();
    return cnt;
}
// FILE OUTPUT:
// 同一个 index 的 fields 写在同一行。
// open 失败 return false，成功 return true。
// 最后一行不额外输出 newline。
bool save(string path, const string nam[], const float bank[],
          const int wins[], const int loss[], const int push[], int cnt) {
    ofstream out(path.c_str());
    if (!out) {
        return false;
    }
    out << fixed << setprecision(2);
    for (int i = 0; i < cnt; i++) {
        out << nam[i] << ' ' << bank[i] << ' ' << wins[i] << ' '
            << loss[i] << ' ' << push[i];
        if (i < cnt - 1) {
            out << '\n';
        }
    }
    out.close();
    return !out.fail();
}
// LINEAR SEARCH:
// 1. 从 index 0 开始，一个个 compare name。
// 2. 只要 pos 还是 -1，就继续推进 i。
// 3. found 后 while condition 会自然停止。
// 4. 全部检查完仍没找到，就 return -1。
// 这里 search 的是玩家，不是 cards。
// 玩家使用 one-word name，所以直接 compare string。
int findpl(const string nam[], int cnt, string key) {
    int pos = -1;
    int i = 0;
    while (i < cnt && pos == -1) {
        if (nam[i] == key) {
            pos = i;
        }
        i++;
    }
    return pos;
}
// ADD PLAYER:
// 1. 新玩家使用五组 arrays 的同一个 empty index。
// 2. bank 从 $100 开始，三个 records 从 0 开始。
// 3. cnt 用 pass by reference 送回 main。
// 4. arrays 满了就维持 -1，不写到范围外。
int addpl(string nam[], float bank[], int wins[], int loss[],
          int push[], int &cnt, int max, string who) {
    int pos = -1;
    if (cnt < max) {
        pos = cnt;
        nam[pos] = who;
        bank[pos] = 100.0f;
        wins[pos] = 0;
        loss[pos] = 0;
        push[pos] = 0;
        cnt++;
    }
    return pos;
}
// STATS 用 math expression 计算总局数和胜率
// conditional operator 处理还没玩过的玩家
// setprecision 显示一位百分比后再改回两位钱数。
void stats(string nam, float bank, int wins, int loss, int push) {
    int game = wins + loss + push;
    float rate = game > 0
               ? 100.0f * static_cast<float>(wins) /
                 static_cast<float>(game)
               : 0.0f;
    cout << "\nPlayer: " << nam
         << "\nBankroll: $" << bank
         << "\nGames: " << game
         << "\nWins: " << wins
         << "\nLosses: " << loss
         << "\nPushes: " << push
         << setprecision(1)
         << "\nWin rate: " << rate << "%\n"
         << setprecision(2)
         << "Cards dealt this run: " << cards() << '\n';
}
// SEARCH SCREEN:
// 用同一个 findpl 查其他玩家。
// 找到才把 matching fields 传给 stats。
void findui(const string nam[], const float bank[], const int wins[],
            const int loss[], const int push[], int cnt) {
    cout << "Player name to find: ";
    string key;
    cin >> key;
    int pos = findpl(nam, cnt, key);
    if (pos >= 0) {
        stats(nam[pos], bank[pos], wins[pos], loss[pos], push[pos]);
    } else {
        cout << "Player not found.\n";
    }
}
// 2D ARRAY:
// grid 有 4 suits x 13 ranks。
// 每格放 unique id，再传到 deck[52]。
// 两层 for loop 会刚好写入 52 个位置。
// 因此同一局不会抽到完全相同的一张牌。
void mkdeck(int grid[][13], int deck[]) {
    int pos = 0;
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 0; rank < 13; rank++) {
            grid[suit][rank] = suit * 13 + rank;
            deck[pos] = grid[suit][rank];
            pos++;
        }
    }
}
// SHUFFLE:
// Fisher-Yates 从 deck 后面往前换。
// size 没写时使用 default argument 52。
void shufle(int deck[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}
// VECTOR OUT:
// deal 建一只两张牌的 Vector 再 return。
// pos reference 会保留 deck 的新位置。
// cards(1) 同时记录这次发牌数量。
vector<int> deal(int deck[], int &pos) {
    vector<int> hand;
    for (int i = 0; i < 2; i++) {
        hand.push_back(deck[pos]);
        pos++;
        cards(1);
    }
    return hand;
}
// VECTOR IN/OUT:
// hit 用 reference 修改原来的 hand。
// deck 与 pos 也继续传进 function。
// push_back 会在原 Vector 后面加一张。
void hit(vector<int> &hand, int deck[], int &pos) {
    hand.push_back(deck[pos]);
    pos++;
    cards(1);
}
// 第一个 overloaded score 接一张 card id
// 两个 independent if 处理 Ace 和 face cards
// 第二个同名 function 会接收整个 Vector。
int score(int card) {
    int rank = card % 13 + 1;
    int val = rank;
    if (rank == 1) {
        val = 11;
    }
    if (rank > 10) {
        val = 10;
    }
    return val;
}
// 第二个 overloaded score 接一整只 Vector
// Ace 先算 11，爆牌时再一个个改成 1
int score(const vector<int> &hand) {
    int total = 0;
    int aces = 0;
    for (int i = 0; i < static_cast<int>(hand.size()); i++) {
        total += score(hand[i]);
        if (hand[i] % 13 == 0) {
            aces++;
        }
    }
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }
    return total;
}
// BOOLEAN RETURN: 两张牌而且总数 21
bool nat21(const vector<int> &hand) {
    return hand.size() == 2 && score(hand) == 21;
}
// STATIC + DEFAULT ARGUMENT:
// total 在 function calls 之间不会消失。
// add 默认是 0，所以 stats 可以只读取。
int cards(int add) {
    static int total = 0;
    total += add;
    return total;
}
// CMATH 把 prize round 到 cents
// black 用 conditional operator 决定倍率
float payout(float bet, bool black) {
    float rate = black ? 1.5f : 1.0f;
    return roundf(bet * rate * 100.0f) / 100.0f;
}
// BLACKJACK ROUND:
// 1. 先建 4x13 grid，再 shuffle deck。
// 2. deal() return 两只 Vector hands。
// 3. 玩家 Hit/Stand，dealer 少于 17 自动 Hit。
// 4. compare totals，用 references 更新钱和 record。
// 5. 最后 round 到 cents 并打印新余额。
void play(float &bank, int &wins, int &loss, int &push) {
    if (bank < 1.0f) {
        cout << "You need at least $1.00 to play.\n";
        return;
    }
    float bet = getbet(bank);
    int grid[4][13];
    int deck[52];
    int pos = 0;
    mkdeck(grid, deck);
    shufle(deck);
    vector<int> plyr = deal(deck, pos);
    vector<int> dlr = deal(deck, pos);
    cout << "\nDealer shows: " << score(dlr[1])
         << "\nYour total: " << score(plyr) << '\n';
    bool pjack = nat21(plyr);
    bool djack = nat21(dlr);
    char act = 'H';
    // 玩家 loop 里不使用 break
    while (!pjack && !djack && score(plyr) < 21 && act != 'S') {
        act = getact();
        if (act == 'H') {
            hit(plyr, deck, pos);
            cout << "You drew: " << score(plyr.back())
                 << "\nYour total: " << score(plyr) << '\n';
        }
    }
    // 玩家没爆时 dealer 才继续
    // 这个 outer if 只决定 dealer 是否需要继续。
    if (score(plyr) <= 21 && !pjack && !djack) {
        while (score(dlr) < 17) {
            hit(dlr, deck, pos);
            cout << "Dealer drew: " << score(dlr.back()) << '\n';
        }
    }
    int ptotal = score(plyr);
    int dtotal = score(dlr);
    cout << "Dealer total: " << dtotal << '\n';
    // result: 1 win, 2 loss, 3 push
    // if-else-if chain 只会选中一个最终结果。
    int result;
    if (pjack && djack) {
        result = 3;
    } else if (pjack) {
        result = 1;
    } else if (djack || ptotal > 21) {
        result = 2;
    } else if (dtotal > 21 || ptotal > dtotal) {
        result = 1;
    } else if (ptotal < dtotal) {
        result = 2;
    } else {
        result = 3;
    }
    // 更新 bankroll 和 matching record
    if (result == 1) {
        float prize = payout(bet, pjack);
        bank = roundf((bank + prize) * 100.0f) / 100.0f;
        wins++;
        cout << "You win $" << prize << ".\n";
    } else if (result == 2) {
        bank = roundf((bank - bet) * 100.0f) / 100.0f;
        loss++;
        cout << "Dealer wins. You lose $" << bet << ".\n";
    } else {
        push++;
        cout << "Push.\n";
    }
    // 很接近 0 时直接归零
    if (bank < 0.005f) {
        bank = 0.0f;
    }
    cout << "New bankroll: $" << bank << '\n';
}