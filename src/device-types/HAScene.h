#ifndef AHA_HASCENE_H
#define AHA_HASCENE_H

#include "HABaseDeviceType.h"

#ifndef EX_ARDUINOHA_SCENE

#if defined(ESP32) || defined(ESP8266)
#define HASCENE_CALLBACK_STD(name) std::function<void(HAScene* sender)> name
#define HASCENE_CALLBACK_PTR(name) void (*name)(HAScene* sender)
#else
#define HASCENE_CALLBACK_PTR(name) void (*name)(HAScene* sender)
#endif

/**
 * HAScene adds a new scene to the Home Assistant that triggers your callback once activated.
 *
 * @note
 * You can find more information about this entity in the Home Assistant documentation:
 * https://www.home-assistant.io/integrations/scene.mqtt/
 */
class HAScene : public HABaseDeviceType
{
public:
    /**
     * @param uniqueId The unique ID of the scene. It needs to be unique in a scope of your device.
     */
    HAScene(const char* uniqueId);

    /**
     * Sets icon of the scene.
     * Any icon from MaterialDesignIcons.com (for example: `mdi:home`).
     *
     * @param icon The icon name.
     */
    inline void setIcon(const char* icon)
        { _icon = icon; }

    /**
     * Sets retain flag for the scene's command.
     * If set to `true` the command produced by Home Assistant will be retained.
     *
     * @param retain
     */
    inline void setRetain(const bool retain)
        { _retain = retain; }

#if defined(ESP32) || defined(ESP8266)
    /**
     * Registers callback that will be called when the scene is activated in the HA panel.
     * Please note that it's not possible to register multiple callbacks for the same scene.
     * This overload accepts a pointer to a function.
     *
     * @param callback
     */
    inline void onCommand(HASCENE_CALLBACK_PTR(callback)) {
        _commandCallback = [callback](HAScene* sender) {
            if (callback) {
                callback(sender);
            }
        };
    }

    /**
     * Registers callback that will be called when the scene is activated in the HA panel.
     * Please note that it's not possible to register multiple callbacks for the same scene.
     * This overload accepts a std::function.
     *
     * @param callback
     */
    inline void onCommand(HASCENE_CALLBACK_STD(callback))
    { _commandCallback = std::move(callback); }
#else
    /**
     * Registers callback that will be called when the scene is activated in the HA panel.
     * Please note that it's not possible to register multiple callbacks for the same scene.
     * This version is used for platforms without std::function support.
     *
     * @param callback
     */
    inline void onCommand(HASCENE_CALLBACK_PTR(callback))
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
    /// The icon of the scene. It can be nullptr.
    const char* _icon;

    /// The retain flag for the HA commands.
    bool _retain;

    /// The command callback that will be called when scene is activated from the HA panel.
#if defined(ESP32) || defined(ESP8266)
    HASCENE_CALLBACK_STD(_commandCallback);
#else
    HASCENE_CALLBACK_PTR(_commandCallback);
#endif
};

#endif
#endif
