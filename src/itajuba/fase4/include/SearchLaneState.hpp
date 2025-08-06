#include "fsm/fsm.hpp"

typedef fsm::Blackboard BB;

class SearchLaneState : public fsm::State {
public:
    SearchLaneState() : fsm::State() {}

    void on_enter(BB &bb) override {

    }

    std::string act(BB &bb) override {
        return "";
    }
};