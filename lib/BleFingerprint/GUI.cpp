#include "GUI.h"

#ifdef M5STICK
#include "tb_display.h"
#endif

#if defined M5STICK

#define LED_BUILTIN 10
#define LED_BUILTIN_ON 0

#define BUTTON 39
#define BUTTON_PRESSED 0

#elif defined M5ATOM

#define BUTTON 39
#define BUTTON_PRESSED 0

#elif defined HUZZAH32

#define LED_BUILTIN 13
#define LED_BUILTIN_ON 1

#else //DevKit / generic

#define LED_BUILTIN 2
#define LED_BUILTIN_ON 1

#endif

void GUI::seenStart()
{
    begin();
#ifdef M5ATOM
    M5.dis.drawpix(0, CRGB(15, 15, 15));
#else
    if (GUI::statusLed) digitalWrite(LED_BUILTIN, LED_BUILTIN_ON);
#endif
}

void GUI::seenEnd()
{
    begin();
#ifdef M5ATOM
    M5.dis.drawpix(0, CRGB(0, 0, 0));
#else
    digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
#endif
}

void GUI::erasing()
{
    status("Erasing...");
    Serial.println(F("Resetting back to defaults..."));
}

void GUI::erased()
{
}

void GUI::connecting()
{
#ifdef LED_BUILTIN
    if (GUI::statusLed) digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
#endif
}

void GUI::connected(bool wifi = false, bool mqtt = false)
{
    begin();
#ifdef M5ATOM
    if (!wifi)
        M5.dis.drawpix(0, CRGB(0, 128, 0));
    else if (!mqtt)
        M5.dis.drawpix(0, CRGB(0, 0, 128));
    else
        M5.dis.drawpix(0, CRGB(0, 0, 0));
#else
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
#endif
    status("Wifi:%s Mqtt:%s", (wifi ? "yes" : "no"), (mqtt ? "yes" : "no"));
#endif
}

void GUI::added(BleFingerprint *f)
{
    if (f->getIgnore()) return;
    Serial.printf("%u New %s | MAC: %s, ID: %-58s%ddBm %s\n", xPortGetCoreID(), f->getRmAsst() ? "R" : (f->getAllowQuery() ? "Q" : " "), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDiscriminator().c_str());
}

void GUI::removed(BleFingerprint *f)
{
    if (f->getIgnore() || !f->getAdded()) return;
    Serial.printf("\u001b[38;5;236m%u Del   | MAC: %s, ID: %-58s%ddBm %s\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDiscriminator().c_str());
}

void GUI::plusOne(BleFingerprint *f)
{
    Serial.printf("\u001b[36m%u C# +1 | MAC: %s, ID: %-58s%ddBm (%.2fm) %lums\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
}

void GUI::minusOne(BleFingerprint *f)
{
    Serial.printf("\u001b[35m%u C# -1 | MAC: %s, ID: %-58s%ddBm (%.2fm) %lums\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getRssi(), f->getDistance(), f->getMsSinceLastSeen());
}

void GUI::close(BleFingerprint *f)
{
    Serial.printf("\u001b[32m%u Close | MAC: %s, ID: %-58s%ddBm\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getNewestRssi());
    status("C:%s", f->getId().c_str());
}

void GUI::left(BleFingerprint *f)
{
    Serial.printf("\u001b[33m%u Left  | MAC: %s, ID: %-58s%ddBm\u001b[0m\n", xPortGetCoreID(), f->getMac().c_str(), f->getId().c_str(), f->getNewestRssi());
    status("L:%s", f->getId().c_str());
}

void GUI::radar(bool value)
{
    Serial.printf("%u Radar | %s\n", xPortGetCoreID(), value ? "detected" : "cleared");
    status("Radar:%s", value ? "detected" : "cleared");
}

void GUI::pir(bool value)
{
    Serial.printf("%u Pir   | %s\n", xPortGetCoreID(), value ? "detected" : "cleared");
    status("Pir:%s", value ? "detected" : "cleared");
}

void GUI::status(const char *format, ...)
{
    begin();
#ifdef M5STICK
    char *message;
    va_list args;
    va_start(args, format);
    vasprintf(&message, format, args);
    va_end(args);
    tb_display_print_String(message);
    tb_display_print_String("\n");
#ifdef PLUS
    //drawString(message, sprite.width() / 2, sprite.height() / 2, 4);
#else

    //sprite.drawString(message, sprite.width() / 2, sprite.height() / 2, 1);
#endif
    free(message);
    dirty = true;
#endif
}

void GUI::setup()
{
#ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
#endif
}

void GUI::begin()
{
    if (!GUI::init)
    {
#ifdef M5STICK
        M5.begin(true, true, false);
        M5.Axp.ScreenBreath(12);
        tb_display_init(3);
#elif defined M5ATOM
        M5.begin(false, false, true);
#endif
        GUI::init = true;
    }
}

void GUI::updateStart()
{
#ifdef LED_BUILTIN
    if (GUI::statusLed) digitalWrite(LED_BUILTIN, LED_BUILTIN_ON);
#endif
}

void GUI::updateProgress(unsigned int percent)
{
#ifdef LED_BUILTIN
    if (GUI::statusLed) digitalWrite(LED_BUILTIN, percent % 2);
#endif
}

void GUI::updateEnd()
{
#ifdef LED_BUILTIN
    digitalWrite(LED_BUILTIN, !LED_BUILTIN_ON);
#endif
}

bool GUI::init = false;
bool GUI::statusLed = false;

#ifdef M5STICK
bool GUI::dirty = false;
#endif
