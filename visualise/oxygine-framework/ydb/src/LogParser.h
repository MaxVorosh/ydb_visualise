#pragma once

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

#include "StageInfo.h"

class BinaryLogWriter;
class TextLogWriter;

class BinaryLogAddVisitor: public StageInfoVisitor {
public:
    BinaryLogAddVisitor() = default;
    BinaryLogAddVisitor(BinaryLogWriter* logger);
    void visit(NewStageInfo* op) override;
    void visit(RegisterStageInfo* op) override;
    void visit(SendStageInfo* op) override;
private:
    BinaryLogWriter* logger;
};

class BinaryWriteVisitor: public StageInfoVisitor {
public:
    BinaryWriteVisitor() = default;
    BinaryWriteVisitor(BinaryLogWriter* logger);
    void set_file_out(std::ofstream* fout);
    void visit(NewStageInfo* op) override;
    void visit(RegisterStageInfo* op) override;
    void visit(SendStageInfo* op) override;
private:
    BinaryLogWriter* logger;
    std::ofstream* fout;
};

class TextWriteVisitor: public StageInfoVisitor {
public:
    TextWriteVisitor() = default;
    TextWriteVisitor(TextLogWriter* logger);
    void set_file_out(std::ofstream* fout);
    void visit(NewStageInfo* op) override;
    void visit(RegisterStageInfo* op) override;
    void visit(SendStageInfo* op) override;
private:
    TextLogWriter* logger;
    std::ofstream* fout;
};

class EnumerateVisitor: public StageInfoVisitor {
private:
    int number;
public:
    EnumerateVisitor() = default;
    void visit(NewStageInfo*) override {
        number = 0;
    }

    void visit(RegisterStageInfo*) override {
        number = 1;
    }

    void visit(SendStageInfo*) override {
        number = 2;
    }
    
    int get_number() {
        return number;
    }

    std::string type_number_to_str(int n) {
        if (n >= 4 || n < 0) {
            return "Other";
        }
        const std::string types[3] = {"New", "Register", "Send"};
        return types[n];
    }
};

struct LogActorInfo {
    std::string name;
    std::string activity_type;
    std::string type;
    std::string node_id;

    LogActorInfo();
    LogActorInfo(std::string name, std::string activity_type, std::string type, std::string node_id);
};

class LogParser {
public:
    virtual void parse(std::string filename) = 0;
    std::vector<StageInfo*>& getStages();
    std::set<std::string>& getActors();
    std::vector<LogActorInfo>& getActorsInfo();
    virtual ~LogParser();
protected:
    std::vector<StageInfo*> stages;
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
    EnumerateVisitor enumerator;
    void read_types(std::ifstream& fin, std::vector<std::string>& data);
    std::string read_actor_id(std::ifstream& fin);
};


class LogWriter {
public:
    LogWriter(std::string filename, int max_operations);
    virtual ~LogWriter();
    virtual void add_operation(StageInfo* op) = 0;
protected:
    std::vector<StageInfo*> operations;
    std::string filename;
    int max_operations;

    virtual void write_file() = 0;
};

class TextLogWriter: public LogWriter {
public:
    TextLogWriter(std::string filename, int max_operations): LogWriter(filename, max_operations) {
        writer = TextWriteVisitor(this);
    };
    void add_operation(StageInfo* op) override;
    ~TextLogWriter();
protected:
    void write_file() override;
    TextWriteVisitor writer;
};

class BinaryLogWriter: public LogWriter {
    friend class BinaryLogAddVisitor;
    friend class BinaryWriteVisitor;
protected:
    BinaryLogAddVisitor adder;
    BinaryWriteVisitor writer;
    EnumerateVisitor enumerator;
    std::unordered_map<std::string, unsigned char> activity_types;
    std::unordered_map<std::string, unsigned char> actor_types;
    std::unordered_map<std::string, unsigned char> send_types;

    void write_file() override;
    void write_map(std::ofstream& fout, std::unordered_map<std::string, unsigned char>& data);
    
    using hash_values = std::pair<std::pair<std::uint32_t, std::uint64_t>, std::uint32_t>;
    hash_values parse_actor_id(std::string actor_id);
public:
    BinaryLogWriter(std::string filename, int max_operations): LogWriter(filename, max_operations) {
        adder = BinaryLogAddVisitor(this);
        writer = BinaryWriteVisitor(this);
    };
    void add_operation(StageInfo* op) override;
    ~BinaryLogWriter();
};