#include "HABinarySensor.h"
#ifndef EX_ARDUINOHA_BINARY_SENSOR

#include "../HAMqtt.h"
#include "../utils/HASerializer.h"

HABinarySensor::HABinarySensor(const char* uniqueId) :
    HABaseDeviceType(AHATOFSTR(HAComponentBinarySensor), uniqueId),
    _deviceClass(nullptr),
    _stateClass(nullptr),
    _icon(nullptr),
    _enableByDefault(true),
    _entityCategory(nullptr),
    _currentState(false)
{

}

bool HABinarySensor::setState(const bool state, const bool force)
{
    if (!force && state == _currentState) {
        return true;
    }

    if (publishState(state)) {
        _currentState = state;
        return true;
    }

    return false;
}

void HABinarySensor::setExpireAfter(uint16_t expireAfter)
{
    if (expireAfter > 0) {
        _expireAfter.setBaseValue(expireAfter);
    } else {
        _expireAfter.reset();
    }
}

void HABinarySensor::buildSerializer()
{
    if (_serializer || !uniqueId()) {
        return;
    }

    _serializer = new HASerializer(this, 11); // 11 - max properties nb
    _serializer->set(AHATOFSTR(HANameProperty), _name);
    _serializer->set(AHATOFSTR(HAObjectIdProperty), _objectId);
    _serializer->set(HASerializer::WithUniqueId);
    _serializer->set(AHATOFSTR(HADeviceClassProperty), _deviceClass);
    _serializer->set(AHATOFSTR(HAStateClassProperty), _stateClass);
    _serializer->set(AHATOFSTR(HAIconProperty), _icon);
    _serializer->set(AHATOFSTR(HAEntityCategory), _entityCategory);

    if (!_enableByDefault) {
        _serializer->set(
            AHATOFSTR(HAEnabledByDefaultProperty),
            &_enableByDefault,
            HASerializer::BoolPropertyType
        );
    }

    if (_expireAfter.isSet()) {
        _serializer->set(
            AHATOFSTR(HAExpireAfterProperty),
            &_expireAfter,
            HASerializer::NumberPropertyType
        );
    }

    _serializer->set(HASerializer::WithDevice);
    _serializer->set(HASerializer::WithAvailability);
    _serializer->topic(AHATOFSTR(HAStateTopic));
}

void HABinarySensor::onMqttConnected()
{
    if (!uniqueId()) {
        return;
    }

    publishConfig();
    publishAvailability();
    publishState(_currentState);
}

bool HABinarySensor::publishState(const bool state)
{
    return publishOnDataTopic(
        AHATOFSTR(HAStateTopic),
        AHATOFSTR(state ? HAStateOn : HAStateOff),
        true
    );
}

#endif
