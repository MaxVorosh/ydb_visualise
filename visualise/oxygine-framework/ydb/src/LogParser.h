#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

enum class StageType {New, Register, Send};
enum class LogType {Text, Binary};

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
    virtual void parse(std::string filename) = 0;
    std::vector<StageInfo>& getStages();
    std::set<std::string>& getActors();
    std::vector<LogActorInfo>& getActorsInfo();
protected:
    std::vector<StageInfo> stages;
    std::set<std::string> actors;
    std::vector<LogActorInfo> actor_info;
};

class TextLogParser: public LogParser {
public:
    void parse(std::string filename) override;
};

class BinaryLogParser: public LogParser {
public:
    void parse(std::string filename) override;
private:
    void read_types(std::ifstream& fin, std::vector<std::string>& data);
    std::string read_actor_id(std::ifstream& fin);
    const std::string types[4] = {"New", "Register", "Send", "Other"};
};


class LogWriter {
public:
    LogWriter(std::string filename, int max_operations, LogType type);
    ~LogWriter();
    void add_operation(StageInfo op);
private:
    std::vector<StageInfo> operations;
    std::unordered_map<std::string, unsigned char> activity_types;
    std::unordered_map<std::string, unsigned char> actor_types;
    std::unordered_map<std::string, unsigned char> send_types;
    std::string filename;
    int max_operations;
    LogType type;

    void write_file();
    void write_map(std::ofstream& fout, std::unordered_map<std::string, unsigned char>& data);
    unsigned char type_to_int(StageType type);
    
    using hash_values = std::pair<std::pair<std::uint32_t, std::uint64_t>, std::uint32_t>;
    hash_values parse_actor_id(std::string actor_id);

    void add_operation_binary(StageInfo op);
    void add_operation_text(StageInfo op);

    void write_file_binary();
    void write_file_text();
};