#pragma once
#include "utils.h"
#include <vector>
#include <string>

static const int TOTAL_LEVELS = 8;

enum PropType {
    PROP_PILLAR,
    PROP_WALL,
    PROP_ALTAR,
    PROP_TORCH,
    PROP_COLUMN,
    PROP_STATUE
};

struct LevelProp {
    vec3 pos;
    vec3 scale;
    vec3 color;
    PropType type;
    float emissive;
};

struct LevelSpawn {
    vec3 pos;
    int type; // EnemyType
    float speed;
};

struct LevelDef {
    std::string name;
    std::string subtitle;
    vec3 skyColor;
    vec3 fogColor;
    float fogDensity;
    vec3 lightDir;
    vec3 groundColor;
    vec3 playerStart;
    std::vector<LevelProp> props;
    std::vector<LevelSpawn> enemies;
    std::vector<vec3> keys;
    std::string storyEvent;
};

class LevelData {
public:
    static const LevelDef& get(int level);
    static int count() { return TOTAL_LEVELS; }
};
