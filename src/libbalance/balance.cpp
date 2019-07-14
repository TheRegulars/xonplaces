#include "balance.h"
#include <vector>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <string>
#include <sstream>
#include <climits>
#include <cassert>
#include <cstring>

const unsigned char* parse_players(const unsigned char *teams, player_list& players) {
    constexpr int STATE_MASK      = 0b0001;
    constexpr int FIRST_MASK      = 0b0010;
    constexpr int NEG_MASK        = 0b0100;
    constexpr int SEEN_DIGIT_MASK = 0b1000;
    constexpr int PREV_MAX = INT_MAX / 12;

    int prev = 0;
    int flags = FIRST_MASK;
    std::unordered_set<int> idset(256);
    struct player_s cur_player;

    while (*teams != '\0') {
        if (*teams == '-' && (flags & FIRST_MASK) == FIRST_MASK) {
            flags &= ~FIRST_MASK;
            flags |= NEG_MASK;
        } else if (*teams == '+' && (flags & FIRST_MASK) == FIRST_MASK) {
            flags &= ~FIRST_MASK;
        } else if (*teams == ':' && (flags & STATE_MASK) == 0) {
            // check invalid states
            if ((flags & FIRST_MASK) == FIRST_MASK || (flags & SEEN_DIGIT_MASK) == 0) {
                return teams;
            }
            cur_player.id = ((flags & NEG_MASK) == NEG_MASK) ? -prev : prev;
            auto search = idset.find(cur_player.id);

            // error: id isn't unique
            if (search != idset.end()) {
                return teams;
            }
            idset.insert(cur_player.id);
            flags = STATE_MASK | FIRST_MASK;
            prev = 0;
        } else if (*teams == ',' && (flags & STATE_MASK) == STATE_MASK) {
            // check invalid states
            if ((flags & FIRST_MASK) == FIRST_MASK || (flags & SEEN_DIGIT_MASK) == 0) {
                return teams;
            }

            cur_player.score = ((flags & NEG_MASK) == NEG_MASK) ? -prev : prev;
            players.push_back(cur_player);
            cur_player.id = 0;
            cur_player.score = 0;
            flags = FIRST_MASK;
            prev = 0;
        } else if (*teams >= '0' && *teams <= '9') {
            if (prev > PREV_MAX) {
                // error: integer overflow, value to big
                return teams;
            }
            prev = prev * 10 + (*teams - '0');
            flags |= SEEN_DIGIT_MASK;
            flags &= ~FIRST_MASK;
        } else if (*teams == ' ' || *teams == '\t') {
            // ignore, do nothing
        } else {
            // error: received invalid symbol
            return teams;
        }
        // reset first indicator
        teams++;
    }
    // save last part
    if ((flags & STATE_MASK) == STATE_MASK) {
        if ((flags & FIRST_MASK) == FIRST_MASK || (flags & SEEN_DIGIT_MASK) == 0) {
            return teams;
        }
        cur_player.score = ((flags & NEG_MASK) == NEG_MASK) ? -prev : prev;
        players.push_back(cur_player);
        return nullptr;
    } else if ((flags & STATE_MASK) == 0 && ((flags & FIRST_MASK) == FIRST_MASK)) {
        // allow ending with ,
        return nullptr;
    } else {
        return teams;
    }
}

const char* parse_players(const char *teams, player_list& players) {
    const unsigned char* uteams = reinterpret_cast<const unsigned char*>(teams);
    return reinterpret_cast<const char*>(parse_players(uteams, players));
}

