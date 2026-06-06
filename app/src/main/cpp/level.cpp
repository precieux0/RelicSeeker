#include "level.h"

static LevelDef makeLevel1() {
    LevelDef l;
    l.name = "Portes du Temple Perdu";
    l.subtitle = "Jungle Maya - Entree des ruines";
    l.skyColor = vec3(0.12f, 0.18f, 0.28f);
    l.fogColor = vec3(0.08f, 0.14f, 0.10f);
    l.fogDensity = 0.018f;
    l.lightDir = vec3(0.4f, 1.0f, 0.3f).normalized();
    l.groundColor = vec3(0.32f, 0.28f, 0.20f);
    l.playerStart = vec3(0, 0.5f, 8);
    l.storyEvent = "level_1";
    l.props = {
        {{ -6, 0, -4 }, { 1.2f, 3.5f, 1.2f }, { 0.55f, 0.50f, 0.42f }, PROP_PILLAR, 0 },
        {{  6, 0, -4 }, { 1.2f, 3.5f, 1.2f }, { 0.55f, 0.50f, 0.42f }, PROP_PILLAR, 0 },
        {{  0, 0, -8 }, { 8.0f, 0.8f, 1.0f }, { 0.48f, 0.44f, 0.38f }, PROP_WALL, 0 },
        {{ -4, 0,  2 }, { 0.8f, 2.0f, 0.8f }, { 0.50f, 0.45f, 0.38f }, PROP_COLUMN, 0 },
        {{  4, 0,  2 }, { 0.8f, 2.0f, 0.8f }, { 0.50f, 0.45f, 0.38f }, PROP_COLUMN, 0 },
        {{ -7, 1.5f, 0 }, { 0.3f, 0.3f, 0.3f }, { 1.0f, 0.7f, 0.2f }, PROP_TORCH, 0.8f },
        {{  7, 1.5f, 0 }, { 0.3f, 0.3f, 0.3f }, { 1.0f, 0.7f, 0.2f }, PROP_TORCH, 0.8f },
    };
    l.enemies = { {{3,0,2},0,1.1f}, {{-2,0,4},1,1.4f} };
    l.keys = { vec3(5, 0.6f, -2) };
    return l;
}

static LevelDef makeLevel2() {
    LevelDef l;
    l.name = "Catacombes de Sable";
    l.subtitle = "Couloirs etendus sous le desert";
    l.skyColor = vec3(0.10f, 0.08f, 0.12f);
    l.fogColor = vec3(0.18f, 0.12f, 0.08f);
    l.fogDensity = 0.025f;
    l.lightDir = vec3(-0.2f, 1.0f, 0.5f).normalized();
    l.groundColor = vec3(0.38f, 0.30f, 0.22f);
    l.playerStart = vec3(0, 0.5f, 10);
    l.storyEvent = "level_2";
    for (int i = -3; i <= 3; i += 2) {
        l.props.push_back({{ (float)i * 2.5f, 0, -6 }, { 1.0f, 3.0f, 1.0f }, { 0.52f, 0.46f, 0.36f }, PROP_PILLAR, 0 });
        l.props.push_back({{ (float)i * 2.5f, 0,  6 }, { 1.0f, 3.0f, 1.0f }, { 0.52f, 0.46f, 0.36f }, PROP_PILLAR, 0 });
    }
    l.props.push_back({{0,0,0},{2.5f,1.2f,2.5f},{0.45f,0.38f,0.30f},PROP_ALTAR,0});
    l.props.push_back({{-8,1.2f,-2},{0.25f,0.25f,0.25f},{1.0f,0.6f,0.15f},PROP_TORCH,0.9f});
    l.enemies = { {{0,0,3},0,1.2f}, {{4,0,-1},1,1.5f}, {{-4,0,1},1,1.5f} };
    l.keys = { vec3(-6, 0.6f, -4), vec3(6, 0.6f, -4) };
    return l;
}

