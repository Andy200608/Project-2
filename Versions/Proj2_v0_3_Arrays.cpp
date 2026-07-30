/*
 * File:    Proj2_v0_3_Arrays.cpp
 * Author:  Siyi Zhang
 * Purpose: 加入 1D/2D arrays 和 Vector hands。
 * Version: 0.3
 */
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <vector>
using namespace std;

// INPUT / MENU functions
int getint(int low, int high);
float getbet(float bank);
char getact();
int menu();
void rules();
bool option(int pick, string nam, float &bank,
            int &wins, int &loss, int &push);

// 这一版仍用 one-player file
bool load(string path, float &bank, int &wins,
          int &loss, int &push);
bool save(string path, float bank, int wins,
          int loss, int push);
void stats(string nam, float bank, int wins,
           int loss, int push);

// DECK / VECTOR functions
void mkdeck(int grid[][13], int deck[]);
void shufle(int deck[], int size = 52);
vector<int> deal(int deck[], int &pos);
void hit(vector<int> &hand, int deck[], int &pos);
int score(int card);
int score(const vector<int> &hand);
bool nat21(const vector<int> &hand);
string cname(int card);
void show(const vector<int> &hand);
void show(const vector<int> &hand, bool hide);
int cards(int add = 0);
float payout(float bet, bool black);
void play(float &bank, int &wins, int &loss, int &push);

