/*
 * File:    Proj2_v0_2_Functions.cpp
 * Author:  Siyi Zhang
 * Purpose: 把 Project 1 Blackjack 拆成小 functions。
 * Version: 0.2
 */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
using namespace std;

// INPUT / MENU，简单 validation 也放进 functions
int getint(int low, int high);
float getbet(float bank);
char getact();
int menu();
void rules();
bool option(int pick, string nam, float &bank,
            int &wins, int &loss, int &push);

// FILE functions 用 reference 把旧记录送回 main
bool load(string path, float &bank, int &wins,
          int &loss, int &push);
bool save(string path, float bank, int wins,
          int loss, int push);
void stats(string nam, float bank, int wins,
           int loss, int push);

// GAME functions 展示 parameters 和 return values
int draw(int &aces);
int score(int rank);
int score(int total, int &aces);
int cards(int add = 0);
float payout(float bet, bool black);
void play(float &bank, int &wins, int &loss, int &push);

// MAIN pseudocode:
// 1. 从单一玩家文件 load 钱和战绩。
// 2. 菜单分别 call 游戏、统计和规则。
// 3. play 用 references 修改 main 里的资料。
// 4. 离开时检查 save 的 bool return。
int main(int argc, char **argv) {
    static_cast<void>(argc);
    static_cast<void>(argv);
    srand(static_cast<unsigned int>(time(0)));

    // 这一版仍是 one player file，后面才加 arrays。
    const string PATH = "player.txt";
    float bank = 100.0f;
    int wins = 0;
    int loss = 0;
    int push = 0;
    bool old = load(PATH, bank, wins, loss, push);

    cout << fixed << showpoint << setprecision(2);
    cout << "SIMPLIFIED BLACKJACK - FUNCTIONS\n";
    cout << "Enter one-word player name: ";
    string nam;
    cin >> nam;

    if (old) {
        cout << "Saved record loaded.\n";
    } else {
        cout << "New record: $100.00\n";
    }

    // do-while 让 menu 至少执行一次。
    bool run = true;
    do {
        int pick = menu();
        run = option(pick, nam, bank, wins, loss, push);
    } while (run);

    bool good = save(PATH, bank, wins, loss, push);
    if (!good) {
        cout << "Could not save the player file.\n";
    }
    cout << "Goodbye, " << nam << ".";
    exit(good ? EXIT_SUCCESS : EXIT_FAILURE);
}

// 读到范围外或 bad text 时继续问。
// loop 里面不用 break，合法后直接 return。
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

// 全部金额都使用 primitive float。
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

// 把小写 char 转成大写，再检查 Hit / Stand。
char getact() {
    char act;
    do {
        cout << "(H)it or (S)tand? ";
        cin >> act;
        if (act >= 'a' && act <= 'z') {
            act = static_cast<char>(act - 'a' + 'A');
        }
    } while (act != 'H' && act != 'S');
    return act;
}

// menu 只负责显示和 return choice。
int menu() {
    cout << "\n1. Play\n"
         << "2. My statistics\n"
         << "3. Rules\n"
         << "4. Save and exit\n"
         << "Choice: ";
    return getint(1, 4);
}

// switch 单独放在 function，所以 menu loop 不用 break。
bool option(int pick, string nam, float &bank,
            int &wins, int &loss, int &push) {
    switch (pick) {
        case 1:
            play(bank, wins, loss, push);
            return true;
        case 2:
            stats(nam, bank, wins, loss, push);
            return true;
        case 3:
            rules();
            return true;
        default:
            return false;
    }
}

// 继续使用 Project 1 的 simplified rules。
void rules() {
    cout << "\nGet close to 21 without going over.\n"
         << "Face cards are 10. An Ace is 11 or 1.\n"
         << "The dealer must hit below 17.\n"
         << "A two-card Blackjack pays 3 to 2.\n";
}

