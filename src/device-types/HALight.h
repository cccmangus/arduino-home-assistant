#ifndef AHA_HALIGHT_H
#define AHA_HALIGHT_H

#include "HABaseDeviceType.h"
#include "../utils/HANumeric.h"

#ifndef EX_ARDUINOHA_LIGHT

#if defined(ESP32) || defined(ESP8266)
#include <functional>
#define HALIGHT_STATE_CALLBACK_STD(name) std::function<void(bool state, HALight* sender)> name
#define HALIGHT_STATE_CALLBACK_PTR(name) void (*name)(bool state, HALight* sender)
#define HALIGHT_BRIGHTNESS_CALLBACK_STD(name) std::function<void(uint8_t brightness, HALight* sender)> name
#define HALIGHT_BRIGHTNESS_CALLBACK_PTR(name) void (*name)(uint8_t brightness, HALight* sender)
#define HALIGHT_COLOR_TEMP_CALLBACK_STD(name) std::function<void(uint16_t temperature, HALight* sender)> name
#define HALIGHT_COLOR_TEMP_CALLBACK_PTR(name) void (*name)(uint16_t temperature, HALight* sender)
#define HALIGHT_RGB_COLOR_CALLBACK_STD(name) std::function<void(HALight::RGBColor color, HALight* sender)> name
#define HALIGHT_RGB_COLOR_CALLBACK_PTR(name) void (*name)(HALight::RGBColor color, HALight* sender)
#else
#define HALIGHT_STATE_CALLBACK_PTR(name) void (*name)(bool state, HALight* sender)
#define HALIGHT_BRIGHTNESS_CALLBACK_PTR(name) void (*name)(uint8_t brightness, HALight* sender)
#define HALIGHT_COLOR_TEMP_CALLBACK_PTR(name) void (*name)(uint16_t temperature, HALight* sender)
#define HALIGHT_RGB_COLOR_CALLBACK_PTR(name) void (*name)(HALight::RGBColor color, HALight* sender)
#endif

/**
 * HALight allows adding a controllable light in the Home Assistant panel.
 * The library supports only the state, brightness, color temperature and RGB color.
 * If you need more features please open a new GitHub issue.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/light.mqtt/
 */
class HALight : public HABaseDeviceType
{
public:
    static const uint8_t RGBStringMaxLength;

    enum Features {
        DefaultFeatures = 0,
        BrightnessFeature = 1,
        ColorTemperatureFeature = 2,
        RGBFeature = 4
    };

    struct RGBColor {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        bool isSet;

        RGBColor() :
            red(0), green(0), blue(0), isSet(false) { }

        RGBColor(uint8_t r, uint8_t g, uint8_t b) :
            red(r), green(g), blue(b), isSet(true) { }

        void operator= (const RGBColor& a) {
            red = a.red;
            green = a.green;
            blue = a.blue;
            isSet = a.isSet;
        }

        bool operator== (const RGBColor& a) const {
            return (
                red == a.red &&
                green == a.green &&
                blue == a.blue
            );
        }

        bool operator!= (const RGBColor& a) const {
            return (
                red != a.red ||
                green != a.green ||
                blue != a.blue
            );
        }

        void fromBuffer(const uint8_t* data, const uint16_t length);
    };

    /**
     * @param uniqueId The unique ID of the light. It needs to be unique in a scope of your device.
     * @param features Features that should be enabled for the light.
     *                 You can enable multiple features by using OR bitwise operator, for example:
     *                 `HALight::BrightnessFeature | HALight::ColorTemperatureFeature`
     */
    HALight(const char* uniqueId, const uint8_t features = DefaultFeatures);

