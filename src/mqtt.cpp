#include "globals.h"
#include "string_utils.h"
#include <WiFi.h>

bool pub(const char *topic, uint8_t qos, bool retain, const char *payload, size_t length = 0, bool dup = false, uint16_t message_id = 0)
{
    for (int i = 0; i < 10; i++)
    {
        if (mqttClient.publish(topic, qos, retain, payload, length, dup, message_id))
            return true;
        delay(50);
    }
    return false;
}

void commonDiscovery()
{
    doc.clear();
    auto identifiers = doc["dev"].createNestedArray("ids");
    identifiers.add(Sprintf("espresense_%06lx", CHIPID));
    auto connections = doc["dev"].createNestedArray("cns");
    auto mac = connections.createNestedArray();
    mac.add("mac");
    mac.add(WiFi.macAddress());
    doc["dev"]["name"] = "ESPresense " + room;
    doc["dev"]["sa"] = room;
#ifdef VERSION
    doc["dev"]["sw"] = VERSION;
#endif
#ifdef FIRMWARE
    doc["dev"]["mf"] = "ESPresense (" FIRMWARE ")";
#endif
    doc["dev"]["cu"] = "http://" + localIp;
    doc["dev"]["mdl"] = String(ESP.getChipModel());
}

bool sendConnectivityDiscovery()
{
    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = "ESPresense " + room;
    doc["uniq_id"] = Sprintf("espresense_%06lx_connectivity", CHIPID);
    doc["json_attr_t"] = "~/telemetry";
    doc["stat_t"] = "~/status";
    doc["dev_cla"] = "connectivity";
    doc["pl_on"] = "online";
    doc["pl_off"] = "offline";

    serializeJson(doc, buffer);
    String discoveryTopic = Sprintf("homeassistant/binary_sensor/espresense_%06lx/connectivity/config", CHIPID);
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendTeleBinarySensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &devClass)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06lx_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["value_template"] = temp;
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;
    if (!devClass.isEmpty()) doc["dev_cla"] = devClass;

    serializeJson(doc, buffer);
    String discoveryTopic = Sprintf("homeassistant/binary_sensor/espresense_%06lx/%s/config", CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendTeleSensorDiscovery(const String &name, const String &entityCategory, const String &temp, const String &units, const String &devClass)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06lx_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/telemetry";
    doc["value_template"] = temp;
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;
    if (!units.isEmpty()) doc["unit_of_meas"] = units;
    if (!devClass.isEmpty()) doc["dev_cla"] = devClass;

    serializeJson(doc, buffer);
    String discoveryTopic = Sprintf("homeassistant/sensor/espresense_%06lx/%s/config", CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendSensorDiscovery(const String &name, const String &entityCategory, const String &units, const String &devClass)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06lx_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    if (!entityCategory.isEmpty()) doc["entity_category"] = entityCategory;
    if (!units.isEmpty()) doc["unit_of_meas"] = units;
    if (!devClass.isEmpty()) doc["dev_cla"] = devClass;
    doc["frc_upd"] = true;

    serializeJson(doc, buffer);
    String discoveryTopic = Sprintf("homeassistant/sensor/espresense_%06lx/%s/config", CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendButtonDiscovery(const String &name, const String &entityCategory)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06lx_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["entity_category"] = entityCategory;

    serializeJson(doc, buffer);
    String discoveryTopic = Sprintf("homeassistant/button/espresense_%06lx/%s/config", CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendSwitchDiscovery(const String &name, const String &entityCategory)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06lx_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["entity_category"] = entityCategory;

    serializeJson(doc, buffer);
    String discoveryTopic = Sprintf("homeassistant/switch/espresense_%06lx/%s/config", CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, buffer, 0);
}

bool sendNumberDiscovery(const String &name, const String &entityCategory)
{
    auto slug = slugify(name);

    commonDiscovery();
    doc["~"] = roomsTopic;
    doc["name"] = Sprintf("ESPresense %s %s", room.c_str(), name.c_str());
    doc["uniq_id"] = Sprintf("espresense_%06lx_%s", CHIPID, slug.c_str());
    doc["avty_t"] = "~/status";
    doc["stat_t"] = "~/" + slug;
    doc["cmd_t"] = "~/" + slug + "/set";
    doc["step"] = "0.1";
    doc["entity_category"] = entityCategory;

    serializeJson(doc, buffer);
    String discoveryTopic = Sprintf("homeassistant/number/espresense_%06lx/%s/config", CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, true, buffer);
}

bool sendDeleteDiscovery(const String &domain, const String &name)
{
    auto slug = slugify(name);
    String discoveryTopic = Sprintf("homeassistant/%s/espresense_%06lx/%s/config", domain, CHIPID, slug.c_str());
    return pub(discoveryTopic.c_str(), 0, false, "");
}
