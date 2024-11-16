#pragma once

#include <string>
#include <vector>
#include <set>

enum class StageType {New, Register, Send};

struct StageInfo {
    StageType type;
    std::string main_actor;
    std::string other_actor; // needed for Send type

    StageInfo();
    StageInfo(StageType type, std::string actor_from, std::string actor_to);
};

class LogParser {
public:
    LogParser();
    void parse(std::string filename);
    std::vector<StageInfo>& getStages();
    std::set<std::string>& getActors();
private:
    std::vector<StageInfo> stages;
    std::set<std::string> actors;
};