    /**
     * Changes state of the light and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param state New state of the light.
     * @param force Forces to update state without comparing it to previous known state.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setState(const bool state, const bool force = false);

    /**
     * Changes the brightness of the light and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param brightness The new brightness of the light.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setBrightness(const uint8_t brightness, const bool force = false);

    /**
     * Changes the color temperature of the light and publishes MQTT message.
     * Please note that if a new value is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param temperature The new color temperature of the light.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setColorTemperature(const uint16_t temperature, const bool force = false);

    /**
     * Changes the RGB color of the light and publishes MQTT message.
     * Please note that if a new color is the same as previous one,
     * the MQTT message won't be published.
     *
     * @param color The new RGB color of the light.
     * @param force Forces to update the value without comparing it to a previous known value.
     * @returns Returns `true` if MQTT message has been published successfully.
     */
    bool setRGBColor(const RGBColor& color, const bool force = false);

    /**
     * Alias for `setState(true)`.
     */
    inline bool turnOn()
        { return setState(true); }

    /**
     * Alias for `setState(false)`.
     */
    inline bool turnOff()
        { return setState(false); }

    /**
     * Sets current state of the light without publishing it to Home Assistant.
     * This method may be useful if you want to change state before connection
     * with MQTT broker is acquired.
     *
     * @param state New state of the light.
     */
    inline void setCurrentState(const bool state)
        { _currentState = state; }

    /**
     * Returns last known state of the light.
     * By default it's `false`.
     */
    inline bool getCurrentState() const
        { return _currentState; }

    /**
     * Sets the current brightness of the light without pushing the value to Home Assistant.
     * This method may be useful if you want to change the brightness before the connection
     * with the MQTT broker is acquired.
     *
     * @param brightness The new brightness of the light.
     */
    inline void setCurrentBrightness(const uint8_t brightness)
        { _currentBrightness = brightness; }

    /**
     * Returns the last known brightness of the light.
     * By default brightness is set to `0`.
     */
    inline uint8_t getCurrentBrightness() const
        { return _currentBrightness; }

    /**
     * Sets the current color temperature of the light without pushing the value to Home Assistant.
     * This method may be useful if you want to change the color temperature before the connection
     * with the MQTT broker is acquired.
     *
     * @param colorTemp The new color temperature (mireds).
     */
    inline void setCurrentColorTemperature(const uint16_t temperature)
        { _currentColorTemperature = temperature; }

    /**
     * Returns the last known color temperature of the light.
     * By default temperature is set to `0`.
     */
    inline uint16_t getCurrentColorTemperature() const
        { return _currentColorTemperature; }

    /**
     * Sets the current RGB color of the light without pushing the value to Home Assistant.
     * This method may be useful if you want to change the color before the connection
     * with the MQTT broker is acquired.
     *
     * @param color The new RGB color.
     */
    inline void setCurrentRGBColor(const RGBColor& color)
        { _currentRGBColor = color; }

    /**
     * Returns the last known RGB color of the light.
     * By default the RGB color is set to `0,0,0`.
     */
    inline const RGBColor& getCurrentRGBColor() const
        { return _currentRGBColor; }

    /**
     * Sets icon of the light.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the light's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

    /**
     * Sets optimistic flag for the light state.
     * In this mode the light state doesn't need to be reported back to the HA panel when a command is received.
     * By default the optimistic mode is disabled.
     *
     * @param optimistic The optimistic mode (`true` - enabled, `false` - disabled).
     */
    inline void setOptimistic(const bool optimistic)
        { _optimistic = optimistic; }

    /**
     * Sets the maximum brightness value that can be set via HA panel.
     * By default it's `255`.
     *
     * @param scale The maximum value of the brightness.
     */
    inline void setBrightnessScale(const uint8_t scale)
        { _brightnessScale.setBaseValue(scale); }

    /**
     * Sets the minimum color temperature (mireds) value that can be set via HA panel.
     * By default it's `153`.
     *
     * @param mireds The minimum value of the brightness.
     */
    inline void setMinMireds(const uint16_t mireds)
        { _minMireds.setBaseValue(mireds); }

    /**
     * Sets the maximum color temperature (mireds) value that can be set via HA panel.
     * By default it's `500`.
     *
     * @param mireds The maximum value of the brightness.
     */
    inline void setMaxMireds(const uint16_t mireds)
        { _maxMireds.setBaseValue(mireds); }

#if defined(ESP32) || defined(ESP8266)
    /**
     * Registers callback that will be called each time the state command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a pointer to a function.
     *
     * @param callback
     * @note In non-optimistic mode, the state must be reported back to HA using the HALight::setState method.
     */
    inline void onStateCommand(HALIGHT_STATE_CALLBACK_PTR(callback)) {
      _stateCallback = [callback](bool state, HALight* sender) {
            if (callback) {
                callback(state, sender);
            }
        };
    }

