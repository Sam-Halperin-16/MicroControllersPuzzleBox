/**
 * State machine header file
 * Author: Micah Henney, Samuel Halperin, Noah Billman
 * 
 * Defines a state machine that ticks every loop, performing actions asynchronously
 */

// Include members for the state machine class, like current state.

template <typename StateType>
class StateMachine {
public:
    StateType getState() const {
        return currentState;
    }
    void setState(StateType newState) {
        currentState = newState;
    }
    virtual void tick() = 0;

protected:
    explicit StateMachine(StateType initialState)
        : currentState(initialState) {}

    StateType currentState;
};
