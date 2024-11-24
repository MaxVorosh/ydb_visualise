#include "LogParser.h"
#include <fstream>
#include <set>
#include <vector>
#include <string>

LogActorInfo::LogActorInfo() {
    name = "";
    activity_type = "";
}

LogActorInfo::LogActorInfo(std::string name, std::string activity_type) {
    this->name = name;
    this->activity_type = activity_type;
}

StageInfo::StageInfo() {
    type = StageType::New;
    main_actor = "";
    other_actor = "";
    info = "";
}

StageInfo::StageInfo(StageType type, std::string actor_from, std::string actor_to, std::string info) {
    this->type = type;
    main_actor = actor_from;
    other_actor = actor_to;
    this->info = info;
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
            std::string actor_from, actor_to, send_type;
            fin >> actor_from >> actor_to >> send_type;
            if (actors.find(actor_from) == actors.end()) {
                actors.insert(actor_from);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor_from, activity_type));
                stages.push_back(StageInfo(StageType::New, actor_from, "", activity_type));
            }
            if (actors.find(actor_to) == actors.end()) {
                actors.insert(actor_to);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor_to, activity_type));
                stages.push_back(StageInfo(StageType::New, actor_to, "", activity_type));
            }
            stages.push_back(StageInfo(StageType::Send, actor_from, actor_to, send_type));
        }
        else if (type == "Register") {
            std::string actor;
            fin >> actor;
            stages.push_back(StageInfo(StageType::Register, actor, "", ""));
        }
        else if (type == "New") {
            std::string actor, activity_type;
            fin >> actor >> activity_type;
            actors.insert(actor);
            actor_info.push_back(LogActorInfo(actor, activity_type));
            stages.push_back(StageInfo(StageType::New, actor, "", activity_type));
        }
    }
}

std::vector<StageInfo>& LogParser::getStages() {
    return stages;
}

std::set<std::string>& LogParser::getActors() {
    return actors;
}

std::vector<LogActorInfo>& LogParser::getActorsInfo() {
    return actor_info;
}