    /**
     * Registers callback that will be called each time the brightness command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a pointer to a function.
     *
     * @param callback
     * @note In non-optimistic mode, the brightness must be reported back to HA using the HALight::setBrightness method.
     */
    inline void onBrightnessCommand(HALIGHT_BRIGHTNESS_CALLBACK_PTR(callback)) {
      _brightnessCallback = [callback](uint8_t brightness, HALight* sender) {
            if (callback) {
                callback(brightness, sender);
            }
        };
    }

    /**
     * Registers callback that will be called each time the color temperature command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a pointer to a function.
     *
     * @param callback
     * @note In non-optimistic mode, the color temperature must be reported back to HA using the HALight::setColorTemperature method.
     */
    inline void onColorTemperatureCommand(HALIGHT_COLOR_TEMP_CALLBACK_PTR(callback)) {
      _colorTemperatureCallback = [callback](uint16_t temperature, HALight* sender) {
            if (callback) {
                callback(temperature, sender);
            }
        };
    }

    /**
     * Registers callback that will be called each time the RGB color command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a pointer to a function.
     *
     * @param callback
     * @note In non-optimistic mode, the color must be reported back to HA using the HALight::setRGBColor method.
     */
    inline void onRGBColorCommand(HALIGHT_RGB_COLOR_CALLBACK_PTR(callback)) {
      _rgbColorCallback = [callback](HALight::RGBColor color, HALight* sender) {
            if (callback) {
                callback(color, sender);
            }
        };
    }

    /**
     * Registers callback that will be called each time the state command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a std::function.
     *
     * @param callback
     * @note In non-optimistic mode, the state must be reported back to HA using the HALight::setState method.
     */
    inline void onStateCommand(HALIGHT_STATE_CALLBACK_STD(callback))
        { _stateCallback = std::move(callback); }

    /**
     * Registers callback that will be called each time the brightness command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a std::function.
     *
     * @param callback
     * @note In non-optimistic mode, the brightness must be reported back to HA using the HALight::setBrightness method.
     */
    inline void onBrightnessCommand(HALIGHT_BRIGHTNESS_CALLBACK_STD(callback))
        { _brightnessCallback = std::move(callback); }

    /**
     * Registers callback that will be called each time the color temperature command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a std::function.
     *
     * @param callback
     * @note In non-optimistic mode, the color temperature must be reported back to HA using the HALight::setColorTemperature method.
     */
    inline void onColorTemperatureCommand(HALIGHT_COLOR_TEMP_CALLBACK_STD(callback))
        { _colorTemperatureCallback = std::move(callback); }

    /**
     * Registers callback that will be called each time the RGB color command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This overload accepts a std::function.
     *
     * @param callback
     * @note In non-optimistic mode, the color must be reported back to HA using the HALight::setRGBColor method.
     */
    inline void onRGBColorCommand(HALIGHT_RGB_COLOR_CALLBACK_STD(callback))
        { _rgbColorCallback = std::move(callback); }
#else
    /**
     * Registers callback that will be called each time the state command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This version is used for platforms without std::function support.
     *
     * @param callback
     * @note In non-optimistic mode, the state must be reported back to HA using the HALight::setState method.
     */
    inline void onStateCommand(HALIGHT_STATE_CALLBACK_PTR(callback))
        { _stateCallback = callback; }

    /**
     * Registers callback that will be called each time the brightness command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This version is used for platforms without std::function support.
     *
     * @param callback
     * @note In non-optimistic mode, the brightness must be reported back to HA using the HALight::setBrightness method.
     */
    inline void onBrightnessCommand(HALIGHT_BRIGHTNESS_CALLBACK_PTR(callback))
        { _brightnessCallback = callback; }