static LevelDef makeLevel3() {
    LevelDef l;
    l.name = "Crypte Engloutie";
    l.subtitle = "Eaux stagnantes et pierre moussue";
    l.skyColor = vec3(0.04f, 0.08f, 0.14f);
    l.fogColor = vec3(0.05f, 0.12f, 0.18f);
    l.fogDensity = 0.035f;
    l.lightDir = vec3(0.1f, 1.0f, -0.3f).normalized();
    l.groundColor = vec3(0.22f, 0.28f, 0.30f);
    l.playerStart = vec3(-5, 0.5f, 0);
    l.storyEvent = "level_3";
    l.props = {
        {{0,0,-7},{10,0.5f,1},{0.30f,0.35f,0.38f},PROP_WALL,0},
        {{0,0,7},{10,0.5f,1},{0.30f,0.35f,0.38f},PROP_WALL,0},
        {{-6,0,0},{1,2.5f,6},{0.35f,0.40f,0.42f},PROP_WALL,0},
        {{6,0,0},{1,2.5f,6},{0.35f,0.40f,0.42f},PROP_WALL,0},
        {{0,0,0},{1.8f,0.6f,1.8f},{0.55f,0.48f,0.35f},PROP_ALTAR,0},
        {{-3,1.0f,3},{0.2f,0.2f,0.2f},{0.3f,0.7f,1.0f},PROP_TORCH,0.7f},
        {{3,1.0f,-3},{0.2f,0.2f,0.2f},{0.3f,0.7f,1.0f},PROP_TORCH,0.7f},
    };
    l.enemies = { {{2,0,2},1,1.6f}, {{-2,0,-2},1,1.6f}, {{0,0,-4},0,1.3f} };
    l.keys = { vec3(0, 0.7f, -5) };
    return l;
}

static LevelDef makeLevel4() {
    LevelDef l;
    l.name = "Galerie des Pieges";
    l.subtitle = "Mecanismes mortels du passe";
    l.skyColor = vec3(0.08f, 0.06f, 0.08f);
    l.fogColor = vec3(0.12f, 0.08f, 0.06f);
    l.fogDensity = 0.028f;
    l.lightDir = vec3(0.6f, 0.8f, 0.2f).normalized();
    l.groundColor = vec3(0.35f, 0.28f, 0.22f);
    l.playerStart = vec3(0, 0.5f, 11);
    l.storyEvent = "level_4";
    for (int z = -8; z <= 8; z += 4) {
        l.props.push_back({{ -3.5f, 0, (float)z }, { 0.6f, 2.8f, 0.6f }, { 0.60f, 0.25f, 0.20f }, PROP_PILLAR, 0 });
        l.props.push_back({{  3.5f, 0, (float)z }, { 0.6f, 2.8f, 0.6f }, { 0.60f, 0.25f, 0.20f }, PROP_PILLAR, 0 });
    }
    l.props.push_back({{0,2.5f,0},{5,0.4f,0.8f},{0.50f,0.42f,0.35f},PROP_WALL,0});
    l.enemies = { {{0,0,5},1,1.7f}, {{0,0,-2},1,1.7f}, {{3,0,0},0,1.4f} };
    l.keys = { vec3(0, 0.6f, -7), vec3(-5, 0.6f, 4) };
    return l;
}

static LevelDef makeLevel5() {
    LevelDef l;
    l.name = "Sanctuaire du Serpent";
    l.subtitle = "Idoles et murmures anciens";
    l.skyColor = vec3(0.06f, 0.10f, 0.06f);
    l.fogColor = vec3(0.08f, 0.14f, 0.08f);
    l.fogDensity = 0.030f;
    l.lightDir = vec3(-0.4f, 1.0f, 0.2f).normalized();
    l.groundColor = vec3(0.26f, 0.32f, 0.24f);
    l.playerStart = vec3(0, 0.5f, 9);
    l.storyEvent = "level_5";
    l.props = {
        {{-5,0,-3},{1.5f,4.0f,1.5f},{0.40f,0.48f,0.38f},PROP_STATUE,0},
        {{5,0,-3},{1.5f,4.0f,1.5f},{0.40f,0.48f,0.38f},PROP_STATUE,0},
        {{0,0,-6},{6,1.0f,1},{0.42f,0.50f,0.40f},PROP_WALL,0},
        {{0,0,0},{3,0.8f,3},{0.38f,0.45f,0.35f},PROP_ALTAR,0},
        {{-6,1.5f,4},{0.3f,0.3f,0.3f},{0.9f,0.8f,0.2f},PROP_TORCH,0.85f},
        {{6,1.5f,4},{0.3f,0.3f,0.3f},{0.9f,0.8f,0.2f},PROP_TORCH,0.85f},
    };
    l.enemies = { {{2,0,3},1,1.8f}, {{-2,0,3},1,1.8f}, {{4,0,-1},2,1.2f}, {{-4,0,-1},2,1.2f} };
    l.keys = { vec3(0, 0.8f, -4) };
    return l;
}

