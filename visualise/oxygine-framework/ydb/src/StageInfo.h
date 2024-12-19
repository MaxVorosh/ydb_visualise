#pragma once

#include <string>

struct NewStageInfo;
struct RegisterStageInfo;
struct SendStageInfo;

class StageInfoVisitor {
public:
    virtual void visit(NewStageInfo* stage) = 0;
    virtual void visit(RegisterStageInfo* stage) = 0;
    virtual void visit(SendStageInfo* stage) = 0;
};

struct StageInfo {
    std::string main_actor;
    
    virtual void accept(StageInfoVisitor* v) = 0;
    virtual ~StageInfo() = default;
};

struct NewStageInfo: public StageInfo {
    std::string actor_type;
    std::string activity_type;

    NewStageInfo() = default;
    NewStageInfo(std::string actor, std::string actor_type, std::string activity_type) {
        this->main_actor = actor;
        this->actor_type = actor_type;
        this->activity_type = activity_type;
    }
    
    void accept(StageInfoVisitor* v) override {
        v->visit(this);
    }

    ~NewStageInfo() = default;
};

struct RegisterStageInfo: public StageInfo {
    RegisterStageInfo() = default;
    RegisterStageInfo(std::string actor) {
        this->main_actor = actor;
    }

    void accept(StageInfoVisitor* v) override {
        v->visit(this);
    }

    ~RegisterStageInfo() = default;
};

struct SendStageInfo: public StageInfo {
    std::string other_actor;
    std::string type;

    SendStageInfo() = default;
    SendStageInfo(std::string actor_from, std::string actor_to, std::string type) {
        this->main_actor = actor_from;
        this->other_actor = actor_to;
        this->type = type;
    }

    void accept(StageInfoVisitor* v) override {
        v->visit(this);
    }

    ~SendStageInfo() = default;
};