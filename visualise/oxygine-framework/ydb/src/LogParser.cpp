#include "LogParser.h"
#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

LogActorInfo::LogActorInfo() {
    name = "";
    activity_type = "";
    type = "";
    node_id = "";
}

LogActorInfo::LogActorInfo(std::string name, std::string activity_type, std::string type, std::string node_id) {
    this->name = name;
    this->activity_type = activity_type;
    this->type = type;
    this->node_id = node_id;
}

LogParser::~LogParser() {
    for (std::size_t i = 0; i < stages.size(); ++i) {
        delete stages[i];
    }
}

void BinaryLogParser::read_types(std::ifstream& fin, std::vector<std::string>& data) {
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

std::string BinaryLogParser::read_actor_id(std::ifstream& fin) {
    std::uint32_t node_id;
    std::uint64_t local_id;
    std::uint32_t hint;
    fin.read(reinterpret_cast<char*>(&node_id), sizeof(node_id));
    fin.read(reinterpret_cast<char*>(&local_id), sizeof(local_id));
    fin.read(reinterpret_cast<char*>(&hint), sizeof(hint));
    return "[" + std::to_string(node_id) + ':' + std::to_string(local_id) + ':' + std::to_string(hint) + ']';
}

void TextLogParser::parse(std::string filename) {
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
                actor_info.push_back(LogActorInfo(actor_from, activity_type, activity_type, activity_type));
                stages.push_back(new NewStageInfo{actor_from, "", "", ""});
            }
            if (actors.find(actor_to) == actors.end()) {
                actors.insert(actor_to);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor_to, activity_type, activity_type, activity_type));
                stages.push_back(new NewStageInfo{actor_to, "", "", ""});
            }
            stages.push_back(new SendStageInfo{actor_from, actor_to, send_type});
        }
        else if (type == "Register") {
            std::string actor;
            fin >> actor;
            if (actors.find(actor) == actors.end()) {
                actors.insert(actor);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor, activity_type, activity_type, activity_type));
                stages.push_back(new NewStageInfo{actor, "", "", ""});
            }
            stages.push_back(new RegisterStageInfo{actor});
        }
        else if (type == "New") {
            std::string actor, activity_type, type, node_id;
            fin >> actor >> activity_type >> type >> node_id;
            actors.insert(actor);
            actor_info.push_back(LogActorInfo(actor, activity_type, type, node_id));
            stages.push_back(new NewStageInfo{actor, "", "", ""});
        }
    }
}

void BinaryLogParser::parse(std::string filename) {
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
        std::string type = enumerator.type_number_to_str(type_idx);
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
                actor_info.push_back(LogActorInfo(actor, activity_type, activity_type, activity_type));
                stages.push_back(new NewStageInfo{actor, "", "", ""});
            }
            if (actors.find(actor_to) == actors.end()) {
                actors.insert(actor_to);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor_to, activity_type, activity_type, activity_type));
                stages.push_back(new NewStageInfo{actor_to, "", "", ""});
            }
            stages.push_back(new SendStageInfo{actor, actor_to, send_type});
        }
        else if (type == "Register") {
            if (actors.find(actor) == actors.end()) {
                actors.insert(actor);
                std::string activity_type = "Unknown";
                actor_info.push_back(LogActorInfo(actor, activity_type, activity_type, activity_type));
                stages.push_back(new NewStageInfo{actor, "", "", ""});
            }
            stages.push_back(new RegisterStageInfo{actor});
        }
        else if (type == "New") {
            std::string activity_type, actor_type;
            unsigned char activity_type_idx, actor_type_idx;
            std::uint32_t node_id;
            fin.read(reinterpret_cast<char*>(&activity_type_idx), sizeof(activity_type_idx));
            fin.read(reinterpret_cast<char*>(&actor_type_idx), sizeof(actor_type_idx));
            fin.read(reinterpret_cast<char*>(&node_id), sizeof(node_id));
            activity_type = activity_types[activity_type_idx];
            actor_type = actor_types[activity_type_idx];
            actors.insert(actor);
            actor_info.push_back(LogActorInfo(actor, activity_type, actor_type, std::to_string(node_id)));
            stages.push_back(new NewStageInfo{actor, "", "", ""});
        }
    }
}

std::vector<StageInfo*>& LogParser::getStages() {
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
    for (std::size_t i = 0; i < operations.size(); ++i) {
        delete operations[i];
    }
}

BinaryLogWriter::~BinaryLogWriter() {
    write_file();
}

TextLogWriter::~TextLogWriter() {
    write_file();
}

void TextLogWriter::add_operation(StageInfo* op) {
    if (static_cast<int>(operations.size()) >= max_operations) {
        return;
    }
    operations.push_back(op);
}

void BinaryLogWriter::add_operation(StageInfo* op) {
    if (static_cast<int>(operations.size()) >= max_operations) {
        return;
    }
    operations.push_back(op);
    op->accept(&adder);
}

void BinaryLogWriter::write_map(std::ofstream& fout, std::unordered_map<std::string, unsigned char>& data) {
    unsigned char types_cnt = data.size();
    fout.write(reinterpret_cast<char*>(&types_cnt), sizeof(types_cnt));
    std::vector<std::pair<unsigned char, std::string>> vec_data;
    vec_data.reserve(types_cnt);
    for (auto type: data) {
        vec_data.push_back({type.second, type.first});
    }
    sort(vec_data.begin(), vec_data.end());
    for (auto type: vec_data) {
        unsigned short type_length = type.second.size();
        fout.write(reinterpret_cast<char*>(&type_length), sizeof(type_length));
        fout.write(type.second.c_str(), type_length);
    }
}

