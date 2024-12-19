#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

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
    LogWriter(std::string filename, int max_operations);
    virtual ~LogWriter() = default;
    virtual void add_operation(StageInfo op) = 0;
protected:
    std::vector<StageInfo> operations;
    std::string filename;
    int max_operations;

    virtual void write_file() = 0;
};

class TextLogWriter: public LogWriter {
public:
    TextLogWriter(std::string filename, int max_operations): LogWriter(filename, max_operations) {};
    void add_operation(StageInfo op) override;
    ~TextLogWriter();
protected:
    void write_file() override;
};

class BinaryLogWriter: public LogWriter {
public:
    BinaryLogWriter(std::string filename, int max_operations): LogWriter(filename, max_operations) {};
    void add_operation(StageInfo op) override;
    ~BinaryLogWriter();
protected:
    std::unordered_map<std::string, unsigned char> activity_types;
    std::unordered_map<std::string, unsigned char> actor_types;
    std::unordered_map<std::string, unsigned char> send_types;

    void write_file() override;
    void write_map(std::ofstream& fout, std::unordered_map<std::string, unsigned char>& data);
    unsigned char type_to_int(StageType type);
    
    using hash_values = std::pair<std::pair<std::uint32_t, std::uint64_t>, std::uint32_t>;
    hash_values parse_actor_id(std::string actor_id);
};