#pragma once
#include "th/tools/INIReader.h"
#include <string>

struct DecayConfig {
    bool enabled{};
    double base_interval{};
    double min_interval{};
    double min_damage{};
    double max_damage{};
};

struct SanityConfig {
    bool enabled{};
    bool show_hud{};
    double sanity_percentage_flashes_fire_automatically{};
    double flash_fire_health_percentage_threshold{};
    double director_timer{};
    double director_booster{};
    double volume_modifier{};
};

struct RogueliteConfig {
    bool loss_scrap_on_death{};
    bool empty_canteen_on_loading{};
    bool loss_fuel_on_car_damage{};
    double fuel_modifier_on_loading{};
    double loss_fuel_on_car_damage_factor{};
};

struct PluginConfig {
    DecayConfig decay;
    SanityConfig sanity;
    RogueliteConfig roguelite;
};

inline PluginConfig loadConfig(const std::string& filename) {
    INIReader reader(filename);
    PluginConfig cfg;

    cfg.decay.enabled = reader.GetBoolean("decay", "enabled", false);
    cfg.decay.base_interval = reader.GetReal("decay", "base_interval", 0.0);
    cfg.decay.min_interval = reader.GetReal("decay", "min_interval", 0.0);
    cfg.decay.min_damage = reader.GetReal("decay", "min_damage", 0.0);
    cfg.decay.max_damage = reader.GetReal("decay", "max_damage", 0.0);

    cfg.sanity.enabled = reader.GetBoolean("sanity", "enabled", false);
    cfg.sanity.show_hud = reader.GetBoolean("sanity", "show_hud", false);
    cfg.sanity.sanity_percentage_flashes_fire_automatically = reader.GetReal("sanity", "sanity_percentage_flashes_fire_automatically", 0.0);
    cfg.sanity.flash_fire_health_percentage_threshold = reader.GetReal("sanity", "flash_fire_health_percentage_threshold", 100.0);
    cfg.sanity.director_timer = reader.GetReal("sanity", "director_timer", 0.0);
    cfg.sanity.director_booster = reader.GetReal("sanity", "director_booster", 0.0);
    cfg.sanity.volume_modifier = reader.GetReal("sanity", "volume_modifier", 1.0);

    cfg.roguelite.loss_scrap_on_death = reader.GetBoolean("roguelite", "loss_scrap_on_death", false);
    cfg.roguelite.empty_canteen_on_loading = reader.GetBoolean("roguelite", "empty_canteen_on_loading", false);
    cfg.roguelite.loss_fuel_on_car_damage = reader.GetBoolean("roguelite", "loss_fuel_on_car_damage", false);
    cfg.roguelite.fuel_modifier_on_loading = reader.GetReal("roguelite", "fuel_modifier_on_loading", 1.0);
    cfg.roguelite.loss_fuel_on_car_damage_factor = reader.GetReal("roguelite", "loss_fuel_on_car_damage_factor", 0.0);


    return cfg;
}

class Config {
public:
    static Config& instance() {
        static Config instance;
        return instance;
    }

    void load(const std::string& filename) {
        config_ = loadConfig(filename);
    }

    const PluginConfig& get() const {
        return config_;
    }

    // accesos directos opcionales
    const DecayConfig& decay() const { return config_.decay; }
    const SanityConfig& sanity() const { return config_.sanity; }
    const RogueliteConfig& roguelite() const { return config_.roguelite; }

private:
    Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    PluginConfig config_;
};