// simple naive implementation but fast: O(n)
// this will be used only for very large teams
sorted_teams balance_fast(player_list& players, int teams) {
    assert(players.size() < 1024);
    assert(teams > 1 && teams <= static_cast<int>(players.size()));

    struct team_s {
        int team_id;
        int count = 0;
        int sum_score = 0;
    };
    std::sort(players.begin(), players.end(), [](auto a, auto b) {
        return a.score > b.score;
    });

    std::vector<struct team_s> team_list(teams);
    for (int i = 0 ; i < teams; i++) {
        team_list[i].team_id = i;
    }
    players_teams result(players.size());
    for (int i = 0; i < static_cast<int>(players.size()); i++) {
        auto &p = players[i];
        auto &r = result[i];
        auto &team = team_list.front();
        team.sum_score += p.score;
        team.count++;
        r.id = p.id;
        r.team_id = team.team_id;
        std::make_heap(team_list.begin(), team_list.end(), [](auto a, auto b) {
            return a.count > b.count || ((a.count == b.count) && (a.sum_score > b.sum_score));
        });
    }

    // sort and prepare result
    std::sort(team_list.begin(), team_list.end(), [](auto a, auto b) {
        return a.sum_score > b.sum_score;
    });
    std::vector<int> team_map(teams);
    for(int i = 0; i < teams ; i++) {
        team_map[team_list[i].team_id] = i;
    }

    sorted_teams final_result(teams);
    for (auto &player: result) {
        final_result[team_map[player.team_id]].push_back(player.id);
    }

    return final_result;
}


struct dynamic_balance {
    const player_list& players;

    dynamic_balance(const player_list& players) : players(players) {};

    std::pair<int, std::vector<int>> find_balance(int perfect_score, short min_team, short max_team) {
        assert(min_team <= max_team);
        assert(perfect_score > 0);

        // Heuristic of initial table size, to reduce amount of memory allocations
        int table_size = players.size() * players.size() * std::min(std::max(perfect_score >> 5, 4), 16);

        init_memoize(table_size);
        int result_score = build_path(perfect_score, 0, min_team, max_team);
        return std::make_pair(result_score, std::move(resolve_path(perfect_score, max_team)));
    }
    
    private:
    struct MemoizeHash {
        std::size_t operator() (std::tuple<int, unsigned short, short> const& s) const noexcept {
            int score, i;
            short max_team;
            std::tie(score, i, max_team) = s;
            return score ^ (static_cast<unsigned int>(i) | static_cast<unsigned int>(max_team) << 16);
        }
    };

    // memoization tables
    std::unordered_map<std::tuple<int, unsigned short, short>, int, MemoizeHash> memoize_table;
    std::unordered_set<std::tuple<int, unsigned short, short>, MemoizeHash> memoize_path;

    static constexpr int invalid_balance = -667 * 32;
    void init_memoize(int table_size) {
        memoize_table.reserve(table_size);
        memoize_path.reserve(table_size >> 2);
    }

    // TODO: support for set
    int build_path(int perfect_score, unsigned short i, short min_team, short max_team) {
        int result;

        if (max_team == 0) {
            return 0;
        }
        assert(max_team > 0);

        if (i >= players.size()) {
            if (min_team > 0) {
                // if team is smaller than min_team and no more players available then we made wrong choices
                return invalid_balance;
            } else {
                return 0;
            }
        }
        auto index = std::make_tuple(perfect_score, i, max_team);
        auto search = memoize_table.find(index);
        if (search != memoize_table.end()) {
            return search->second;
        }

        int first_option = build_path(perfect_score - players[i].score, i + 1, min_team - 1, max_team - 1);
        int second_option = build_path(perfect_score, i + 1, min_team, max_team);

        if (first_option != invalid_balance) {
            first_option += players[i].score;
        }

        // select correct option and set path
        if (first_option == invalid_balance && second_option == invalid_balance) {
            result = invalid_balance;
        } else if (first_option == invalid_balance) {
            result = second_option;
        } else if (second_option == invalid_balance) {
            memoize_path.insert(index);
            result = first_option;
        } else if (std::abs(first_option - perfect_score) < std::abs(second_option - perfect_score)) {
            memoize_path.insert(index);
            result = first_option;
        } else {
            result = second_option;
        }

        memoize_table[index] = result;
        return result;
    }