// FILE INPUT:
// 文件缺少或 fields 不合理时保留新账号初始值。
bool load(string path, float &bank, int &wins,
          int &loss, int &push) {
    ifstream in(path.c_str());
    bool good = static_cast<bool>(
        in >> bank >> wins >> loss >> push);
    if (good) {
        good = bank >= 0.0f && wins >= 0 &&
               loss >= 0 && push >= 0;
    }
    in.close();
    if (!good) {
        bank = 100.0f;
        wins = 0;
        loss = 0;
        push = 0;
    }
    return good;
}

// FILE OUTPUT:
// bool return 告诉 main 文件有没有成功打开。
bool save(string path, float bank, int wins,
          int loss, int push) {
    ofstream out(path.c_str());
    if (!out) {
        return false;
    }
    out << fixed << setprecision(2)
        << bank << ' ' << wins << ' '
        << loss << ' ' << push;
    out.close();
    return true;
}

// 统计画面用 expression 算总局数和胜率。
void stats(string nam, float bank, int wins,
           int loss, int push) {
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

// 随机 rank 先送给 overloaded score。
// aces 用 pass by reference 记录能调整的 A。
int draw(int &aces) {
    int rank = rand() % 13 + 1;
    int val = score(rank);
    if (rank == 1) {
        aces++;
    }
    cards(1);
    return val;
}

// 第一个 score 接一张牌的 rank。
int score(int rank) {
    int val = rank;
    if (rank == 1) {
        val = 11;
    }
    if (rank > 10) {
        val = 10;
    }
    return val;
}

// 第二个 score 接 total 和 Ace 数量。
int score(int total, int &aces) {
    while (total > 21 && aces > 0) {
        total -= 10;
        aces--;
    }
    return total;
}

// local static 会在多次 function calls 之间保留。
// add 的 default argument 是 0。
int cards(int add) {
    static int total = 0;
    total += add;
    return total;
}

// Blackjack 使用 3:2，roundf 保留 cents。
float payout(float bet, bool black) {
    float rate = black ? 1.5f : 1.0f;
    return roundf(bet * rate * 100.0f) / 100.0f;
}

// PLAY pseudocode:
// 1. 检查余额，再取得合法 bet。
// 2. 双方各抽两张 random cards。
// 3. 玩家选择 Hit / Stand，dealer 抽到 17。
// 4. compare total，再用 references 更新记录。
void play(float &bank, int &wins, int &loss, int &push) {
    if (bank < 1.0f) {
        cout << "You need at least $1.00 to play.\n";
        return;
    }

    float bet = getbet(bank);
    int ptotal = 0;
    int dtotal = 0;
    int paces = 0;
    int daces = 0;
    int dshow = 0;

    for (int i = 0; i < 2; i++) {
        ptotal += draw(paces);
        ptotal = score(ptotal, paces);
        int val = draw(daces);
        dtotal += val;
        dtotal = score(dtotal, daces);
        if (i == 0) {
            dshow = val;
        }
    }

    cout << "\nDealer shows: " << dshow
         << "\nYour total: " << ptotal << '\n';
    bool pjack = ptotal == 21;
    bool djack = dtotal == 21;
    char act = 'H';

    // 玩家 loop 里没有 break。
    while (!pjack && !djack && ptotal < 21 && act != 'S') {
        act = getact();
        if (act == 'H') {
            int val = draw(paces);
            ptotal += val;
            ptotal = score(ptotal, paces);
            cout << "You drew: " << val
                 << "\nYour total: " << ptotal << '\n';
        }
    }

    if (ptotal <= 21 && !pjack && !djack) {
        while (dtotal < 17) {
            int val = draw(daces);
            dtotal += val;
            dtotal = score(dtotal, daces);
            cout << "Dealer drew: " << val << '\n';
        }
    }

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

    cout << "Dealer total: " << dtotal << '\n';
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

    if (bank < 0.005f) {
        bank = 0.0f;
    }
    cout << "New bankroll: $" << bank << '\n';
}