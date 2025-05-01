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

class FakeContext {
public:
    uint8_t EnteredCount = 0;
    uint8_t ExitedCount = 0;
};

using IdleState = State<
        AppState::Idle,
        [](void* context) {
            if (context) {
                auto* ctx = static_cast<FakeContext*>(context);
                ctx->EnteredCount++;
            }
        },
        [](void* context) {
            if (context) {
                auto* ctx = static_cast<FakeContext*>(context);
                ctx->ExitedCount++;
            }
        },
        AppState::Running>;

using RunningState = State<
        AppState::Running,
        [](void* context) {
            if (context) {
                auto* ctx = static_cast<FakeContext*>(context);
                ctx->EnteredCount++;
            }
        },
        [](void* context) {
            if (context) {
                auto* ctx = static_cast<FakeContext*>(context);
                ctx->ExitedCount++;
            }
        },
        AppState::Stopped>;

using StoppedState = State<
    AppState::Stopped,
        [](void* context) {
            if (context) {
                auto* ctx = static_cast<FakeContext*>(context);
                ctx->EnteredCount++;
            }
        },
        [](void* context) {
            if (context) {
                auto* ctx = static_cast<FakeContext*>(context);
                ctx->ExitedCount++;
            }
        },
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

TEST(StateMachineTransitions, Context) {
    FakeContext context;
    StateMachine<AppState, AppState::Idle, IdleState, RunningState, StoppedState> sm(&context);
    EXPECT_EQ(sm.GetContext(), &context);
    EXPECT_EQ(context.EnteredCount, 0);
    EXPECT_EQ(context.ExitedCount, 0);

    EXPECT_TRUE(sm.GoToState(AppState::Running));
    EXPECT_EQ(context.EnteredCount, 1);
    EXPECT_EQ(context.ExitedCount, 1);
    EXPECT_TRUE(sm.GoToState(AppState::Stopped));
    EXPECT_EQ(context.EnteredCount, 2);
    EXPECT_EQ(context.ExitedCount, 2);
    EXPECT_TRUE(sm.GoToState(AppState::Idle));
    EXPECT_EQ(context.EnteredCount, 3);
    EXPECT_EQ(context.ExitedCount, 3);
}