    std::vector<int> resolve_path(int score, short max_team) {
        std::vector<int> result;
        result.reserve(max_team);

        for (unsigned short i = 0; i < players.size() && max_team > 0; i++) {
            auto index = std::make_tuple(score, i, max_team);

            if (memoize_path.find(index) != memoize_path.end()) {
                result.push_back(players[i].id);
                score -= players[i].score;
                max_team--;
            }
        }
        return result;
    }
};


// slower but much more accurate, works only for 2 teams
sorted_teams balance_perfect2(const player_list& players) {
    // it works only for 2 teams
    constexpr int teams = 2;
    int team_score;
    assert(players.size() >= 2);
    sorted_teams result;
    std::vector<int> scores;
    std::unordered_set<int> filter_players(players.size() + players.size() / 3);
    result.reserve(teams);
    scores.reserve(teams);

    auto sum_fold = [](int s, struct player_s i){ return i.score + s; };
    int sum_score = std::accumulate(players.begin(), players.end(), 0, sum_fold);
    // TODO: this might be incorrect
    int perfect_score = (sum_score + (teams / 2)) / teams; // round(sum_score / teams);
    int min_team = players.size() / teams;
    int max_team = (players.size() + teams - 1) / teams; // ceil(players.size() / teams)
    std::vector<int> team;
    player_list remaining_players(players.size());

    dynamic_balance balance(players);
    std::tie(team_score, team) = balance.find_balance(perfect_score, min_team, max_team);
    std::copy(team.begin(), team.end(), std::inserter(filter_players, filter_players.begin()));
    scores.push_back(team_score);
    assert(team.size() >= min_team && team.size() <= max_team);
    result.push_back(std::move(team));

    team_score = 0;
    team.clear();
    for(auto &p : players) {
        if (filter_players.find(p.id) == filter_players.end()) {
            team.push_back(p.id);
            team_score += p.score;
        }
    }
    scores.push_back(team_score);
    result.push_back(std::move(team));
    // sort, there is only 2 teams
    if (scores[1] > scores[0]) {
        std::swap(result[1], result[0]);
    }
    return result;
}

int format_teams(const sorted_teams& teams, char *result, size_t max_size) {
    // this is slow but easy to read ;-)
    std::ostringstream os;
    bool first_team = true;
    for (auto &team: teams) {
        if (!first_team) {
            os << ";";
        }
        first_team = false;
        bool first_player = true;
        for (auto &player: team) {
            if (!first_player) {
                os << ",";
            }
            first_player = false;
            os << player;
        }
    }
    std::string output = std::move(os.str());
    if ((output.length() + 1) <= max_size) {
        std::memcpy(result, output.c_str(), output.length() + 1);
        return 0;
    } else {
        // result string to big
        return -4;
    }
}


/*
 * @team_players -- string that represent team ("1: 20, 2: 30": player id 1 have score 20, and
 *                  player with id 2 have score 30
 *
 * @teams -- number of teams (in most cases should be 2, in some cases could be 3 or 4)
 * @result -- address for result string
 * @max_size -- max size of result string including '\0' byte
 *
 * Result: on success returns 0, othersize returns error code
 */

extern "C" int team_balance(const char *team_players, int teams, char *result, size_t max_size) {
    player_list parsed_players;
    sorted_teams balanced;
    auto error = parse_players(team_players, parsed_players);
    if (error != nullptr) {
        // parsing error
        return -1;
    }
    if (teams < 2 || teams > 4) {
        // incorrect teams value
        return -2;
    }
    if (parsed_players.size() < teams) {
        // not enough players
        return -3;
    }

    if (teams == 2 && parsed_players.size() < 32) {
        balanced = balance_perfect2(parsed_players);
    } else {
        balanced = balance_fast(parsed_players, teams);
    }
    return format_teams(balanced, result, max_size);
}
