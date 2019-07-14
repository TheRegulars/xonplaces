#include "balance.h"
#include <gtest/gtest.h>
#include <numeric>
#include <random>
#include <vector>
#include <unordered_map>
#include <iostream>

#define test_parse_error(STR) { \
    player_list parsed_players; \
    auto error = parse_players(STR, parsed_players); \
    EXPECT_NE(error, nullptr); \
}

#define test_parse_ok(STR) { \
    player_list parsed_players; \
    auto error = parse_players(STR, parsed_players); \
    EXPECT_EQ(error, nullptr); \
}


std::vector<int> calculate_team_scores(sorted_teams teams, const player_list& players) {
    std::unordered_map<int, int> player_scores(players.size() * 1.2);
    std::vector<int> result(teams.size());

    for (const auto &p: players) {
        player_scores[p.id] = p.score;
    }

    for(size_t i = 0; i < teams.size(); i++) {
        result[i] = std::accumulate(teams[i].begin(), teams[i].end(), 0, [&player_scores](int sum, int id) {
            return player_scores[id] + sum;
        });
    }

    return result;
}

float team_score(std::vector<int> sums, int perfect_score) {
    int acc = 0;
    for (int s: sums) {
        int v = perfect_score - s;
        acc += v * v;
    }

    return acc / sums.size();
}

int calc_perfect_score(const player_list& players, int teams) {
    int acc = std::accumulate(players.begin(), players.end(), 0, [](int acc, auto p) {
        return p.score + acc;
    });
    return (acc + (teams / 2)) / teams; // round(acc / teams)
}

TEST(ParsePlayersTest, GoodInput) {
    player_list parsed_players;
    auto error = parse_players("10: 129, 21: 3030, 15: -1, 42: 15", parsed_players);
    ASSERT_EQ(error, nullptr);
    // 10: 129
    EXPECT_EQ(parsed_players[0].id, 10);
    EXPECT_EQ(parsed_players[0].score, 129);
    // 21: 3030
    EXPECT_EQ(parsed_players[1].id, 21);
    EXPECT_EQ(parsed_players[1].score, 3030);
    // 15: -1 
    EXPECT_EQ(parsed_players[2].id, 15);
    EXPECT_EQ(parsed_players[2].score, -1);
    // 42:  15
    EXPECT_EQ(parsed_players[3].id, 42);
    EXPECT_EQ(parsed_players[3].score, 15);
}

TEST(ParsePlayersTest, BadInput) {

    test_parse_error("1: 2, 2,")
    test_parse_error("1: 2:");
    test_parse_error("1: 2:");
    test_parse_error("1: 2: ");
    test_parse_error("1: 2,,");
    test_parse_error("1:: 2");
    test_parse_error("1: 2, 2 3");
    test_parse_error("1: 2, 2, 3;");
    test_parse_error("1, 2,");
    test_parse_error("1: 2, 2");
    test_parse_error("1: 2, 2:");
    test_parse_error("1: 2, 2: ");
    test_parse_error("1: 222222222222222222222222,"); // integer overflow
    test_parse_error("9999999999999999: 2,"); // integer overflow
    test_parse_error("1: 2, 1: 4"); // id's should be unique
}

TEST(ParsePlayersTest, GoodInput2) {

    test_parse_ok("1: 2")
    test_parse_ok("1: 2,")
    test_parse_ok("1:  2,")
    test_parse_ok("1:  2, 2: 3")
    test_parse_ok("1:  2, 2:    3,")
    test_parse_ok("1:  2, 2:    -3")
}

TEST(BalanceFast, NormalInput) {
    player_list parsed_players;
    sorted_teams teams;
    parse_players("1: 10, 2: 10, 3: 10, 4: 10", parsed_players);
    teams = balance_fast(parsed_players, 2);
    EXPECT_EQ(teams[0].size(), teams[1].size());

    parsed_players.clear();
    parse_players("1: 10, 2: 10, 3: 10", parsed_players);
    teams = balance_fast(parsed_players, 2);
    EXPECT_GT(teams[0].size(), teams[1].size());

    parsed_players.clear();
    parse_players("1: 10, 2: 10, 3: 10, 4: 10, 5: 10, 6: 10", parsed_players);
    teams = balance_fast(parsed_players, 3);
    EXPECT_EQ(teams[0].size(), teams[1].size());
    EXPECT_EQ(teams[1].size(), teams[2].size());
}

TEST(BalancePerf2, PerfectInput) {
    player_list parsed_players;
    sorted_teams teams;
    parse_players("1: 100, 2: 10, 3: 5, 4: 15, 5: 90, 6: 10", parsed_players);
    teams = balance_perfect2(parsed_players);

    EXPECT_EQ(teams[0].size(), 3);
    EXPECT_EQ(teams[0].size(), teams[1].size());

    std::vector<int> sums = calculate_team_scores(teams, parsed_players);
    EXPECT_EQ(sums[0], 115);
    EXPECT_EQ(sums[1], 115);
}

