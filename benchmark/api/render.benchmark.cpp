#include <benchmark/benchmark.h>

#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_observer.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/storage/network_status.hpp>
#include <mbgl/storage/resource_options.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layers/symbol_layer.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <sstream>
#include <optional>

using namespace mbgl;

namespace {

static std::string cachePath { "benchmark/fixtures/api/cache.db" };
constexpr double pixelRatio { 1.0 };
constexpr Size size { 1000, 1000 };

class RenderBenchmark {
public:
    RenderBenchmark() {
        NetworkStatus::Set(NetworkStatus::Status::Offline);
    }

    util::RunLoop loop;
};

void prepare(Map& map, std::optional<std::string> json = std::nullopt) {
    map.getStyle().loadJSON(json ? *json : util::read_file("benchmark/fixtures/api/style.json"));
    map.jumpTo(CameraOptions().withCenter(LatLng { 40.726989, -73.992857 }).withZoom(15.0)); // Manhattan

    auto image = decodeImage(util::read_file("benchmark/fixtures/api/default_marker.png"));
    map.getStyle().addImage(std::make_unique<style::Image>("test-icon", std::move(image), 1.0f));
}

void prepare_map2(Map& map, std::optional<std::string> json = std::nullopt) {
    map.getStyle().loadJSON(json ? *json : util::read_file("benchmark/fixtures/api/style.json"));
    map.jumpTo(CameraOptions().withCenter(LatLng{41.379800, 2.176810}).withZoom(15.0)); // Barcelona

    auto image = decodeImage(util::read_file("benchmark/fixtures/api/default_marker.png"));
    map.getStyle().addImage(std::make_unique<style::Image>("test-icon", std::move(image), 1.0f));
}

} // end namespace

static void API_renderStill_reuse_map(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { size, pixelRatio };
    Map map { frontend, MapObserver::nullObserver(),
              MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
              ResourceOptions().withCachePath(cachePath).withApiKey("foobar") };
    prepare(map);

    for (auto _ : state) {
        frontend.render(map);
    }
}

static void API_renderStill_reuse_map_formatted_labels(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { size, pixelRatio };
    Map map { frontend, MapObserver::nullObserver(),
              MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
              ResourceOptions().withCachePath(cachePath).withApiKey("foobar") };
    prepare(map, util::read_file("benchmark/fixtures/api/style_formatted_labels.json"));

    for (auto _ : state) {
        frontend.render(map);
    }
}

static void API_renderStill_reuse_map_switch_styles(::benchmark::State& state) {
    RenderBenchmark bench;
    HeadlessFrontend frontend { size, pixelRatio };
    Map map { frontend, MapObserver::nullObserver(),
              MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
              ResourceOptions().withCachePath(cachePath).withApiKey("foobar") };

    for (auto _ : state) {
        prepare(map, { "{}" });
        frontend.render(map);
        prepare(map);
        frontend.render(map);
    }
}

static void API_renderStill_recreate_map(::benchmark::State& state) {
    RenderBenchmark bench;

    for (auto _ : state) {
        HeadlessFrontend frontend { size, pixelRatio };
        Map map { frontend, MapObserver::nullObserver(),
                  MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
                  ResourceOptions().withCachePath(cachePath).withApiKey("foobar") };
        prepare(map);
        frontend.render(map);
    }
}

static void API_renderStill_recreate_map_2(::benchmark::State& state) {
    RenderBenchmark bench;

    for (auto _ : state) {
        HeadlessFrontend frontend{size, pixelRatio};
        Map map{frontend,
                MapObserver::nullObserver(),
                MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
                ResourceOptions().withCachePath(cachePath).withApiKey("foobar")};
        prepare_map2(map);
        frontend.render(map);
    }
}

static void API_renderStill_multiple_sources(::benchmark::State& state) {
    using namespace mbgl::style;
    RenderBenchmark bench;
    HeadlessFrontend frontend{size, pixelRatio};
    Map map{frontend,
            MapObserver::nullObserver(),
            MapOptions().withMapMode(MapMode::Static).withSize(size).withPixelRatio(pixelRatio),
            ResourceOptions().withCachePath(cachePath).withApiKey("foobar")};
    prepare(map);
    auto& style = map.getStyle();
    const int kSourcesCount = 50;
    const int kLayersCount = 50;
    for (int i = 0; i < kSourcesCount; ++i) {
        std::ostringstream sourceOss;
        sourceOss << "GeoJSONSource" << i;
        std::string sourceId{sourceOss.str()};
        auto source = std::make_unique<GeoJSONSource>(sourceId);
        style.addSource(std::move(source));
        for (int j = 0; j < kLayersCount; ++j) {
            std::ostringstream layerOss;
            layerOss << sourceId << '#' << j;
            auto layer = std::make_unique<SymbolLayer>(layerOss.str(), sourceId);
            style.addLayer(std::move(layer));
        }
    }

    for (auto _ : state) {
        frontend.render(map);
    }
}

BENCHMARK(API_renderStill_reuse_map)->Unit(benchmark::kMillisecond)->Iterations(50);
BENCHMARK(API_renderStill_reuse_map_formatted_labels)->Unit(benchmark::kMillisecond)->Iterations(50);
BENCHMARK(API_renderStill_reuse_map_switch_styles)->Unit(benchmark::kMillisecond)->Iterations(50);
BENCHMARK(API_renderStill_recreate_map)->Unit(benchmark::kMillisecond)->Iterations(50);
BENCHMARK(API_renderStill_recreate_map_2)->Unit(benchmark::kMillisecond)->Iterations(50);
BENCHMARK(API_renderStill_multiple_sources)->Unit(benchmark::kMillisecond)->Iterations(50);
