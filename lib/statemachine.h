//
// Created by paulm on 4/29/25.
//

#pragma once
#include <functional>
#include <utility>
#include <tuple>
template<typename T>
concept StatelessCallable = requires(T t) {
    { t() } -> std::same_as<void>;
};

template<auto ThisState,
    StatelessCallable auto OnEnter,
    StatelessCallable auto OnExit,
    auto... AllowedTransitions>
class State {
public:
    State() = default;

    using EnumType = decltype(ThisState);

    static EnumType GetState() {
        return ThisState;
    }

    static constexpr bool IsTransitionAllowed(EnumType nextState) {
        return ((nextState == AllowedTransitions) || ...);
    }

    static void EnterState(EnumType nextState) {
        OnEnter();
    }

    static void ExitState() {
        OnExit();
    }
};

template<typename EnumType, EnumType InitialState = {}, typename... StatesTypes>
class StateMachine {
public:
    StateMachine() {
        States = std::make_tuple(StatesTypes{}...);
        CurrentState = InitialState;
    };

    bool GoToState(EnumType nextState) {
        // Ignore redundant transitions
        if (CurrentState == nextState)
            return false;

        // Get type-erased handles to the current and next state objects.
        auto currentVariant = FindState(CurrentState);
        auto nextVariant    = FindState(nextState);

        bool allowedTransition = false;
        // std::visit will call the lambda with the active alternatives from each variant.
        std::visit([&](auto& current, auto& next) {
            // current and next are std::reference_wrapper of the specific state types
            if (current.get().IsTransitionAllowed(nextState)) {
                current.get().ExitState();
                next.get().EnterState(CurrentState);
                allowedTransition = true;
            }
        }, currentVariant, nextVariant);

        if (allowedTransition)
            CurrentState = nextState;
        return allowedTransition;
    }

    [[nodiscard]] EnumType GetCurrentState() const {
        return CurrentState;
    }

private:
    EnumType CurrentState;
    std::tuple<StatesTypes...> States;

    using StateVariant = std::variant<std::reference_wrapper<StatesTypes>...>;

    StateVariant FindState(EnumType state) {
        std::optional<StateVariant> foundState;

        std::apply([&](auto&... s) {
            ((s.GetState() == state
                ? static_cast<void>(foundState = StateVariant{std::ref(s)})
                : void()), ...);
        }, States);

        if (!foundState)
            throw std::runtime_error("State not found");

        return *foundState;
    }
};