// MAIN pseudocode:
// 1. load 单一玩家资料。
// 2. menu 把工作交给 separate functions。
// 3. play 建 4x13 grid、deck[52] 和两只 Vectors。
// 4. save 的 bool return 决定 exit status。
// 所有 arrays 和 Vectors 都保持 local，不建立 global data。
int main(int argc, char **argv) {
    static_cast<void>(argc);
    static_cast<void>(argv);
    srand(static_cast<unsigned int>(time(0)));

    const string PATH = "player.txt";
    float bank = 100.0f;
    int wins = 0;
    int loss = 0;
    int push = 0;
    bool old = load(PATH, bank, wins, loss, push);

    cout << fixed << showpoint << setprecision(2);
    cout << "SIMPLIFIED BLACKJACK - ARRAYS\n";
    cout << "Enter one-word player name: ";
    string nam;
    cin >> nam;
    cout << (old ? "Saved record loaded.\n"
                 : "New record: $100.00\n");

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

// 范围外或 bad text 都会继续问。
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

// bet 使用 float，而且不能超过 bankroll。
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

// do-while 检查 Hit / Stand。
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

int menu() {
    cout << "\n1. Play\n"
         << "2. My statistics\n"
         << "3. Rules\n"
         << "4. Save and exit\n"
         << "Choice: ";
    return getint(1, 4);
}

// switch 自己 return，外层 menu loop 不需要 break。
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

void rules() {
    cout << "\nGet close to 21 without going over.\n"
         << "Face cards are 10. An Ace is 11 or 1.\n"
         << "The dealer must hit below 17.\n"
         << "A two-card Blackjack pays 3 to 2.\n";
}

// FILE INPUT 用 references 传回 fields。
// 1. 尝试打开 one-player file。
// 2. 一次读完 bank、wins、loss、push。
// 3. 任一值不合法就 reset 成新账号。
// 4. return value 让 main 知道有没有旧资料。
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

// FILE OUTPUT 最后一项后不加 newline。
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

// 2D ARRAY:
// grid 按 4 suits x 13 ranks 建好，再 copy 到 deck。
// outer loop 选择 suit，inner loop 选择 rank。
// 每个 cell 都有 0 到 51 之间的 unique id。
// deck 是 1D array，发牌时比较容易往后移动。
// 两种 arrays 都会真正传进 function 使用。
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

// Fisher-Yates 保证同一副 deck 不会重复发牌。
void shufle(int deck[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// deal 会 return 一只两张牌的 Vector。
// 1. 建 empty hand。
// 2. 从 deck 当前 pos 拿牌。
// 3. push_back 后把 pos 和 counter 加一。
// 4. 完成两次以后 return 整只 hand。
vector<int> deal(int deck[], int &pos) {
    vector<int> hand;
    for (int i = 0; i < 2; i++) {
        hand.push_back(deck[pos]);
        pos++;
        cards(1);
    }
    return hand;
}

// hit 用 reference 修改原来的 Vector。
void hit(vector<int> &hand, int deck[], int &pos) {
    hand.push_back(deck[pos]);
    pos++;
    cards(1);
}

// overloaded score：这一版先接单张 card id。
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

// 第二个 score 接完整 Vector，并调整多余的 Aces。
// 1. for loop 加总每一张牌。
// 2. 遇到 Ace 就同步增加 aces。
// 3. total 爆掉时把一个 Ace 从 11 改成 1。
// 4. 没有可调 Ace 后 return final total。
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

bool nat21(const vector<int> &hand) {
    return hand.size() == 2 && score(hand) == 21;
}

// 两只 local arrays 把数字 card 转成 readable name。
string cname(int card) {
    const string rank[13] = {
        "Ace", "Two", "Three", "Four", "Five", "Six", "Seven",
        "Eight", "Nine", "Ten", "Jack", "Queen", "King"
    };
    const string suit[4] = {
        "Clubs", "Diamonds", "Hearts", "Spades"
    };
    return rank[card % 13] + " of " + suit[card / 13];
}

// 第一个 show 显示全部 cards。
void show(const vector<int> &hand) {
    show(hand, false);
}

// overloaded show 可以藏住 dealer 第二张牌。
// hide 为 true 时仍保留第一张 visible card。
// 普通版本会把所有 card 和 total 都打印出来。
// 这里只读取 Vector，不会改变里面的牌。
void show(const vector<int> &hand, bool hide) {
    for (int i = 0; i < static_cast<int>(hand.size()); i++) {
        if (hide && i == 1) {
            cout << " [hidden card]\n";
        } else {
            cout << ' ' << cname(hand[i]) << '\n';
        }
    }
    if (!hide) {
        cout << "Total: " << score(hand) << '\n';
    }
}

// static counter 使用 default argument。
int cards(int add) {
    static int total = 0;
    total += add;
    return total;
}

float payout(float bet, bool black) {
    float rate = black ? 1.5f : 1.0f;
    return roundf(bet * rate * 100.0f) / 100.0f;
}

// PLAY:
// 1. 建 local grid、deck 和两只 hands。
// 2. 玩家和 dealer 都从同一副 shuffled deck 抽牌。
// 3. compare totals，再更新 single player record。
// 4. natural 21 会先于普通 Hit / Stand 判断。
// 5. 玩家 loop 以 act 和 total 控制，不用 break。
// 6. dealer 只有在玩家没爆牌时才继续抽。
// 7. result 1、2、3 分别代表 win、loss、push。
// 8. payout 只在 win 时计算，普通牌局倍率是 1。
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

    cout << "\nYOUR HAND\n";
    show(plyr);
    cout << "DEALER HAND\n";
    show(dlr, true);
    bool pjack = nat21(plyr);
    bool djack = nat21(dlr);
    char act = 'H';

    while (!pjack && !djack && score(plyr) < 21 && act != 'S') {
        act = getact();
        if (act == 'H') {
            hit(plyr, deck, pos);
            cout << "You drew: " << cname(plyr.back()) << '\n';
            show(plyr);
        }
    }

    if (score(plyr) <= 21 && !pjack && !djack) {
        while (score(dlr) < 17) {
            hit(dlr, deck, pos);
            cout << "Dealer drew: " << cname(dlr.back()) << '\n';
        }
    }

    int ptotal = score(plyr);
    int dtotal = score(dlr);
    int result;
    cout << "Dealer total: " << dtotal << '\n';
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