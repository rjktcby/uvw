#pragma once


#include <utility>
#include <memory>
#include <uv.h>
#include "event.hpp"
#include "handle.hpp"
#include "util.hpp"


namespace uvw {


/**
 * @brief SignalEvent event.
 *
 * It will be emitted by the SignalHandle according with its functionalities.
 */
struct SignalEvent: Event<SignalEvent> {
    explicit SignalEvent(int sig) noexcept: signum(sig) { }

    /**
     * @brief Gets the signal being monitored by this handle.
     * @return The signal being monitored by this handle.
     */
    int signal() const noexcept { return signum; }

private:
    const int signum;
};


/**
 * @brief The SignalHandle handle.
 *
 * Signal handles implement Unix style signal handling on a per-event loop
 * bases.<br/>
 * Reception of some signals is emulated on Windows.
 *
 * See the official
 * [documentation](http://docs.libuv.org/en/v1.x/signal.html)
 * for further details.
 */
class SignalHandle final: public Handle<SignalHandle, uv_signal_t> {
    static void startCallback(uv_signal_t *handle, int signum) {
        SignalHandle &signal = *(static_cast<SignalHandle*>(handle->data));
        signal.publish(SignalEvent{signum});
    }

    using Handle::Handle;

public:
    /**
     * @brief Creates a new signal handle.
     * @param args A pointer to the loop from which the handle generated.
     * @return A pointer to the newly created handle.
     */
    template<typename... Args>
    static std::shared_ptr<SignalHandle> create(Args&&... args) {
        return std::shared_ptr<SignalHandle>{new SignalHandle{std::forward<Args>(args)...}};
    }

    /**
     * @brief Initializes the handle.
     * @return True in case of success, false otherwise.
     */
    bool init() {
        return initialize<uv_signal_t>(&uv_signal_init);
    }

    /**
     * @brief Starts the handle.
     *
     * The handle will start emitting SignalEvent when needed.
     *
     * @param signum The signal to be monitored.
     */
    void start(int signum) {
        invoke(&uv_signal_start, get<uv_signal_t>(), &startCallback, signum);
    }

    /**
     * @brief Stops the handle.
     */
    void stop() {
        invoke(&uv_signal_stop, get<uv_signal_t>());
    }

    /**
     * @brief Gets the signal being monitored.
     * @return The signal being monitored.
     */
    int signal() const noexcept {
        return get<uv_signal_t>()->signum;
    }
};


}