static LevelDef makeLevel6() {
    LevelDef l;
    l.name = "Chambre d'Anubis";
    l.subtitle = "Le juge des morts veille";
    l.skyColor = vec3(0.05f, 0.04f, 0.10f);
    l.fogColor = vec3(0.10f, 0.06f, 0.14f);
    l.fogDensity = 0.032f;
    l.lightDir = vec3(0.2f, 0.9f, 0.4f).normalized();
    l.groundColor = vec3(0.30f, 0.26f, 0.34f);
    l.playerStart = vec3(0, 0.5f, 10);
    l.storyEvent = "level_6";
    l.props = {
        {{0,0,-5},{8,1.2f,1},{0.48f,0.40f,0.55f},PROP_WALL,0},
        {{-4,0,0},{1.2f,3.5f,1.2f},{0.55f,0.48f,0.60f},PROP_PILLAR,0},
        {{4,0,0},{1.2f,3.5f,1.2f},{0.55f,0.48f,0.60f},PROP_PILLAR,0},
        {{0,0,0},{2.5f,1.5f,2.5f},{0.60f,0.50f,0.20f},PROP_ALTAR,0},
        {{0,2.0f,0},{0.5f,0.5f,0.5f},{1.0f,0.85f,0.3f},PROP_TORCH,1.0f},
    };
    l.enemies = { {{0,0,2},2,1.0f}, {{3,0,-2},0,1.5f}, {{-3,0,-2},0,1.5f}, {{0,0,-3},3,0.9f} };
    l.keys = { vec3(0, 1.0f, -4) };
    return l;
}

static LevelDef makeLevel7() {
    LevelDef l;
    l.name = "Vault de Cristal";
    l.subtitle = "Reliques scintillantes";
    l.skyColor = vec3(0.08f, 0.12f, 0.20f);
    l.fogColor = vec3(0.10f, 0.16f, 0.24f);
    l.fogDensity = 0.022f;
    l.lightDir = vec3(0.3f, 1.0f, 0.6f).normalized();
    l.groundColor = vec3(0.28f, 0.32f, 0.38f);
    l.playerStart = vec3(-6, 0.5f, 6);
    l.storyEvent = "level_7";
    for (int i = -2; i <= 2; ++i) {
        float x = i * 3.0f;
        l.props.push_back({{ x, 0, -4 }, { 0.8f, 2.2f, 0.8f }, { 0.45f, 0.55f, 0.65f }, PROP_COLUMN, 0 });
        l.props.push_back({{ x, 1.2f, 0 }, { 0.25f, 0.25f, 0.25f }, { 0.5f, 0.8f, 1.0f }, PROP_TORCH, 0.9f });
    }
    l.enemies = { {{2,0,2},1,1.9f}, {{-2,0,2},1,1.9f}, {{0,0,-2},2,1.3f} };
    l.keys = { vec3(5, 0.7f, -5), vec3(-5, 0.7f, -5), vec3(0, 0.7f, -7) };
    return l;
}

static LevelDef makeLevel8() {
    LevelDef l;
    l.name = "Sanctuaire Interieur";
    l.subtitle = "L'Echo de l'Eternite - Boss final";
    l.skyColor = vec3(0.04f, 0.02f, 0.06f);
    l.fogColor = vec3(0.08f, 0.04f, 0.10f);
    l.fogDensity = 0.040f;
    l.lightDir = vec3(0.0f, 1.0f, 0.0f).normalized();
    l.groundColor = vec3(0.25f, 0.20f, 0.28f);
    l.playerStart = vec3(0, 0.5f, 12);
    l.storyEvent = "level_8";
    l.props = {
        {{0,0,-8},{12,2.0f,1},{0.50f,0.42f,0.55f},PROP_WALL,0},
        {{-7,0,0},{1.5f,5.0f,1.5f},{0.52f,0.45f,0.58f},PROP_PILLAR,0},
        {{7,0,0},{1.5f,5.0f,1.5f},{0.52f,0.45f,0.58f},PROP_PILLAR,0},
        {{0,0,0},{4,1.0f,4},{0.55f,0.45f,0.25f},PROP_ALTAR,0},
        {{0,3.0f,0},{1,1,1},{1.0f,0.9f,0.4f},PROP_TORCH,1.2f},
        {{-5,0,-4},{1.8f,3.5f,1.8f},{0.45f,0.38f,0.50f},PROP_STATUE,0},
        {{5,0,-4},{1.8f,3.5f,1.8f},{0.45f,0.38f,0.50f},PROP_STATUE,0},
    };
    l.enemies = { {{0,0,0},3,0.7f}, {{4,0,3},0,1.4f}, {{-4,0,3},0,1.4f}, {{0,0,-5},2,1.5f} };
    l.keys = { vec3(0, 1.2f, -6) };
    return l;
}

const LevelDef& LevelData::get(int level) {
    static LevelDef levels[TOTAL_LEVELS] = {
        makeLevel1(), makeLevel2(), makeLevel3(), makeLevel4(),
        makeLevel5(), makeLevel6(), makeLevel7(), makeLevel8()
    };
    int idx = level - 1;
    if (idx < 0) idx = 0;
    if (idx >= TOTAL_LEVELS) idx = TOTAL_LEVELS - 1;
    return levels[idx];
}
