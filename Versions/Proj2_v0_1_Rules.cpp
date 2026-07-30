/*
 * File:    Proj2_v0_1_Rules.cpp
 * Author:  Siyi Zhang
 * Purpose: 先整理 Project 1 的类型、命名和基本规则。
 * Version: 0.1
 */

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

int main(int argc, char** argv) {
    // 保留老师给的 main signature，这一版还不用 arguments。
    static_cast<void>(argc);
    static_cast<void>(argv);

    // 每次运行先换 random seed。
    srand(static_cast<unsigned int>(time(0)));

    // 这一版先只保存一个玩家，全部 variables 都是 local。
    const string file = "blackjack_stats.txt";
    string name;
    float bank = 100.00f;
    int game = 0;
    int wins = 0;
    int loss = 0;
    int push = 0;
    int menu = 0;
    bool run = true;
    bool load = false;

    // 文件存在而且内容有效时才 load 旧记录。
    ifstream fin(file.c_str());
    if (fin) {
        fin >> bank >> game >> wins >> loss >> push;
        if (fin && bank >= 0 && game >= 0) {
            load = true;
        } else {
            bank = 100.00f;
            game = wins = loss = push = 0;
        }
    }
    fin.close();

    cout << fixed << showpoint << setprecision(2);
    cout << "SIMPLIFIED BLACKJACK\n";
    cout << "Enter your name: ";
    getline(cin, name);

    if (name.empty()) {
        name = "Player";
    }

    if (load) {
        cout << "Saved data load.\n";
    } else {
        cout << "New account: $100.00\n";
    }

    // 重复 main menu，直到玩家选择保存离开。
    do {
        cout << "\n1. Play\n";
        cout << "2. Rules\n";
        cout << "3. Statistics\n";
        cout << "4. Save and exit\n";
        cout << "Choice: ";
        cin >> menu;

        while (cin.fail() || menu < 1 || menu > 4) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Enter 1, 2, 3, or 4: ";
            cin >> menu;
        }

        if (menu == 1) {
            if (bank < 1.00f) {
                cout << "You need at least $1.00 to play.\n";
            }

            if (bank >= 1.00f) {
                float bet = 0.0f;
                bool valid = false;

                // 下注用 float，并在 loop 里一直检查到合法。
                while (!valid) {
                    cout << "Bankroll: $" << bank << '\n';
                    cout << "Bet: $";
                    cin >> bet;

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cout << "Enter a number.\n";
                    } else if (bet < 1.00f) {
                        cout << "Minimum bet is $1.00.\n";
                    } else if (bet > bank) {
                        cout << "Bet is too high.\n";
                    } else {
                        valid = true;
                    }
                }

                int ptotal = 0;
                int dtotal = 0;
                int paces = 0;
                int daces = 0;
                int shown = 0;

                cout << "\nInitial cards\n";

                // 玩家和 dealer 各发两张，先把 Ace 算成 11。
                for (int card = 1; card <= 2; card++) {
                    int rank = rand() % 13 + 1;
                    int value = (rank > 10) ? 10 : rank;

                    if (rank == 1) {
                        value = 11;
                        paces++;
                    }

                    ptotal += value;

                    while (ptotal > 21 && paces > 0) {
                        ptotal -= 10;
                        paces--;
                    }

                    cout << "Your card " << card << ": " << value << '\n';

                    rank = rand() % 13 + 1;
                    value = (rank > 10) ? 10 : rank;

                    if (rank == 1) {
                        value = 11;
                        daces++;
                    }

                    dtotal += value;

                    while (dtotal > 21 && daces > 0) {
                        dtotal -= 10;
                        daces--;
                    }

                    if (card == 1) {
                        shown = value;
                    }
                }

                cout << "Dealer shows: " << shown << '\n';
                cout << "Your total: " << ptotal << '\n';

                bool pjack = (ptotal == 21);
                bool djack = (dtotal == 21);
                int result = 0;

                // 先检查双方开局是不是 Blackjack。
                if (pjack || djack) {
                    cout << "Dealer total: " << dtotal << '\n';

                    if (pjack && djack) {
                        result = 3;
                    } else if (pjack) {
                        result = 1;
                    } else {
                        result = 2;
                    }
                } else {
                    char action = 'H';

                    // 玩家在这里选择 Hit 或 Stand。
                    while (ptotal < 21 && action != 'S') {
                        do {
                            cout << "(H)it or (S)tand? ";
                            cin >> action;

                            if (action >= 'a' && action <= 'z') {
                                action = static_cast<char>(action - 32);
                            }

                            if (action != 'H' && action != 'S') {
                                cout << "Enter H or S.\n";
                            }
                        } while (action != 'H' && action != 'S');

                        if (action == 'H') {
                            int rank = rand() % 13 + 1;
                            int value = (rank > 10) ? 10 : rank;

                            if (rank == 1) {
                                value = 11;
                                paces++;
                            }

                            ptotal += value;
                            cout << "You drew: " << value << '\n';

                            if (ptotal > 21 && paces > 0) {
                                ptotal -= 10;
                                paces--;
                                cout << "Ace changed from 11 to 1.\n";
                            }

                            cout << "Your total: " << ptotal << '\n';
                        }
                    }

                    if (ptotal > 21) {
                        result = 2;
                        cout << "You busted.\n";
                    } else {
                        cout << "Dealer total: " << dtotal << '\n';

                        // dealer 少于 17 就继续抽牌。
                        while (dtotal < 17) {
                            int rank = rand() % 13 + 1;
                            int value = (rank > 10) ? 10 : rank;

                            if (rank == 1) {
                                value = 11;
                                daces++;
                            }

                            dtotal += value;
                            cout << "Dealer drew: " << value << '\n';

                            if (dtotal > 21 && daces > 0) {
                                dtotal -= 10;
                                daces--;
                                cout << "Dealer changed an Ace to 1.\n";
                            }

                            cout << "Dealer total: " << dtotal << '\n';
                        }

                        // 最后 compare 双方点数。
                        if (dtotal > 21 ||
                            ptotal > dtotal) {
                            result = 1;
                        } else if (ptotal < dtotal) {
                            result = 2;
                        } else {
                            result = 3;
                        }
                    }
                }

                game++;

                // 按 result 更新 bankroll 和战绩。
                if (result == 1) {
                    float prize = pjack ? bet * 1.5f : bet;
                    bank += prize;
                    wins++;
                    cout << "You win $" << prize << ".\n";
                } else if (result == 2) {
                    bank -= bet;
                    loss++;
                    cout << "Dealer wins. You lose $" << bet << ".\n";
                } else {
                    push++;
                    cout << "Push.\n";
                }

                if (bank < 0.005f) {
                    bank = 0.00f;
                }

                cout << "New bank: $" << bank << '\n';
            }
        } else if (menu == 2) {
            cout << "\nGet close to 21 without going over.\n";
            cout << "Face cards are 10. An Ace is 11 or 1.\n";
            cout << "The dealer must hit below 17.\n";
            cout << "A Blackjack pays 3 to 2.\n";
        } else if (menu == 3) {
            float rate = game > 0
                       ? 100.0f * static_cast<float>(wins) /
                         static_cast<float>(game)
                       : 0.0f;
            cout << "\nPlayer: " << name << '\n';
            cout << "Bankroll: $" << bank << '\n';
            cout << "Games: " << game << '\n';
            cout << "Wins: " << wins << '\n';
            cout << "Losses: " << loss << '\n';
            cout << "Pushes: " << push << '\n';
            cout << setprecision(1);
            cout << "Win rate: " << rate << "%\n";
            cout << setprecision(2);
        } else {
            run = false;
        }
    } while (run);

    // 离开前把五个 fields 写回文件。
    ofstream fout(file.c_str());
    if (fout) {
        fout << fixed << setprecision(2)
                   << bank << '\n'
                   << game << '\n'
                   << wins << '\n'
                   << loss << '\n'
                   << push;
    } else {
        cout << "Could not save the file.\n";
    }
    fout.close();

    cout << "\nGoodbye, " << name << ".";
    return 0;
}