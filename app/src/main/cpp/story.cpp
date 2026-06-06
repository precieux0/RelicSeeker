#include "story.h"
#include "renderer.h"
#include "font.h"

Story& Story::get() { static Story s; return s; }

void Story::trigger(const std::string& e) {
    if (e == "level_1") mCurrentText = "Les ruines jumelles s'ouvrent. Trouvez la relique.";
    else if (e == "level_2") mCurrentText = "Les catacombes murmurent. Avancez prudemment.";
    else if (e == "level_3") mCurrentText = "L'eau recouvre d'anciens secrets.";
    else if (e == "level_4") mCurrentText = "Pieges mecaniques! Un faux pas et c'est la mort.";
    else if (e == "level_5") mCurrentText = "Le serpent sacre observe chaque mouvement.";
    else if (e == "level_6") mCurrentText = "Anubis juge les intrus. Preparez-vous.";
    else if (e == "level_7") mCurrentText = "Les cristaux revelent le chemin du tresor.";
    else if (e == "level_8") mCurrentText = "Le gardien millenaire se reveille!";
    else if (e == "level_complete") mCurrentText = "Passage deverrouille. Niveau suivant...";
    else if (e == "key_found") mCurrentText = "Relique ancienne recuperee!";
    else if (e == "game_win") mCurrentText = "Victoire! L'Echo de l'Eternite est a vous.";
    else if (e == "game_over") mCurrentText = "Vous avez succombe. Retour au camp.";
    else mCurrentText = e;
    mTimer = 4.5f;
}

void Story::update(float dt) {
    if (mTimer > 0) mTimer -= dt;
}

void Story::render(Renderer& r) {
    (void)r;
    if (!isActive() || mCurrentText.empty()) return;
    Font::get().drawRect(-0.92f, 0.68f, 1.84f, 0.12f, 0, 0, 0, 0.55f);
    Font::get().drawText(mCurrentText, -0.88f, 0.70f, 0.042f, 1.0f, 0.95f, 0.7f, 1.0f);
}

bool Story::isActive() const { return mTimer > 0; }