BinaryLogWriter::hash_values BinaryLogWriter::parse_actor_id(std::string actor_id) {
    int first_delimeter = actor_id.find(':');
    int last_delimeter = actor_id.find(':');
    std::string node_id = actor_id.substr(1, first_delimeter - 1);
    std::string local_id = actor_id.substr(first_delimeter + 1, last_delimeter - first_delimeter - 1);
    std::string hint = actor_id.substr(last_delimeter + 1, actor_id.size() - 1 - last_delimeter);
    return {{std::atoi(node_id.c_str()), std::atoll(local_id.c_str())}, std::atoi(hint.c_str())};
}

void TextLogWriter::write_file() {
    std::ofstream fout(filename);
    writer.set_file_out(&fout);
    for (auto op: operations) {
        op->accept(&writer);
    }
}

void BinaryLogWriter::write_file() {
    std::ofstream fout(filename, std::ios::binary);

    writer.set_file_out(&fout);

    write_map(fout, actor_types);
    write_map(fout, activity_types);
    write_map(fout, send_types);
    std::uint32_t operations_cnt = operations.size();
    fout.write(reinterpret_cast<char*>(&operations_cnt), sizeof(operations_cnt));
    for (auto op: operations) {
        BinaryLogWriter::hash_values actor_hash = parse_actor_id(op->main_actor);
        op->accept(&enumerator);
        unsigned char stage_type = enumerator.get_number();
        fout.write(reinterpret_cast<char*>(&stage_type), sizeof(stage_type));
        std::uint32_t node_id = actor_hash.first.first;
        std::uint64_t local_id = actor_hash.first.second;
        std::uint32_t hint = actor_hash.second;
        fout.write(reinterpret_cast<char*>(&node_id), sizeof(node_id));
        fout.write(reinterpret_cast<char*>(&local_id), sizeof(local_id));
        fout.write(reinterpret_cast<char*>(&hint), sizeof(hint));
        op->accept(&writer);
    }
    fout.close();

}

BinaryLogAddVisitor::BinaryLogAddVisitor(BinaryLogWriter* logger) {
    this->logger = logger;
}

void BinaryLogAddVisitor::visit(NewStageInfo* op) {
    if (logger->actor_types.find(op->actor_type) == logger->actor_types.end()) {
        logger->actor_types[op->actor_type] = logger->actor_types.size();
    }
    if (logger->activity_types.find(op->activity_type) == logger->activity_types.end()) {
        logger->activity_types[op->activity_type] = logger->activity_types.size();
    }
}


void BinaryLogAddVisitor::visit(RegisterStageInfo*) {}


void BinaryLogAddVisitor::visit(SendStageInfo* op) {
    if (logger->send_types.find(op->type) == logger->send_types.end()) {
        logger->send_types[op->type] = logger->send_types.size();
    }
}

BinaryWriteVisitor::BinaryWriteVisitor(BinaryLogWriter* logger) {
    this->logger = logger;
}


void BinaryWriteVisitor::set_file_out(std::ofstream* fout) {
    this->fout = fout;
}

void BinaryWriteVisitor::visit(NewStageInfo* op) {
    unsigned char activity_type = logger->activity_types[op->activity_type];
    fout->write(reinterpret_cast<char*>(&activity_type), sizeof(activity_type));
    unsigned char actor_type = logger->actor_types[op->actor_type];
    fout->write(reinterpret_cast<char*>(&actor_type), sizeof(actor_type));
    std::uint32_t node_id = std::atoi(op->node_id.c_str());
    fout->write(reinterpret_cast<char*>(&node_id), sizeof(node_id));
}


void BinaryWriteVisitor::visit(RegisterStageInfo*) {}


void BinaryWriteVisitor::visit(SendStageInfo* op) {
    BinaryLogWriter::hash_values other_actor_hash = logger->parse_actor_id(op->other_actor);
    std::uint32_t other_node_id = other_actor_hash.first.first;
    std::uint64_t other_local_id = other_actor_hash.first.second;
    std::uint32_t other_hint = other_actor_hash.second;
    fout->write(reinterpret_cast<char*>(&other_node_id), sizeof(other_node_id));
    fout->write(reinterpret_cast<char*>(&other_local_id), sizeof(other_local_id));
    fout->write(reinterpret_cast<char*>(&other_hint), sizeof(other_hint));
    unsigned char send_type = logger->send_types[op->type];
    fout->write(reinterpret_cast<char*>(&send_type), sizeof(send_type));
}

TextWriteVisitor::TextWriteVisitor(TextLogWriter* logger) {
    this->logger = logger;
}


void TextWriteVisitor::set_file_out(std::ofstream* fout) {
    this->fout = fout;
}

void TextWriteVisitor::visit(NewStageInfo* op) {
    *fout << "New " << op->main_actor << ' ' << op->actor_type << ' ' << op->activity_type << ' ' << op->node_id << std::endl;
}


void TextWriteVisitor::visit(RegisterStageInfo* op) {
    *fout << "Register " << op->main_actor << std::endl;
}


void TextWriteVisitor::visit(SendStageInfo* op) {
    *fout << "Send " << op->main_actor << ' ' << op->other_actor << ' ' << op->type << std::endl;
}