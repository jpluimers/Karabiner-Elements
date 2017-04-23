#pragma once

#include "boost_defs.hpp"

#include "event_queue.hpp"
#include "types.hpp"
#include <boost/optional.hpp>
#include <vector>

namespace krbn {
class pressed_physical_keys_counter final {
public:
  class pressed_key final {
  public:
    pressed_key(device_id device_id,
                const event_queue::queued_event::event& original_event) : device_id_(device_id),
                                                                          original_event_(original_event) {
    }

    device_id get_device_id(void) const {
      return device_id_;
    }

    const event_queue::queued_event::event& get_original_event(void) const {
      return original_event_;
    }

  private:
    device_id device_id_;
    event_queue::queued_event::event original_event_;
  };

  bool empty(device_id device_id) {
    for (const auto& k : pressed_keys_) {
      if (k.get_device_id() == device_id) {
        return false;
      }
    }
    return true;
  }

  bool is_pointing_button_pressed(device_id device_id) {
    for (const auto& k : pressed_keys_) {
      if (k.get_device_id() == device_id &&
          k.get_original_event().get_type() == event_queue::queued_event::event::type::pointing_button) {
        return true;
      }
    }
    return false;
  }

  bool update(const event_queue::queued_event& queued_event) {
    auto key_code = queued_event.get_event().get_key_code();
    auto pointing_button = queued_event.get_event().get_pointing_button();

    if (key_code || pointing_button) {
      auto device_id = queued_event.get_device_id();
      auto original_event = queued_event.get_original_event();

      if (queued_event.get_original_event().get_event_type() == event_type::key_down) {
        emplace_back_event(device_id, original_event);
      } else {
        erase_all_matched_events(device_id, original_event);
      }

      return true;
    }

    return false;
  }

  void emplace_back_event(device_id device_id,
                          const event_queue::queued_event::event& original_event) {
    pressed_keys_.emplace_back(device_id, original_event);
  }

  void erase_all_matched_events(device_id device_id) {
    pressed_keys_.erase(std::remove_if(std::begin(pressed_keys_),
                                       std::end(pressed_keys_),
                                       [&](const pressed_key& k) {
                                         return k.get_device_id() == device_id;
                                       }),
                        std::end(pressed_keys_));
  }

  void erase_all_matched_events(device_id device_id,
                                const event_queue::queued_event::event& original_event) {
    pressed_keys_.erase(std::remove_if(std::begin(pressed_keys_),
                                       std::end(pressed_keys_),
                                       [&](const pressed_key& k) {
                                         // key_code or pointing_button
                                         return k.get_device_id() == device_id &&
                                                k.get_original_event().get_key_code() == original_event.get_key_code() &&
                                                k.get_original_event().get_pointing_button() == original_event.get_pointing_button();
                                       }),
                        std::end(pressed_keys_));
  }

private:
  std::vector<pressed_key> pressed_keys_;
};
} // namespace krbn
