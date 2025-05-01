//
// Created by paulm on 4/29/25.
//

#include <gtest/gtest.h>
#include <statemachine.h>

enum class AppState {
    Idle,
    Running,
    Stopped
};
using IdleState = State<
        AppState::Idle,
        []() { std::cout << "Entering Idle" << std::endl; },
        []() { std::cout << "Exiting Idle" << std::endl; },
        AppState::Running>;

using RunningState = State<
        AppState::Running,
        []() { std::cout << "Entering Running" << std::endl; },
        []() { std::cout << "Exiting Running" << std::endl; },
        AppState::Stopped>;

using StoppedState = State<
    AppState::Stopped,
        []() { std::cout << "Entering Stopped" << std::endl; },
        []() { std::cout << "Exiting Stopped" << std::endl; },
        AppState::Idle>;

TEST(StateTransitions, AreAllowed) {
    IdleState state;
    EXPECT_TRUE(state.IsTransitionAllowed(AppState::Running));
    EXPECT_FALSE(state.IsTransitionAllowed(AppState::Stopped));
}

TEST(StateMachineTransitions, CorrectInitialState) {
    StateMachine<AppState, AppState::Idle, IdleState, RunningState> sm;
    StateMachine<AppState, AppState::Running, IdleState, RunningState> sm2;
    EXPECT_EQ(sm.GetCurrentState(), AppState::Idle);
    EXPECT_EQ(sm2.GetCurrentState(), AppState::Running);
}

TEST(StateMachineTransitions, AreAllowed) {
    StateMachine<AppState, AppState::Idle, IdleState, RunningState, StoppedState> sm;
    EXPECT_TRUE(sm.GoToState(AppState::Running));
    EXPECT_FALSE(sm.GoToState(AppState::Idle));
    EXPECT_TRUE(sm.GoToState(AppState::Stopped));
    EXPECT_FALSE(sm.GoToState(AppState::Stopped));
    EXPECT_FALSE(sm.GoToState(AppState::Running));
    EXPECT_TRUE(sm.GoToState(AppState::Idle));
}