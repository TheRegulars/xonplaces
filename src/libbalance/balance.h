#ifndef LIB_BALANCE_H
#define LIB_BALANCE_H
#include  <stddef.h>

struct player_s {
    int id;
    int score;
};

struct player_team_s {
    int id;
    int team_id;
};

#ifdef __cplusplus
// c++ externs here
#include <vector>
using player_list = std::vector<struct player_s>;
using players_teams = std::vector<struct player_team_s>;
using sorted_teams = std::vector<std::vector<int>>;

const unsigned char* parse_players(const unsigned char *, player_list&);
const char* parse_players(const char *teams, player_list& players);
sorted_teams balance_fast(player_list& players, int teams);
sorted_teams balance_perfect2(const player_list& players);
int format_teams(const sorted_teams& teams, char *result, size_t max_size);

extern "C" {
#endif
// c externs here
int team_balance(const char *team_players, int teams, char *result, size_t max_size);

#ifdef __cplusplus
}
#endif

#endif
