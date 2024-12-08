#include "LogParser.h"
#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <iostream>

LogActorInfo::LogActorInfo() {
    name = "";
    activity_type = "";
    type = "";
}

LogActorInfo::LogActorInfo(std::string name, std::string activity_type, std::string type) {
    this->name = name;
    this->activity_type = activity_type;
    this->type = type;
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

void LogParser::read_types(std::ifstream& fin, std::vector<std::string>& data) {
    unsigned char size;
    fin.read(reinterpret_cast<char*>(&size), sizeof(size));
    data.resize(size);
    for (unsigned char i = 0; i < size; ++i) {
        unsigned short length;
        fin.read(reinterpret_cast<char*>(&length), sizeof(length));
        char str_data[length + 1];
        fin.read(str_data, length);
        str_data[length] = 0;
        data[i] = str_data;
    }
}

std::string LogParser::read_actor_id(std::ifstream& fin) {
    std::uint32_t node_id;
    std::uint64_t local_id;
    std::uint32_t hint;
    fin.read(reinterpret_cast<char*>(&node_id), sizeof(node_id));
    fin.read(reinterpret_cast<char*>(&local_id), sizeof(local_id));
    fin.read(reinterpret_cast<char*>(&hint), sizeof(hint));
    return "[" + std::to_string(node_id) + ':' + std::to_string(local_id) + ':' + std::to_string(hint) + ']';
}

void LogParser::parse(std::string filename) {
    stages.clear();
    actors.clear();
    std::ifstream fin(filename, std::ios::binary);
    if (!fin.is_open()) {
        return;
    }
    std::vector<std::string> activity_types;
    std::vector<std::string> actor_types;
    std::vector<std::string> send_types;
    read_types(fin, actor_types);
    read_types(fin, activity_types);
    read_types(fin, send_types);
    std::uint32_t operations_cnt;
    fin.read(reinterpret_cast<char*>(&operations_cnt), sizeof(operations_cnt));
    for (std::uint32_t i = 0; i < operations_cnt; ++i) {
        unsigned char type_idx;
        fin.read(reinterpret_cast<char*>(&type_idx), sizeof(type_idx));
        std::string type = types[type_idx];
        std::string actor = read_actor_id(fin);
        if (type == "Send") {
            std::string actor_to, send_type;
            actor_to = read_actor_id(fin);
            unsigned char send_type_idx;
            fin.read(reinterpret_cast<char*>(&send_type_idx), sizeof(send_type_idx));
            send_type = send_types[send_type_idx];
            if (actors.find(actor) == actors.end()) {
                actors.insert(actor);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor, activity_type, activity_type));
                stages.push_back(StageInfo(StageType::New, actor, "", ""));
            }
            if (actors.find(actor_to) == actors.end()) {
                actors.insert(actor_to);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor_to, activity_type, activity_type));
                stages.push_back(StageInfo(StageType::New, actor_to, "", ""));
            }
            stages.push_back(StageInfo(StageType::Send, actor, actor_to, send_type));
        }
        else if (type == "Register") {
            if (actors.find(actor) == actors.end()) {
                actors.insert(actor);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor, activity_type, activity_type));
                stages.push_back(StageInfo(StageType::New, actor, "", ""));
            }
            stages.push_back(StageInfo(StageType::Register, actor, "", ""));
        }
        else if (type == "New") {
            std::string activity_type, actor_type;
            unsigned char activity_type_idx, actor_type_idx;
            fin.read(reinterpret_cast<char*>(&activity_type_idx), sizeof(activity_type_idx));
            fin.read(reinterpret_cast<char*>(&actor_type_idx), sizeof(actor_type_idx));
            activity_type = activity_types[activity_type_idx];
            actor_type = actor_types[activity_type_idx];
            actors.insert(actor);
            actor_info.push_back(LogActorInfo(actor, activity_type, actor_type));
            stages.push_back(StageInfo(StageType::New, actor, "", ""));
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

LogWriter::LogWriter(std::string filename, int max_operations) {
    this->filename = filename;
    this->max_operations = max_operations;
}

LogWriter::~LogWriter() {
    write_file();
}

void LogWriter::add_operation(StageInfo op) {
    if (max_operations <= static_cast<int>(operations.size())) {
        return;
    }
    operations.push_back(op);
    if (op.type == StageType::New) {
        // Assuming other_actor contains actor_type, info contains activity_type
        if (actor_types.find(op.other_actor) == actor_types.end()) {
            actor_types[op.other_actor] = actor_types.size();
        }
        if (activity_types.find(op.info) == activity_types.end()) {
            activity_types[op.info] = activity_types.size();
        }
    }
    if (op.type == StageType::Send) {
        // Assuming info contains send_type
        if (send_types.find(op.info) == send_types.end()) {
            send_types[op.info] = send_types.size();
        }
    }
}

void LogWriter::write_map(std::ofstream& fout, std::unordered_map<std::string, unsigned char>& data) {
    unsigned char types_cnt = data.size();
    fout.write(reinterpret_cast<char*>(&types_cnt), sizeof(types_cnt));
    for (auto type: data) {
        unsigned short type_length = type.first.size();
        fout.write(reinterpret_cast<char*>(&type_length), sizeof(type_length));
        fout.write(type.first.c_str(), type_length);
    }
}

LogWriter::hash_values LogWriter::parse_actor_id(std::string actor_id) {
    int first_delimeter = actor_id.find(':');
    int last_delimeter = actor_id.find(':');
    std::string node_id = actor_id.substr(1, first_delimeter - 1);
    std::string local_id = actor_id.substr(first_delimeter + 1, last_delimeter - first_delimeter - 1);
    std::string hint = actor_id.substr(last_delimeter + 1, actor_id.size() - 1 - last_delimeter);
    return {{std::atoi(node_id.c_str()), std::atoll(local_id.c_str())}, std::atoi(hint.c_str())};
}

unsigned char LogWriter::type_to_int(StageType type) {
    if (type == StageType::New) {
        return 0;
    }
    if (type == StageType::Register) {
        return 1;
    }
    if (type == StageType::Send) {
        return 2;
    }
    return 3;
}

void LogWriter::write_file() {
    std::ofstream fout(filename, std::ios::binary);
    write_map(fout, actor_types);
    write_map(fout, activity_types);
    write_map(fout, send_types);
    std::uint32_t operations_cnt = operations.size();
    fout.write(reinterpret_cast<char*>(&operations_cnt), sizeof(operations_cnt));
    for (auto op: operations) {
        LogWriter::hash_values actor_hash = parse_actor_id(op.main_actor);
        unsigned char stage_type = type_to_int(op.type);
        fout.write(reinterpret_cast<char*>(&stage_type), sizeof(stage_type));
        std::uint32_t node_id = actor_hash.first.first;
        std::uint64_t local_id = actor_hash.first.second;
        std::uint32_t hint = actor_hash.second;
        fout.write(reinterpret_cast<char*>(&node_id), sizeof(node_id));
        fout.write(reinterpret_cast<char*>(&local_id), sizeof(local_id));
        fout.write(reinterpret_cast<char*>(&hint), sizeof(hint));
        if (op.type == StageType::New) {
            // Assuming other_actor contains actor_type, info contains activity_type
            unsigned char activity_type = activity_types[op.info];
            fout.write(reinterpret_cast<char*>(&activity_type), sizeof(activity_type));
            unsigned char actor_type = actor_types[op.other_actor];
            fout.write(reinterpret_cast<char*>(&actor_type), sizeof(actor_type));
        }
        if (op.type == StageType::Send) {
            // Assuming info contains send_type
            LogWriter::hash_values other_actor_hash = parse_actor_id(op.other_actor);
            std::uint32_t other_node_id = other_actor_hash.first.first;
            std::uint64_t other_local_id = other_actor_hash.first.second;
            std::uint32_t other_hint = other_actor_hash.second;
            fout.write(reinterpret_cast<char*>(&other_node_id), sizeof(other_node_id));
            fout.write(reinterpret_cast<char*>(&other_local_id), sizeof(other_local_id));
            fout.write(reinterpret_cast<char*>(&other_hint), sizeof(other_hint));
            unsigned char send_type = send_types[op.info];
            fout.write(reinterpret_cast<char*>(&send_type), sizeof(send_type));
        }
    }
    fout.close();

}