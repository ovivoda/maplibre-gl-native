#include <mbgl/gl/custom_layer.hpp>
#include <mbgl/gl/custom_layer_impl.hpp>
#include <mbgl/gl/render_custom_layer.hpp>
#include <mbgl/style/layer_observer.hpp>

namespace mbgl {
namespace style {

namespace {
const LayerTypeInfo typeInfoCustom{"custom",
                                   LayerTypeInfo::Source::NotRequired,
                                   LayerTypeInfo::Pass3D::NotRequired,
                                   LayerTypeInfo::Layout::NotRequired,
                                   LayerTypeInfo::FadingTiles::NotRequired,
                                   LayerTypeInfo::CrossTileIndex::NotRequired,
                                   LayerTypeInfo::TileKind::NotRequired};
}  // namespace

CustomLayer::CustomLayer(const std::string& layerID,
                         std::unique_ptr<CustomLayerHost> host)
    : Layer(makeMutable<Impl>(layerID, std::move(host))) {
}

CustomLayer::~CustomLayer() = default;

const CustomLayer::Impl& CustomLayer::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

Mutable<CustomLayer::Impl> CustomLayer::mutableImpl() const {
    return makeMutable<Impl>(impl());
}

std::unique_ptr<Layer> CustomLayer::cloneRef(const std::string&) const {
    assert(false);
    return nullptr;
}

using namespace conversion;

std::optional<Error> CustomLayer::setPropertyInternal(const std::string&, const Convertible&) {
    return Error { "layer doesn't support this property" };
}

StyleProperty CustomLayer::getProperty(const std::string&) const {
    return {};
}

Mutable<Layer::Impl> CustomLayer::mutableBaseImpl() const {
    return staticMutableCast<Layer::Impl>(mutableImpl());
}

// static
const LayerTypeInfo* CustomLayer::Impl::staticTypeInfo() noexcept {
    return &typeInfoCustom;
}

} // namespace style
} // namespace mbgl
