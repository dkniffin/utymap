#ifndef INDEX_GEOSTORE_HPP_DEFINED
#define INDEX_GEOSTORE_HPP_DEFINED

#include "BoundingBox.hpp"
#include "GeoCoordinate.hpp"
#include "QuadKey.hpp"
#include "entities/Element.hpp"
#include "entities/ElementVisitor.hpp"
#include "formats/FormatTypes.hpp"
#include "index/ElementStore.hpp"
#include "index/StringTable.hpp"
#include "LodRange.hpp"

#include "mapcss/StyleSheet.hpp"
#include "mapcss/StyleProvider.hpp"

#include <string>
#include <memory>

namespace utymap { namespace index {

// Provides API to store and access geo data using different underlying data stores.
class GeoStore
{
public:
    GeoStore(utymap::index::StringTable& stringTable);

    ~GeoStore();

    // Adds underlying element store for usage.
    void registerStore(const std::string& storeKey, ElementStore& store);

    // Adds element to selected store.
    void add(const std::string& storeKey, 
             const utymap::entities::Element& element,
             const utymap::LodRange& range, 
             const utymap::mapcss::StyleProvider& styleProvider);

    // Adds all data from file to selected store in given level of detail range.
    void add(const std::string& storeKey, 
             const std::string& path,
             const utymap::LodRange& range, 
             const utymap::mapcss::StyleProvider& styleProvider);

    // Adds all data from file to selected store in given quad key.
    void add(const std::string& storeKey,
             const std::string& path,
             const utymap::QuadKey& quadKey,
             const utymap::mapcss::StyleProvider& styleProvider);

    // Adds all data from file to selected store in given boundging box.
    void add(const std::string& storeKey,
             const std::string& path,
             const utymap::BoundingBox& bbox,
             const utymap::LodRange& range,
             const utymap::mapcss::StyleProvider& styleProvider);

    // Searches for elements inside quadkey.
    void search(const QuadKey& quadKey,
                const utymap::mapcss::StyleProvider& styleProvider,
                utymap::entities::ElementVisitor& visitor);

    // Searches for elements inside circle with given parameters.
    void search(const GeoCoordinate& coordinate,
                double radius,
                const utymap::mapcss::StyleProvider& styleProvider,
                utymap::entities::ElementVisitor& visitor);

    // Checks whether there is data for given quadkey.
    bool hasData(const QuadKey& quadKey);

private:
    class GeoStoreImpl;
    std::unique_ptr<GeoStoreImpl> pimpl_;
};

}}

#endif // INDEX_GEOSTORE_HPP_DEFINED
