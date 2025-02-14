#pragma once

#include <mbgl/util/string.hpp>

#include <mapbox/compatibility/value.hpp>

#include <optional>

namespace mbgl {

using Value = mapbox::base::Value;
using NullValue = mapbox::base::NullValue;
using PropertyMap = mapbox::base::ValueObject;
using FeatureIdentifier = mapbox::feature::identifier;
using GeoJSONFeature = mapbox::feature::feature<double>;
using FeatureState = mapbox::base::ValueObject;
using FeatureStates = std::unordered_map<std::string, FeatureState>;       // <featureID, FeatureState>
using LayerFeatureStates = std::unordered_map<std::string, FeatureStates>; // <sourceLayer, FeatureStates>

class Feature : public GeoJSONFeature {
public:
    std::string source;
    std::string sourceLayer;
    PropertyMap state;

    using GeometryType = mapbox::geometry::geometry<double>;

    Feature() = default;
    Feature(const GeoJSONFeature& f) : GeoJSONFeature(f) {}
    Feature(const GeometryType& geom_) : GeoJSONFeature(geom_) {}
    Feature(GeometryType&& geom_) : GeoJSONFeature(std::move(geom_)) {}
};

template <class T>
std::optional<T> numericValue(const Value& value) {
    return value.match([](uint64_t t) { return std::optional<T>(static_cast<T>(t)); },
                       [](int64_t t) { return std::optional<T>(static_cast<T>(t)); },
                       [](double t) { return std::optional<T>(static_cast<T>(t)); },
                       [](const auto&) { return std::optional<T>(); });
}

inline std::optional<std::string> featureIDtoString(const FeatureIdentifier& id) {
    if (id.is<NullValue>()) {
        return std::nullopt;
    }

    return id.match(
        [](const std::string& value_) { return value_; }, [](uint64_t value_) { return util::toString(value_); },
        [](int64_t value_) { return util::toString(value_); }, [](double value_) { return util::toString(value_); },
[](const auto&) -> std::optional<std::string> { return std::nullopt; });
}

} // namespace mbgl
