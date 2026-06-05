#include "story.h"
#include "renderer.h"

Story& Story::get() { static Story s; return s; }
void Story::trigger(const std::string& e) {
    if (e == "enter_tomb") mCurrentText = "Le tombeau s'ouvre... Trouvez les trois clés.";
    else if (e == "key_found") mCurrentText = "Vous avez trouvé une clé ancienne.";
    else if (e == "deeper") mCurrentText = "Un passage secret se révèle...";
    else if (e == "final_boss") mCurrentText = "Le gardien millénaire se dresse devant vous!";
    else if (e == "game_win") mCurrentText = "Victoire! L'artefact est à vous. Fin alternative selon objets.";
    else if (e == "game_over") mCurrentText = "GAME OVER... Recommencez?";
    mTimer = 4.0f;
}
void Story::update(float dt) { if (mTimer>0) mTimer -= dt; }
void Story::render(Renderer& r) { /* affichage d'un texte à l'écran avec OpenGL (simplifié) */ }
bool Story::isActive() const { return mTimer>0; }
