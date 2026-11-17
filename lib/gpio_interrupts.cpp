#include "gpio_interrupts.h"
#include "pico/stdlib.h"
#include "hardware/sync.h"

namespace {
constexpr unsigned int PIN_COUNT = 30; // RP2040 bank 0, GP0..GP29.
constexpr std::uint32_t EDGES = GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL;
struct Subscription {
    gpio_interrupts::Handler handler = nullptr;
    void* context = nullptr;
    std::uint32_t edges = 0;
};
Subscription subscriptions[PIN_COUNT];

void dispatch(unsigned int gpio, std::uint32_t events) {
    if (gpio >= PIN_COUNT) return;
    const auto& subscription = subscriptions[gpio];
    const auto matching = events & subscription.edges;
    if (subscription.handler && matching) {
        subscription.handler(gpio, matching, subscription.context);
    }
}
}

bool gpio_interrupts::attach(unsigned int gpio, std::uint32_t edges, Handler handler, void* context) {
    if (get_core_num() != 0 || gpio >= PIN_COUNT || !handler || edges == 0 || (edges & ~EDGES)) {
        return false;
    }
    const auto interrupts = save_and_disable_interrupts();
    auto& subscription = subscriptions[gpio];
    if (subscription.handler && (subscription.handler != handler || subscription.context != context)) {
        restore_interrupts(interrupts);
        return false;
    }
    gpio_set_irq_enabled(gpio, EDGES, false);
    gpio_acknowledge_irq(gpio, EDGES);
    subscription = {handler, context, edges};
    // Every driver uses this dispatcher instead of replacing the SDK callback.
    gpio_set_irq_enabled_with_callback(gpio, edges, true, dispatch);
    restore_interrupts(interrupts);
    return true;
}

bool gpio_interrupts::detach(unsigned int gpio, Handler handler, void* context) {
    if (get_core_num() != 0 || gpio >= PIN_COUNT || !handler) return false;
    const auto interrupts = save_and_disable_interrupts();
    auto& subscription = subscriptions[gpio];
    if (subscription.handler != handler || subscription.context != context) {
        restore_interrupts(interrupts);
        return false;
    }
    gpio_set_irq_enabled(gpio, EDGES, false);
    gpio_acknowledge_irq(gpio, EDGES);
    subscription = {};
    restore_interrupts(interrupts);
    return true;
}