    /**
     * Registers callback that will be called each time the color temperature command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This version is used for platforms without std::function support.
     *
     * @param callback
     * @note In non-optimistic mode, the color temperature must be reported back to HA using the HALight::setColorTemperature method.
     */
    inline void onColorTemperatureCommand(HALIGHT_COLOR_TEMP_CALLBACK_PTR(callback))
        { _colorTemperatureCallback = callback; }

    /**
     * Registers callback that will be called each time the RGB color command from HA is received.
     * Please note that it's not possible to register multiple callbacks for the same light.
     * This version is used for platforms without std::function support.
     *
     * @param callback
     * @note In non-optimistic mode, the color must be reported back to HA using the HALight::setRGBColor method.
     */
    inline void onRGBColorCommand(HALIGHT_RGB_COLOR_CALLBACK_PTR(callback))
        { _rgbColorCallback = callback; }
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
    /**
     * Publishes the MQTT message with the given state.
     *
     * @param state The state to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishState(const bool state);

    /**
     * Publishes the MQTT message with the given brightness.
     *
     * @param brightness The brightness to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishBrightness(const uint8_t brightness);

    /**
     * Publishes the MQTT message with the given color temperature (mireds).
     *
     * @param temperature The color temperature to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishColorTemperature(const uint16_t temperature);

    /**
     * Publishes the MQTT message with the given RGB color.
     *
     * @param color The color to publish.
     * @returns Returns `true` if the MQTT message has been published successfully.
     */
    bool publishRGBColor(const RGBColor& color);

    /**
     * Parses the given state command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleStateCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given brightness command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleBrightnessCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given color temperature command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleColorTemperatureCommand(const uint8_t* cmd, const uint16_t length);

    /**
     * Parses the given RGB color command and executes the callback with proper value.
     *
     * @param cmd The data of the command.
     * @param length Length of the command.
     */
    void handleRGBCommand(const uint8_t* cmd, const uint16_t length);

    /// Features enabled for the light.
    const uint8_t _features;

    /// The icon of the button. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The optimistic mode of the light (`true` - enabled, `false` - disabled).
    bool _optimistic;

    /// The maximum value of the brightness. By default it's 255.
    HANumeric _brightnessScale;

    /// The current state of the light. By default it's `false`.
    bool _currentState;

    /// The current brightness of the light. By default it's `0`.
    uint8_t _currentBrightness;

    /// The minimum color temperature (mireds). By default the value is not set.
    HANumeric _minMireds;

    /// The maximum color temperature (mireds). By default the value is not set.
    HANumeric _maxMireds;

    /// The current color temperature (mireds). By default the value is not set.
    uint16_t _currentColorTemperature;

    /// The current RBB color. By default the value is not set.
    RGBColor _currentRGBColor;

    /// The callback that will be called when the state command is received from the HA.
#if defined(ESP32) || defined(ESP8266)
    HALIGHT_STATE_CALLBACK_STD(_stateCallback);
#else
    HALIGHT_STATE_CALLBACK_PTR(_stateCallback);
#endif

    /// The callback that will be called when the brightness command is received from the HA.
#if defined(ESP32) || defined(ESP8266)
    HALIGHT_BRIGHTNESS_CALLBACK_STD(_brightnessCallback);
#else
    HALIGHT_BRIGHTNESS_CALLBACK_PTR(_brightnessCallback);
#endif

    /// The callback that will be called when the color temperature command is received from the HA.
#if defined(ESP32) || defined(ESP8266)
    HALIGHT_COLOR_TEMP_CALLBACK_STD(_colorTemperatureCallback);
#else
    HALIGHT_COLOR_TEMP_CALLBACK_PTR(_colorTemperatureCallback);
#endif

    /// The callback that will be called when the RGB command is received from the HA.
#if defined(ESP32) || defined(ESP8266)
    HALIGHT_RGB_COLOR_CALLBACK_STD(_rgbColorCallback);
#else
    HALIGHT_RGB_COLOR_CALLBACK_PTR(_rgbColorCallback);
#endif
};

#endif
#endif