TEST(BalancePerf2, Issue1) {
    player_list parsed_players;
    sorted_teams teams;
    parse_players("1:96,2:381,3:484,4:166,5:260", parsed_players);
    teams = balance_perfect2(parsed_players);
    EXPECT_GE(teams[0].size(), 2);
    EXPECT_GE(teams[1].size(), 2);
    std::vector<int> sums = calculate_team_scores(teams, parsed_players);
    EXPECT_GE(sums[0], sums[1]);
}

TEST(BalancePerf2, Issue2) {
    player_list parsed_players;
    sorted_teams teams;
    parse_players("1:319,2:147,3:11,4:466,5:247,6:189", parsed_players);
    teams = balance_perfect2(parsed_players);
    EXPECT_GE(teams[0].size(), 2);
    EXPECT_GE(teams[1].size(), 2);
    std::vector<int> sums = calculate_team_scores(teams, parsed_players);
    EXPECT_GE(sums[0], sums[1]);
}

/*
TEST(BalanceGood, Issue3) {
    player_list parsed_players;
    parse_players("5:553,2:429,4:413,1:401,7:75,3:48,6:42,8:30", parsed_players);
    sorted_teams teams = balance_perfect2(parsed_players, 3);
    sorted_teams fast_teams = balance_fast(parsed_players, 3);
    std::vector<int> sums1 = calculate_team_scores(teams, parsed_players);
    std::vector<int> sums2 = calculate_team_scores(fast_teams, parsed_players);
    for(int c: sums1) {
        std::cerr << c << ",";
    }
    std::cerr << std::endl;
    for(int c: sums2) {
        std::cerr << c << ",";
    }
    std::cerr << std::endl;
}
*/

TEST(BalancePerf2, ShouldBeGood) {
    // property based test
    // balance_perfect2 should be as good as balance_fast or better
    player_list players;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> players_size_dst(2, 12);
    std::uniform_int_distribution<int> players_score_dst(5, 600);
    players.reserve(32);

    for (int c = 0; c < 8000; c++ ) {
        players.clear();
        int players_size = players_size_dst(gen);
        for (int i = 0; i < players_size; i++) {
            players.push_back({.id = i + 1, .score = players_score_dst(gen)});
        }
        sorted_teams teams_good = balance_perfect2(players);
        sorted_teams teams_fast = balance_fast(players, 2);

        EXPECT_GE(teams_good[0].size(), players_size / 2);
        EXPECT_GE(teams_good[1].size(), players_size / 2);
        EXPECT_GE(teams_fast[0].size(), players_size / 2);
        EXPECT_GE(teams_fast[1].size(), players_size / 2);

        std::vector<int> sums_good = calculate_team_scores(teams_good, players);
        std::vector<int> sums_fast = calculate_team_scores(teams_fast, players);

        // returned teams are sorted in desceding order
        EXPECT_GE(sums_good[0], sums_good[1]);
        EXPECT_GE(sums_fast[0], sums_fast[1]);
        int perfect_score = calc_perfect_score(players, 2);
        float score1 = team_score(sums_good, perfect_score);
        float score2 = team_score(sums_fast, perfect_score);
        EXPECT_LE(score1, score2);
    }
}

/*
TEST(BalanceGood, ShouldBeGood2) {
    // property based test
    // balance_good should be as good as balance_fast or better
    player_list players;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> team_size_dst(2, 6);
    std::uniform_int_distribution<int> players_score_dst(5, 600);
    players.reserve(32);

    // check with more than 2 teams
    for (int c = 0; c < 100; c++ ) {
        players.clear();
        int teams = team_size_dst(gen);
        std::uniform_int_distribution<int> players_size_dst(teams * 2, 16);
        int players_size = players_size_dst(gen);
        for (int i = 0; i < players_size; i++) {
            players.push_back({.id = i + 1, .score = players_score_dst(gen)});
        }

        sorted_teams teams_good = balance_good(players, teams);
        sorted_teams teams_fast = balance_fast(players, teams);

        float score1 = rate_team(teams_good, players);
        float score2 = rate_team(teams_fast, players);
        if (score1 > score2) {
            for (auto &p: players) {
                std::cerr << p.id << ":" << p.score << ",";
            }
            std::cerr << std::endl;
        }
        EXPECT_LE(score1, score2);
    }
    
}
*/

TEST(FormatTeams, Test1) {
    char output[32];
    sorted_teams teams {
        {1, 2, 3},
        {4, 5, 6}
    };
    format_teams(teams, output, sizeof(output));
    // it easier to parse without spaces
    EXPECT_STREQ(output, "1,2,3;4,5,6");
}

TEST(FormatTeams, Test2) {
    char output[10];
    sorted_teams teams {
        {1, 2, 3, 4},
        {5, 6, 7, 8},
    };
    int error = format_teams(teams, output, sizeof(output));
    // error, not enough space
    EXPECT_NE(error, 0);
}

TEST(FormatTeams, Test3) {
    char output[7];
    sorted_teams teams {
        {1, 2,},
        {3, 4,},
    };
    int error = format_teams(teams, output, sizeof(output));
    // error, not enough space
    EXPECT_NE(error, 0);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
