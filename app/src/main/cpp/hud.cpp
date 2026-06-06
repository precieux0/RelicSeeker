#include "hud.h"
#include "font.h"
#include <cstdio>
#include <string>

Hud& Hud::get() { static Hud h; return h; }

void Hud::draw(int health, int maxHealth, int level, const char* levelName,
                 int keys, int keysNeeded, bool paused) {
    Font& f = Font::get();

    char buf[128];
    snprintf(buf, sizeof(buf), "NIV %d  %s", level, levelName ? levelName : "");
    f.drawText(buf, -0.95f, 0.88f, 0.042f, 1.0f, 0.92f, 0.6f, 1.0f);

    // Barre de vie
    f.drawRect(-0.95f, 0.78f, 0.42f, 0.04f, 0.15f, 0.1f, 0.1f, 0.7f);
    float hpFrac = maxHealth > 0 ? (float)health / (float)maxHealth : 0;
    if (hpFrac < 0) hpFrac = 0;
    if (hpFrac > 1) hpFrac = 1;
    float r = hpFrac > 0.5f ? 0.2f : 0.9f;
    float g = hpFrac > 0.5f ? 0.75f : 0.25f;
    f.drawRect(-0.95f, 0.78f, 0.42f * hpFrac, 0.04f, r, g, 0.15f, 0.95f);
    snprintf(buf, sizeof(buf), "PV %d", health);
    f.drawText(buf, -0.50f, 0.775f, 0.032f, 1, 1, 1, 1);

    snprintf(buf, sizeof(buf), "Cles %d/%d", keys, keysNeeded);
    f.drawText(buf, 0.55f, 0.88f, 0.04f, 1.0f, 0.85f, 0.2f, 1.0f);

    if (paused) {
        f.drawRect(-0.5f, -0.08f, 1.0f, 0.16f, 0, 0, 0, 0.65f);
        f.drawText("PAUSE", -0.18f, -0.02f, 0.07f, 1, 0.9f, 0.3f, 1);
        f.drawText("Action: reprendre", -0.35f, -0.12f, 0.035f, 0.8f, 0.8f, 0.8f, 1);
    }
}
