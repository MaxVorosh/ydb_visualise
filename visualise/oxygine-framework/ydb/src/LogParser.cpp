#include "LogParser.h"
#include <fstream>
#include <set>
#include <vector>
#include <string>

StageInfo::StageInfo() {
    type = StageType::New;
    main_actor = "";
    other_actor = "";
}

StageInfo::StageInfo(StageType type, std::string actor_from, std::string actor_to) {
    this->type = type;
    main_actor = actor_from;
    other_actor = actor_to;
}

LogParser::LogParser() {
}

void LogParser::parse(std::string filename) {
    stages.clear();
    actors.clear();
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        return;
    }
    std::string type;
    while (fin >> type) {
        if (type == "Send") {
            std::string actor_from, actor_to, comments;
            fin >> actor_from >> actor_to >> comments;
            if (actors.find(actor_from) == actors.end()) {
                actors.insert(actor_from);
                stages.push_back(StageInfo(StageType::New, actor_from, ""));
            }
            if (actors.find(actor_to) == actors.end()) {
                actors.insert(actor_to);
                stages.push_back(StageInfo(StageType::New, actor_to, ""));
            }
            stages.push_back(StageInfo(StageType::Send, actor_from, actor_to));
        }
        else if (type == "Register") {
            std::string actor;
            fin >> actor;
            stages.push_back(StageInfo(StageType::Register, actor, ""));
        }
    }
}

std::vector<StageInfo>& LogParser::getStages() {
    return stages;
}

std::set<std::string>& LogParser::getActors() {
    return actors;
}