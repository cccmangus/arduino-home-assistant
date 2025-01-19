#ifndef AHA_HABUTTON_H
#define AHA_HABUTTON_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_BUTTON

#if defined(ESP32) || defined(ESP8266)
#include <functional>
#define HABUTTON_CALLBACK_STD(name) std::function<void(HAButton* sender)> name
#define HABUTTON_CALLBACK_PTR(name) void (*name)(HAButton* sender)
#else
#define HABUTTON_CALLBACK_PTR(name) void (*name)(HAButton* sender)
#endif

/**
 * HAButton represents a button that's displayed in the Home Assistant panel and
 * triggers some logic on your Arduino/ESP device once clicked.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/button.mqtt/
 */
class HAButton : public HABaseDeviceType
{
public:
    /**
     * @param uniqueId The unique ID of the button. It needs to be unique in a scope of your device.
     */
    HAButton(const char* uniqueId);

    /**
     * Sets class of the device.
     * You can find list of available values here: https://www.home-assistant.io/integrations/button/#device-class
     *
     * @param deviceClass The class name.
     */
    inline void setDeviceClass(const char* deviceClass)
        { _class = deviceClass; }

    /**
     * Sets icon of the button.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the button's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

#if defined(ESP32) || defined(ESP8266)
    /**
     * Registers a callback that will be called each time the press command from HA is received.
     * This overload accepts a pointer to a function.
     *
     * @param callback Pointer to a function.
     */
    void onCommand(HABUTTON_CALLBACK_PTR(callback)) {
        _commandCallback = [callback](HAButton* sender) {
            if (callback) {
                callback(sender);
            }
        };
    }

    /**
     * Registers a callback that will be called each time the press command from HA is received.
     * This overload accepts a std::function.
     *
     * @param callback std::function.
     */
    void onCommand(HABUTTON_CALLBACK_STD(callback)) {
        _commandCallback = std::move(callback);
    }
#else
    /**
     * Registers a callback that will be called each time the press command from HA is received.
     * This version is used for platforms without std::function support.
     *
     * @param callback Pointer to a function.
     */
    inline void onCommand(HABUTTON_CALLBACK_PTR(callback))
        { _commandCallback = callback; }
#endif

protected:
    virtual void buildSerializer() override;
    virtual void onMqttConnected() override;
    virtual void onMqttMessage(
        const char* topic,
        const uint8_t* payload,
        const uint16_t length
    ) override;

private:
    /// The device class. It can be nullptr.
    const char* _class;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The command callback that will be called once clicking the button in HA panel.
#if defined(ESP32) || defined(ESP8266)
    HABUTTON_CALLBACK_STD(_commandCallback);
#else
    HABUTTON_CALLBACK_PTR(_commandCallback);
#endif
};

#endif
#endif
