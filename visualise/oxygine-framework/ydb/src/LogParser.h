#pragma once

#include <string>
#include <vector>
#include <set>

enum class StageType {New, Register, Send};

struct LogActorInfo {
    std::string name;
    std::string activity_type;
    std::string type;

    LogActorInfo();
    LogActorInfo(std::string name, std::string activity_type, std::string type);
};

struct StageInfo {
    StageType type;
    std::string main_actor;
    std::string other_actor; // needed for Send type
    std::string info;

    StageInfo();
    StageInfo(StageType type, std::string actor_from, std::string actor_to, std::string info);
};

class LogParser {
public:
    LogParser();
    void parse(std::string filename);
    std::vector<StageInfo>& getStages();
    std::set<std::string>& getActors();
    std::vector<LogActorInfo>& getActorsInfo();
private:
    std::vector<StageInfo> stages;
    std::set<std::string> actors;
    std::vector<LogActorInfo> actor_info;
};